#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>

using namespace geode::prelude;

class CreditNode : public CCNode {
public:
    static CreditNode* create(const char* name, const char* reason, int iconID, int c1, int c2, int c3, bool glow) {
        auto ret = new CreditNode();
        if (ret && ret->init(name, reason, iconID, c1, c2, c3, glow)) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }

    bool init(const char* name, const char* reason, int iconID, int c1, int c2, int c3, bool glow) {
        if (!CCNode::init()) return false;
        auto GM = GameManager::get();

        auto icon = SimplePlayer::create(iconID);
        icon->setColor(GM->colorForIdx(c1));
        icon->setSecondColor(GM->colorForIdx(c2));
        if (glow) {
            auto c = GM->colorForIdx(c3);
            icon->setGlowOutline(c);
            icon->enableCustomGlowColor(c);
        }
        icon->setScale(1.2f);
        addChild(icon);

        auto labelName = CCLabelBMFont::create(name, "goldFont.fnt");
        labelName->setScale(0.6f);
        labelName->setPosition({0.f, 31.f});
        addChild(labelName);

        auto labelReason = CCLabelBMFont::create(reason, "chatFont.fnt");
        labelReason->setScale(0.7f);
        labelReason->setPosition({0.f, -30.f});
        addChild(labelReason);

        return true;
    }
};

struct CreditInfo {
    const char* name;
    const char* reason;
    int iconID, c1, c2, c3;
    bool glow;
};


class CreditsLayer : public CCLayerColor {
protected:
    CCScale9Sprite* m_bg = nullptr;
    CCNode* m_creditsContainer = nullptr;
    CCMenu* m_navMenu = nullptr;
    int m_currentPage = 0;
    std::vector<CreditInfo> m_allCredits;

public:
    static CreditsLayer* create(const CCSize& winSize) {
        auto ret = new CreditsLayer();
        if (ret && ret->initWithColor({0, 0, 0, 90}, winSize.width, winSize.height)) {
            ret->autorelease();
            ret->setup(winSize);
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }

    void setup(const CCSize& winSize) {
        CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, -500, true);
        setKeypadEnabled(true);

        // Main credits lits
        m_allCredits = {
            {"Izquierdo","Developer",99,12,3,3,true},
            {"FHGDPS","Server Hosting",268,8,12,11,true},
            {"NightFall","NGDPS Rights",85,12,16,12,true},
            {"YuuMai","Ex Admin",153,35,6,6,true},
            {"RobTop","Game Developer",485,11,29,3,true},
            {"Shion","Server Booster",98,70,12,12,true},
            // Page 2
            {"Elefoco","Temp Admin",98,5,10,10,true},
            {"Cvolton","Server Files",1,30,12,12,true},
            {"Deidara","Ex Admin",457,35,0,0,true},
            {"MegaSa1nt","Server Core",478,14,29,11,true},
            {"FarJohan","SGDPS T2 Editor",102,9,11,3,true},
            {"GeodeTeam","Mod Loader",118,12,3,3,true}
        };

        m_bg = CCScale9Sprite::create("GJ_square01.png");
        m_bg->setContentSize({440.f, 250.f});
        m_bg->setPosition(winSize / 2);
        addChild(m_bg, 1);

        auto title = CCLabelBMFont::create("Salamandra Credits", "bigFont.fnt");
        title->setPosition({220.f, 220.f});
        title->setScale(1.0f);
        m_bg->addChild(title);

        m_creditsContainer = CCNode::create();
        m_bg->addChild(m_creditsContainer);

        m_navMenu = CCMenu::create();
        m_navMenu->setPosition({0, 0});
        m_bg->addChild(m_navMenu, 10);

        updatePage();

        // Close Button
        auto closeSprite = CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png");
        auto closeBtn = CCMenuItemSpriteExtra::create(closeSprite, this, menu_selector(CreditsLayer::onClose));
        auto closeMenu = CCMenu::create(closeBtn, nullptr);
        closeMenu->setPosition({5.f, 240.f});
        m_bg->addChild(closeMenu, 10);

        // Animation
        m_bg->setScale(0.f);
        m_bg->runAction(CCEaseBackOut::create(CCScaleTo::create(0.3f, 1.0f)));
    }

    void updatePage() {
        m_creditsContainer->removeAllChildren();
        m_navMenu->removeAllChildren();

        int startIdx = m_currentPage * 6;
        int endIdx = std::min(startIdx + 6, (int)m_allCredits.size());

        float startX = 75.f;
        float startY = 150.f;

        for (int i = startIdx; i < endIdx; ++i) {
            auto& c = m_allCredits[i];
            auto node = CreditNode::create(c.name, c.reason, c.iconID, c.c1, c.c2, c.c3, c.glow);
            int localIdx = i - startIdx;
            node->setPosition({startX + (localIdx % 3) * 145.f, startY - (localIdx / 3) * 90.f});
            m_creditsContainer->addChild(node);
        }

        // Back Button
        if (m_currentPage > 0) {
            auto prevBtn = CCMenuItemSpriteExtra::create(
                CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png"),
                this, menu_selector(CreditsLayer::onPrev)
            );
            prevBtn->setPosition({-25.f, 125.f});
            m_navMenu->addChild(prevBtn);
        }

        // Next Button
        if (endIdx < m_allCredits.size()) {
            auto nextSprite = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
            nextSprite->setFlipX(true);

            auto nextBtn = CCMenuItemSpriteExtra::create(
                nextSprite,
                this, menu_selector(CreditsLayer::onNext)
            );
            nextBtn->setPosition({465.f, 125.f});
            m_navMenu->addChild(nextBtn);
        }
    }

    void onNext(CCObject*) {
        m_currentPage++;
        updatePage();
    }

    void onPrev(CCObject*) {
        m_currentPage--;
        updatePage();
    }

    bool ccTouchBegan(CCTouch*, CCEvent*) override { return true; }
    void keyBackClicked() override { onClose(nullptr); }
    void onClose(CCObject*) { removeFromParentAndCleanup(true); }

    void onExit() override {
        CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
        CCLayerColor::onExit();
    }
};

class $modify(ThirdExampleButtonMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;

        auto moreGamesMenu = getChildByID("more-games-menu");
        if (!moreGamesMenu) return true;

        auto moreGamesBtn = typeinfo_cast<CCMenuItemSpriteExtra*>(moreGamesMenu->getChildByID("more-games-button"));
        if (!moreGamesBtn) return true;

        auto sprite = CCSprite::createWithSpriteFrameName("GJ_menuBtn_001.png");
        moreGamesBtn->setNormalImage(sprite);
        moreGamesBtn->setSelectedImage(CCSprite::createWithSpriteFrameName("GJ_menuBtn_001.png"));

        moreGamesBtn->setTarget(this, menu_selector(ThirdExampleButtonMenuLayer::onCredits));
        moreGamesBtn->setID("credits-button"_spr);

        return true;
    }

    void onCredits(CCObject*) {
        if (getChildByID("credits-popup-layer"_spr)) return;

        auto winSize = CCDirector::sharedDirector()->getWinSize();
        auto overlay = CreditsLayer::create(winSize);
        overlay->setID("credits-popup-layer"_spr);
        addChild(overlay, 999);
    }
};