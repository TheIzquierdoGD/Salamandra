#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>

using namespace geode::prelude;

// ----------------------
// CreditNode
// ----------------------
class CreditNode : public CCNode {
protected:
    int m_accountID = 0;
    SimplePlayer* m_userIcon = nullptr;
    CCLabelBMFont* m_userName = nullptr;
    CCLabelBMFont* m_userReason = nullptr;
    CCMenuItemSpriteExtra* m_userButton = nullptr;

public:
    static CreditNode* create(
        const char* name,
        const char* reason,
        int iconID,
        int color1,
        int color2,
        int color3,
        bool glow = false,
        int accountID = 0
    ) {
        auto ret = new CreditNode();
        if (ret && ret->init(name, reason, iconID, color1, color2, color3, glow, accountID)) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }

    bool init(
        const char* name,
        const char* reason,
        int iconID,
        int color1,
        int color2,
        int color3,
        bool glow,
        int accountID
    ) {
        if (!CCNode::init()) return false;

        auto GM = GameManager::get();

        m_userName = CCLabelBMFont::create(name, "goldFont.fnt");
        m_userName->setScale(0.6f);

        m_userButton = CCMenuItemSpriteExtra::create(
            m_userName, this, menu_selector(CreditNode::onUser)
        );

        m_userIcon = SimplePlayer::create(iconID);
        m_userIcon->setColor(GM->colorForIdx(color1));
        m_userIcon->setSecondColor(GM->colorForIdx(color2));

        if (glow) {
            auto c = GM->colorForIdx(color3);
            m_userIcon->setGlowOutline(c);
            m_userIcon->enableCustomGlowColor(c);
        }

        m_userIcon->setScale(1.2f);

        m_userReason = CCLabelBMFont::create(reason, "chatFont.fnt");
        m_userReason->setScale(0.7f);
        m_userReason->setPosition({0.f, -30.f});

        auto menu = CCMenu::create(m_userButton, nullptr);
        menu->setPosition({0.f, 31.f});

        addChild(menu);
        addChild(m_userIcon);
        addChild(m_userReason);

        m_accountID = accountID;
        return true;
    }

    void onUser(CCObject*) {
        ProfilePage::create(m_accountID, false)->show();
    }
};

// ----------------------
// CreditsLayer
// ----------------------
class CreditsLayer : public CCLayerColor {
protected:
    CCScale9Sprite* m_bg = nullptr;

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
        CCDirector::sharedDirector()->getTouchDispatcher()
            ->addTargetedDelegate(this, -500, true);

        setKeypadEnabled(true);

        // ----------------------
        // Fondo
        // ----------------------
        m_bg = CCScale9Sprite::create("GJ_square01.png");
        m_bg->setContentSize({440.f, 250.f});
        m_bg->setPosition(winSize / 2);

        // ====== FIX REAL ======
        const float margin = 40.f; // margen seguro
        float scale = std::min(
            (winSize.width  - margin) / 440.f,
            (winSize.height - margin) / 250.f
        );
        scale = std::min(scale, 1.0f); // nunca agrandar

        m_bg->setScale(0.f);
        addChild(m_bg, 1);

        // ----------------------
        // Título
        // ----------------------
        auto title = CCLabelBMFont::create("Salamandra Credits", "bigFont.fnt");
        title->setPosition({220.f, 220.f});
        title->setScale(0.9f);
        m_bg->addChild(title);

        // ----------------------
        // Créditos
        // ----------------------
        struct CreditInfo {
            const char* name;
            const char* reason;
            int iconID, c1, c2, c3;
            bool glow;
        };

        CreditInfo credits[6] = {
            {"TheIzquierdoGD","SGDPS & Mod Creator",99,12,3,3,true},
            {"GhostPower","Original Dark Textures",121,15,1,1,true},
            {"FemboyRyna","NightFall GDPS Rights",85,12,16,12,true},
            {"YuuMai & More","Ex Community Managers",153,35,6,6,true},
            {"RobTop & Cvolton","Game Developer & Server Files",485,11,29,3,true},
            {"FH & MegaSaint","Server Hosting & Core",268,8,12,11,true}
        };

        float startX = 75.f;
        float startY = 150.f;

        for (int i = 0; i < 6; ++i) {
            auto& c = credits[i];
            auto node = CreditNode::create(
                c.name, c.reason, c.iconID, c.c1, c.c2, c.c3, c.glow, 0
            );
            node->setPosition({
                startX + (i % 3) * 145.f,
                startY - (i / 3) * 90.f
            });
            m_bg->addChild(node);
        }

        // ----------------------
        // Botón cerrar
        // ----------------------
        auto closeSprite = CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png");
        auto closeBtn = CCMenuItemSpriteExtra::create(
            closeSprite, this, menu_selector(CreditsLayer::onClose)
        );

        auto closeMenu = CCMenu::create(closeBtn, nullptr);
        closeMenu->setPosition({5.f, 240.f});
        closeMenu->setTouchPriority(-600);
        m_bg->addChild(closeMenu, 10);

        // ----------------------
        // Animación (respeta el scale base)
        // ----------------------
        m_bg->runAction(CCSequence::create(
            CCScaleTo::create(0.15f, scale * 1.1f),
            CCScaleTo::create(0.15f, scale * 0.95f),
            CCScaleTo::create(0.1f,  scale),
            nullptr
        ));
    }

    bool ccTouchBegan(CCTouch*, CCEvent*) override { return true; }
    void keyBackClicked() override { removeFromParentAndCleanup(true); }
    void onClose(CCObject*) { removeFromParentAndCleanup(true); }

    void onExit() override {
        CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
        CCLayerColor::onExit();
    }
};

// ----------------------
// MenuLayer
// ----------------------
class $modify(ThirdExampleButtonMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init())
            return false;

        // Obtener menú de More Games
        auto moreGamesMenu = getChildByID("more-games-menu");
        if (!moreGamesMenu)
            return true;

        // Obtener botón More Games
        auto moreGamesBtn = typeinfo_cast<CCMenuItemSpriteExtra*>(
            moreGamesMenu->getChildByID("more-games-button")
        );

        if (!moreGamesBtn)
            return true;

        // Cambiar sprite (opcional)
        auto sprite = CCSprite::createWithSpriteFrameName("GJ_menuBtn_001.png");
        moreGamesBtn->setNormalImage(sprite);
        moreGamesBtn->setSelectedImage(
            CCSprite::createWithSpriteFrameName("GJ_menuBtn_001.png")
        );

        // Cambiar callback
        moreGamesBtn->setTarget(
            this,
            menu_selector(ThirdExampleButtonMenuLayer::onCredits)
        );

        moreGamesBtn->setID("credits-button"_spr);

        return true;
    }

    void onCredits(CCObject*) {
        if (getChildByID("credits-popup-layer"_spr))
            return;

        auto winSize = CCDirector::sharedDirector()->getWinSize();
        auto overlay = CreditsLayer::create(winSize);
        overlay->setID("credits-popup-layer"_spr);
        addChild(overlay, 999);
    }
};
