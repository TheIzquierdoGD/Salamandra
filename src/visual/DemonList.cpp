#include <Geode/Geode.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/modify/LevelCell.hpp>
#include <Geode/modify/LevelBrowserLayer.hpp>
#include <Geode/utils/web.hpp>

using namespace geode::prelude;

class DemonListManager {
protected:
    std::map<int, std::string> m_rankCache;
    bool m_isDownloading = false;

public:
    static DemonListManager* get() {
        static auto instance = new DemonListManager();
        return instance;
    }

    void clearCache() { m_rankCache.clear(); }

    void fetchList(int type) {
        if (m_isDownloading) return;
        m_isDownloading = true;

        std::string url = "https://salamandra.ps.fhgdps.com/main/demonList.php?type=" + 
                          std::to_string(type) + "&t=" + std::to_string(std::time(nullptr));
        
        spawn(web::WebRequest().get(url), [this](web::WebResponse res) {
            if (res.ok()) {
                auto data = res.string().unwrapOr("");
                auto pairs = string::split(data, ",");
                for (auto& pair : pairs) {
                    auto parts = string::split(pair, ":");
                    if (parts.size() == 2) {
                        try {
                            m_rankCache[std::stoi(string::trim(parts[0]))] = string::trim(parts[1]);
                        } catch(...) {}
                    }
                }
                if (auto browser = CCScene::get()->getChildByType<LevelBrowserLayer>(0)) {
                    browser->loadPage(browser->m_searchObject);
                }
            }
            m_isDownloading = false;
        });
    }

    std::string getRank(int levelID) {
        if (m_rankCache.count(levelID)) return m_rankCache[levelID];
        return "";
    }
};

CCNode* createRankNode(std::string rankStr, float scale) {
    auto container = CCNode::create();
    container->setID("rank-badge"_spr);

    auto label = CCLabelBMFont::create(rankStr.c_str(), "bigFont.fnt");
    label->setScale(scale);
    label->setAnchorPoint({0, 0.5f}); 

    int pos = 0;
    try { pos = std::stoi(rankStr); } catch(...) {}
    
    const char* texture = "rankIcon_all_001.png"; 
    if (pos == 1)      texture = "rankIcon_1_001.png";
    else if (pos <= 5)  texture = "rankIcon_top10_001.png";
    else if (pos <= 10) texture = "rankIcon_top50_001.png";  
    else if (pos <= 15) texture = "rankIcon_top200_001.png"; 
    else if (pos <= 20) texture = "rankIcon_top500_001.png"; 
    else if (pos <= 25) texture = "rankIcon_top2500_001.png";
    else if (pos <= 40) texture = "rankIcon_all_001.png";

    auto trophy = CCSprite::createWithSpriteFrameName(texture);
    
    float targetWidth = 18.0f; 
    float currentWidth = trophy->getContentSize().width;
    float normalizer = targetWidth / currentWidth;

    trophy->setScale(normalizer * (scale / 0.5f)); 
    trophy->setAnchorPoint({0, 0.5f});

    float labelWidth = label->getContentSize().width * label->getScale();
    label->setPosition({0, 0});
    
    // 1.5 unidades de separación
    trophy->setPosition({labelWidth + 1.5f, 0}); 

    float totalWidth = labelWidth + 1.5f + (trophy->getContentSize().width * trophy->getScale());
    container->setContentSize({totalWidth, 0}); 

    container->addChild(label);
    container->addChild(trophy);
    return container;
}

class $modify(MyLevelCell, LevelCell) {
    void loadFromLevel(GJGameLevel* level) {
        LevelCell::loadFromLevel(level);
        if (!level->m_demon) return;

        std::string rank = DemonListManager::get()->getRank(level->m_levelID);
        auto menu = this->getChildByIDRecursive("main-menu");
        if (!menu) return;

        if (auto old = menu->getChildByID("rank-badge"_spr)) old->removeFromParent();

        if (!rank.empty()) {
            auto btn = menu->getChildByID("view-button");
            if (!btn) btn = menu->getChildByID("get-button");
            
            if (btn) {
                auto badge = createRankNode(rank, 0.55f); // Reducido un poco para celdas
                float totalWidth = badge->getContentSize().width;
                badge->setPosition({ btn->getPositionX() - (totalWidth / 2), btn->getPositionY() - 30.0f });
                menu->addChild(badge);
            }
        } else {
            DemonListManager::get()->fetchList(level->isPlatformer() ? 1 : 0);
        }
    }
};

class $modify(MyLevelInfoLayer, LevelInfoLayer) {
    bool init(GJGameLevel* level, bool p1) {
        if (!LevelInfoLayer::init(level, p1)) return false;
        
        // Solo mostrar si el nivel es un Demon
        if (!level->m_demon) return true;

        std::string rank = DemonListManager::get()->getRank(level->m_levelID);
        if (!rank.empty()) {
            auto diffSprite = m_difficultySprite;
            if (diffSprite) {
                auto badge = createRankNode(rank, 0.4f); 
                
                CCNode* anchorNode = this->getChildByID("stars-label");
                if (level->m_coins > 0) {
                    if (auto c3 = this->getChildByID("coin-icon-3")) { if(c3->isVisible()) anchorNode = c3; }
                    else if (auto c2 = this->getChildByID("coin-icon-2")) { if(c2->isVisible()) anchorNode = c2; }
                    else if (auto c1 = this->getChildByID("coin-icon-1")) { if(c1->isVisible()) anchorNode = c1; }
                }

                float yPos = anchorNode ? anchorNode->getPositionY() - 15.0f : diffSprite->getPositionY() - 38.0f;
                float totalWidth = badge->getContentSize().width;
                
                badge->setPosition({ diffSprite->getPositionX() - (totalWidth / 2), yPos });
                diffSprite->getParent()->addChild(badge);
            }
        }
        return true;
    }
};

class $modify(MyLevelBrowserLayer, LevelBrowserLayer) {
    bool init(GJSearchObject* search) {
        if (!LevelBrowserLayer::init(search)) return false;
        DemonListManager::get()->clearCache();
        return true;
    }
};