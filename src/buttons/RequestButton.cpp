#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/utils/web.hpp>

using namespace geode::prelude;

class $modify(NewgroundsRequest, MenuLayer) {
    // Geode requires custom variables to be inside this struct
    struct Fields {
        bool m_blockPlay = true;
    };

    bool init() {
        if (!MenuLayer::init())
            return false;

        auto bottomMenu = this->getChildByID("bottom-menu");
        if (!bottomMenu) return true;

        // Remove Newgrounds button
        if (auto ngBtn = bottomMenu->getChildByID("newgrounds-button")) {
            ngBtn->removeFromParent();
        }

        // Custom Request Button logic
        auto customSprite = CCSprite::createWithSpriteFrameName("geode.loader/baseCircle_MediumAlt_Green.png");
        
        if (customSprite) {
            auto mainIcon = CCSprite::createWithSpriteFrameName("theizquierdogd.salamandra/RequestButton.png");
            
            if (mainIcon) {
                mainIcon->setScale(1.1f);
                mainIcon->setPosition(customSprite->getContentSize() / 2 + ccp(0, 1.0f));
                customSprite->addChild(mainIcon, 1); 
            }

            auto RequestButton = CCMenuItemSpriteExtra::create(
                customSprite, 
                this, 
                menu_selector(NewgroundsRequest::onRequestForm)
            );

            RequestButton->setID("request-form-button"_spr);
            bottomMenu->addChild(RequestButton);
            bottomMenu->updateLayout();
        }

        if (m_fields->m_blockPlay) {
            if (auto mainMenu = this->getChildByID("main-menu")) {
                if (auto playBtn = mainMenu->getChildByID("play-button")) {
                    static_cast<CCNodeRGBA*>(playBtn)->setColor({165, 165, 165});
                }
            }
        }

        return true;
    }

    void onRequestForm(CCObject*) {
        geode::createQuickPopup(
            "Request Formulary",
            "If you want to enter the <cg>Top 100 Whitelist</c>, please open the Verify Form.\n"
            "\n"
            "If you want to enter the <cy>Moderator Team</c>, please open the Moderator Form.",
            "Cancel",
            "Open",
            [this](auto, bool accepted) {
                if (accepted) {
                    this->onChooseForm(nullptr);
                }
            }
        );
    }

    void onChooseForm(CCObject*) {
        geode::createQuickPopup(
            "Choose Formulary",
            "What do you need to apply for? You may be rejected if you do not comply with the rules.",
            "Top 100",
            "Mod Role",
            [this](auto, bool accepted) {
                if (accepted) {
                    geode::utils::web::openLinkInBrowser("https://forms.gle/w1sCSKoZTVwo6aNJ8");
                } else {
                   geode::utils::web::openLinkInBrowser("https://forms.gle/43Gj7NojTHv9xz58A");
                }
            }
        );
    }

    void onPlay(CCObject* sender) {
        if (m_fields->m_blockPlay) {
            Notification::create(
                "Main levels will be ready soon", 
                NotificationIcon::Info, 
                2.5f
            )->show();
        } else {
            MenuLayer::onPlay(sender);
        }
    }
};