// Salamandra credits button - Geode v6
// Uses geode::Popup so navigation/buttons participate in Geode's popup input handling.
#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/ui/Popup.hpp>
#include <Geode/ui/Label.hpp>

#include <algorithm>
#include <vector>

using namespace geode::prelude;

namespace {

struct CreditInfo {
    char const* name;
    char const* reason;
    int iconID, color1, color2, glowColor;
    bool glow;
};

class CreditNode : public CCNode {
public:
    static CreditNode* create(CreditInfo const& info) {
        auto node = new CreditNode();
        if (node && node->init(info)) {
            node->autorelease();
            return node;
        }
        delete node;
        return nullptr;
    }

private:
    bool init(CreditInfo const& info) {
        if (!CCNode::init()) return false;

        auto gm = GameManager::get();
        auto icon = SimplePlayer::create(info.iconID);
        if (!gm || !icon) return false;

        icon->setColor(gm->colorForIdx(info.color1));
        icon->setSecondColor(gm->colorForIdx(info.color2));
        if (info.glow) {
            auto color = gm->colorForIdx(info.glowColor);
            icon->setGlowOutline(color);
            icon->enableCustomGlowColor(color);
        }
        icon->setScale(1.2f);
        addChild(icon);

        if (auto name = geode::Label::create(info.name, "goldFont.fnt")) {
            name->setScale(0.6f);
            name->setPosition({0.f, 31.f});
            name->setLimitLabelWidth(125.f, 0.6f, 0.2f);
            addChild(name);
        }
        if (auto reason = geode::Label::create(info.reason, "chatFont.fnt")) {
            reason->setScale(0.7f);
            reason->setPosition({0.f, -30.f});
            reason->setLimitLabelWidth(135.f, 0.7f, 0.2f);
            addChild(reason);
        }
        return true;
    }
};

class SalamandraCreditsPopup : public geode::Popup {
    CCNode* m_creditsContainer = nullptr;
    CCMenu* m_navMenu = nullptr;
    int m_currentPage = 0;

    // Same 12 entries and page layout as in the original mod.
    std::vector<CreditInfo> m_allCredits = {
        {"Izquierdo", "Developer", 99, 12, 3, 3, true},
        {"FHGDPS", "Server Hosting", 268, 8, 12, 11, true},
        {"NightFall", "NGDPS Rights", 85, 12, 16, 12, true},
        {"YuuMai", "Ex Admin", 153, 35, 6, 6, true},
        {"RobTop", "Game Developer", 485, 11, 29, 3, true},
        {"Shion", "Server Booster", 98, 70, 12, 12, true},
        {"Elefoco", "Temp Admin", 98, 5, 10, 10, true},
        {"Cvolton", "Server Files", 1, 30, 12, 12, true},
        {"Deidara", "Ex Admin", 457, 35, 0, 0, true},
        {"MegaSa1nt", "Server Core", 478, 14, 29, 11, true},
        {"FarJohan", "SGDPS T2 Editor", 102, 9, 11, 3, true},
        {"GeodeTeam", "Mod Loader", 118, 12, 3, 3, true}
    };

public:
    static SalamandraCreditsPopup* create() {
        auto popup = new SalamandraCreditsPopup();
        if (popup && popup->init()) {
            popup->autorelease();
            return popup;
        }
        delete popup;
        return nullptr;
    }

private:
    bool init() {
        if (!Popup::init(440.f, 250.f, "GJ_square01.png")) return false;

        if (auto title = geode::Label::create("Salamandra Credits", "bigFont.fnt")) {
            title->setPosition({220.f, 220.f});
            title->setScale(0.8f);
            m_mainLayer->addChild(title);
        }

        m_creditsContainer = CCNode::create();
        m_navMenu = CCMenu::create();
        if (!m_creditsContainer || !m_navMenu) return false;

        m_mainLayer->addChild(m_creditsContainer);
        m_navMenu->setPosition({0.f, 0.f});
        m_mainLayer->addChild(m_navMenu, 10);
        updatePage();
        return true;
    }

    void updatePage() {
        if (!m_creditsContainer || !m_navMenu) return;
        m_creditsContainer->removeAllChildren();
        m_navMenu->removeAllChildren();

        auto start = static_cast<size_t>(m_currentPage) * 6;
        auto end = std::min(start + 6, m_allCredits.size());
        for (size_t i = start; i < end; ++i) {
            if (auto node = CreditNode::create(m_allCredits[i])) {
                auto index = static_cast<int>(i - start);
                node->setPosition({75.f + (index % 3) * 145.f,
                                   150.f - (index / 3) * 90.f});
                m_creditsContainer->addChild(node);
            }
        }

        if (m_currentPage > 0) {
            if (auto arrow = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png")) {
                if (auto button = CCMenuItemSpriteExtra::create(
                    arrow, this, menu_selector(SalamandraCreditsPopup::onPrev))) {
                    button->setPosition({-25.f, 125.f});
                    m_navMenu->addChild(button);
                }
            }
        }
        if (end < m_allCredits.size()) {
            if (auto arrow = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png")) {
                arrow->setFlipX(true);
                if (auto button = CCMenuItemSpriteExtra::create(
                    arrow, this, menu_selector(SalamandraCreditsPopup::onNext))) {
                    button->setPosition({465.f, 125.f});
                    m_navMenu->addChild(button);
                }
            }
        }
    }

    void onPrev(CCObject*) {
        if (m_currentPage > 0) {
            --m_currentPage;
            updatePage();
        }
    }
    void onNext(CCObject*) {
        if ((static_cast<size_t>(m_currentPage) + 1) * 6 < m_allCredits.size()) {
            ++m_currentPage;
            updatePage();
        }
    }
};

} // namespace

class $modify(SalamandraCreditsMenuLayerV6, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;

        auto menu = getChildByID("more-games-menu");
        if (!menu) return true;

        auto button = typeinfo_cast<CCMenuItemSpriteExtra*>(menu->getChildByID("more-games-button"));
        if (!button) return true;

        // Reuse the existing More Games button, as in the original mod.
        if (auto sprite = CCSprite::createWithSpriteFrameName("GJ_menuBtn_001.png")) {
            button->setSprite(sprite);  // Handles CCMenuItemSpriteExtra sizing.
        }
        button->setTarget(this, menu_selector(SalamandraCreditsMenuLayerV6::onCredits));
        button->setID("credits-button"_spr);
        return true;
    }

    void onCredits(CCObject*) {
        auto scene = CCDirector::get()->getRunningScene();
        if (scene && scene->getChildByID("credits-popup-layer"_spr)) return;

        auto popup = SalamandraCreditsPopup::create();
        if (!popup) {
            log::warn("Salamandra: unable to create credits popup");
            return;
        }
        popup->setID("credits-popup-layer"_spr);
        popup->show();
    }
};
