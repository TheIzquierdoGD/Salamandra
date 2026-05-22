#include <Geode/Geode.hpp>
#include <Geode/modify/LevelBrowserLayer.hpp>

using namespace geode::prelude;

class $modify(MyBrowserUI, LevelBrowserLayer) {
    struct Fields {
        bool m_isSalamandraMode = false;
        int m_currentCase = -1;
    };

    bool init(GJSearchObject* search) {
        if (!LevelBrowserLayer::init(search)) return false;

        this->syncSalamandraState(search);
        this->schedule(schedule_selector(MyBrowserUI::salamandraHeartbeat), 0.0f);
        
        return true;
    }

    void loadPage(GJSearchObject* search) {
        LevelBrowserLayer::loadPage(search);
        this->syncSalamandraState(search);
    }

    void update(float dt) {
        LevelBrowserLayer::update(dt);
        this->suppressNativeTitle();
    }

    void suppressNativeTitle() {
        if (!m_fields->m_isSalamandraMode) return;

        if (auto listLayer = this->getChildByType<GJListLayer>(0)) {
            if (auto title = listLayer->getChildByID("title")) {
                title->setVisible(false);
                title->setScale(0.0f);
            }
            if (auto infoBtn = listLayer->getChildByID("info-button")) {
                infoBtn->setVisible(false);
                infoBtn->setScale(0.0f);
            }
        }
    }

    void salamandraHeartbeat(float dt) {
        this->suppressNativeTitle();

        if (auto backMenu = this->getChildByID("back-menu")) {
            if (!backMenu->getChildByID("salamandra-anchor"_spr)) {
                this->setupPersistentUI(backMenu);
            }
        }
    }

    void syncSalamandraState(GJSearchObject* search) {
        if (!search) return;
        int type = static_cast<int>(search->m_searchType);
        m_fields->m_currentCase = type;
        m_fields->m_isSalamandraMode = (type == 11 || type == 12);

        if (auto backMenu = this->getChildByID("back-menu")) {
            this->setupPersistentUI(backMenu);
        }
    }

    void setupPersistentUI(CCNode* backMenu) {
        if (auto anchor = backMenu->getChildByID("salamandra-anchor"_spr)) {
            this->updateVisuals(anchor);
            return;
        }

        auto anchor = CCNode::create();
        anchor->setID("salamandra-anchor"_spr);
        backMenu->addChild(anchor);

        auto winSize = CCDirector::get()->getWinSize();
        
        CCPoint centerTop = backMenu->convertToNodeSpace({winSize.width / 2, winSize.height - 26.0f});

        auto titleSprite = CCSprite::create();
        titleSprite->setID("salamandra-title-sprite"_spr);
        titleSprite->setPosition(centerTop);
        anchor->addChild(titleSprite);

        this->updateVisuals(anchor);
    }

    void updateVisuals(CCNode* anchor) {
        auto sprite = static_cast<CCSprite*>(anchor->getChildByID("salamandra-title-sprite"_spr));
        if (!sprite) return;

        std::string frameName = "";
        
        if (m_fields->m_currentCase == 11) {
            frameName = "theizquierdogd.salamandra/DemonListTitle.png";
        } else if (m_fields->m_currentCase == 12) {
            frameName = "theizquierdogd.salamandra/PemonListTitle.png";
        }

        if (!frameName.empty()) {
            auto frame = CCSpriteFrameCache::get()->spriteFrameByName(frameName.c_str());
            if (frame) {
                sprite->setDisplayFrame(frame);
                sprite->setScale(1.0f); 
            }
        }

        anchor->setVisible(m_fields->m_isSalamandraMode);
    }
};