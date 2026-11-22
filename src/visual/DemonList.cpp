#include <Geode/Geode.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/modify/LevelCell.hpp>
#include <Geode/binding/LevelBrowserLayer.hpp>
#include <Geode/utils/web.hpp>
#include <Geode/utils/async.hpp>
#include <Geode/ui/Label.hpp>
#include <array>
#include <charconv>
#include <ctime>
#include <map>
#include <string>
#include <vector>

using namespace geode::prelude;

namespace {
    int parsePositiveInt(std::string_view str) {
        int result = 0;
        auto begin = str.data();
        auto end = begin + str.size();
        auto [ptr, err] = std::from_chars(begin, end, result);
        return err == std::errc{} && ptr == end && result > 0 ? result : 0;
    }

    class DemonListManager {
        std::array<std::map<int, std::string>, 2> m_rankCache;
        std::array<bool, 2> m_downloaded{};
        std::array<bool, 2> m_downloading{};
        std::array<async::TaskHolder<web::WebResponse>, 2> m_requests;
    public:
        static DemonListManager* get() {
            static auto instance = new DemonListManager;
            return instance;
        }

        void fetchList(int type) {
            if (type != 0 && type != 1) return;
            if (m_downloaded[type] || m_downloading[type]) return;
            m_downloading[type] = true;

            auto url = fmt::format(
                "https://salamandra.ps.fhgdps.com/main/demonList.php?type={}&t={}",
                type, std::time(nullptr));

            m_requests[type].spawn(web::WebRequest().get(url),
                [this, type](web::WebResponse res) {
                    m_downloading[type] = false;
                    if (!res.ok()) {
                        log::warn("Demon list download failed (HTTP {})", res.code());
                        return;
                    }
                    auto content = res.string();
                    if (!content) return;

                    std::map<int, std::string> incoming;
                    for (auto const& pair : string::split(content.unwrap(), ",")) {
                        auto parts = string::split(pair, ":");
                        if (parts.size() != 2) continue;
                        auto levelID = parsePositiveInt(string::trim(parts[0]));
                        auto rank = string::trim(parts[1]);
                        if (levelID && !rank.empty()) incoming[levelID] = rank;
                    }
                    m_rankCache[type] = std::move(incoming);
                    m_downloaded[type] = true;
                    if (auto scene = CCScene::get()) {
                        if (auto browser = scene->getChildByType<LevelBrowserLayer>(0)) {
                            if (browser->m_searchObject)
                                browser->loadPage(browser->m_searchObject);
                        }
                    }
                });
        }

        std::string getRank(int levelID, int type) const {
            if (type != 0 && type != 1) return {};
            auto const& cache = m_rankCache[type];
            auto it = cache.find(levelID);
            return it == cache.end() ? std::string{} : it->second;
        }
    };

    CCNode* createRankNode(std::string const& rank, float scale) {
        auto container = CCNode::create();
        if (!container) return nullptr;
        container->setID("rank-badge"_spr);

        auto label = geode::Label::create(rank, "bigFont.fnt");
        if (!label) return container;
        label->setScale(scale);
        label->setAnchorPoint({0.f, 0.5f});
        label->setPosition({0.f, 0.f});
        container->addChild(label);

        int position = parsePositiveInt(rank);
        char const* texture = "rankIcon_all_001.png";
        if      (position == 1)  texture = "rankIcon_1_001.png";
        else if (position <= 5)  texture = "rankIcon_top10_001.png";
        else if (position <= 10) texture = "rankIcon_top50_001.png";
        else if (position <= 15) texture = "rankIcon_top200_001.png";
        else if (position <= 20) texture = "rankIcon_top500_001.png";
        else if (position <= 25) texture = "rankIcon_top1000_001.png";
        else if (position <= 30) texture = "rankIcon_top2500_001.png";
        else if (position <= 35) texture = "rankIcon_top5000_001.png";
        else if (position <= 40) texture = "rankIcon_top10000_001.png";

        auto labelWidth = label->getContentSize().width * scale;
        float totalWidth = labelWidth;
        if (auto trophy = CCSprite::createWithSpriteFrameName(texture)) {
            auto currentWidth = trophy->getContentSize().width;
            if (currentWidth > 0.f) {
                trophy->setScale(18.f / currentWidth * (scale / .5f));
                trophy->setAnchorPoint({0.f, 0.5f});
                trophy->setPosition({labelWidth + 1.5f, 0.f});
                totalWidth += 1.5f + currentWidth * trophy->getScale();
                container->addChild(trophy);
            }
        }
        container->setContentSize({totalWidth, 0.f});
        return container;
    }
}

class $modify(SalamandraDemonLevelCell, LevelCell) {
    $override void loadFromLevel(GJGameLevel* level) {
        LevelCell::loadFromLevel(level);
        if (!level) return;
        auto menu = getChildByIDRecursive("main-menu");
        if (!menu) return;
        if (auto old = menu->getChildByID("rank-badge"_spr)) old->removeFromParent();
        if (!level->m_demon) return;

        int type = level->isPlatformer() ? 1 : 0;
        auto manager = DemonListManager::get();
        auto rank = manager->getRank(level->m_levelID, type);
        if (rank.empty()) {
            manager->fetchList(type);
            return;
        }
        auto button = menu->getChildByID("view-button");
        if (!button) button = menu->getChildByID("get-button");
        if (!button) return;

        if (auto badge = createRankNode(rank, 0.55f)) {
            auto width = badge->getContentSize().width;
            badge->setPosition({button->getPositionX() - width / 2.f,
                                button->getPositionY() - 30.f});
            menu->addChild(badge);
        }
    }
};

class $modify(SalamandraDemonLevelInfo, LevelInfoLayer) {
    $override bool init(GJGameLevel* level, bool p1) {
        if (!LevelInfoLayer::init(level, p1)) return false;
        if (!level || !level->m_demon) return true;

        int type = level->isPlatformer() ? 1 : 0;
        auto manager = DemonListManager::get();
        auto rank = manager->getRank(level->m_levelID, type);
        if (rank.empty()) {
            manager->fetchList(type);
            return true;
        }
        auto difficulty = m_difficultySprite;
        if (!difficulty || !difficulty->getParent()) return true;
        auto badge = createRankNode(rank, 0.4f);
        if (!badge) return true;

        CCNode* anchor = getChildByID("stars-label");
        if (level->m_coins > 0) {
            for (auto const* id : {"coin-icon-3", "coin-icon-2", "coin-icon-1"}) {
                if (auto coin = getChildByID(id); coin && coin->isVisible()) {
                    anchor = coin;
                    break;
                }
            }
        }
        auto yPos = anchor ? anchor->getPositionY() - 15.f : difficulty->getPositionY() - 38.f;
        badge->setPosition({difficulty->getPositionX() - badge->getContentSize().width / 2.f,
                            yPos});
        difficulty->getParent()->addChild(badge);
        return true;
    }
};
