// Salamandra request form and main-level gate - Geode v6
#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/utils/web.hpp>

using namespace geode::prelude;

class $modify(SalamandraRequestMenuV6, MenuLayer) {
    struct Fields {
        bool m_blockPlay = true; // Original behavior: main levels are blocked.
    };

    bool init() {
        if (!MenuLayer::init()) return false;

        if (auto menu = getChildByID("bottom-menu")) {
            if (auto old = menu->getChildByID("newgrounds-button")) {
                old->removeFromParent();
            }

            if (!menu->getChildByID("request-form-button"_spr)) {
                auto background = CCSprite::createWithSpriteFrameName(
                    "geode.loader/baseCircle_MediumAlt_Green.png");
                if (!background) {
                    // Fallback in case the Geode loader texture changes in an alpha build.
                    background = CCSprite::createWithSpriteFrameName("GJ_plusBtn_001.png");
                }
                if (background) {
                    if (auto icon = CCSprite::createWithSpriteFrameName("RequestButton.png"_spr)) {
                        icon->setScale(1.1f);
                        icon->setPosition(background->getContentSize() / 2 + ccp(0.f, 1.f));
                        background->addChild(icon, 1);
                    } else {
                        log::warn("Salamandra: RequestButton.png resource not found");
                    }

                    if (auto button = CCMenuItemSpriteExtra::create(
                        background, this, menu_selector(SalamandraRequestMenuV6::onRequestForm))) {
                        button->setID("request-form-button"_spr);
                        menu->addChild(button);
                    }
                }
            }
            menu->updateLayout();
        }

        if (m_fields->m_blockPlay) {
            if (auto mainMenu = getChildByID("main-menu")) {
                if (auto play = typeinfo_cast<CCMenuItemSpriteExtra*>(
                    mainMenu->getChildByID("play-button"))) {
                    play->setColor({165, 165, 165});
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
            "Cancel", "Open",
            [this](auto, bool accepted) {
                if (accepted) this->onChooseForm(nullptr);
            }
        );
    }

    void onChooseForm(CCObject*) {
        geode::createQuickPopup(
            "Choose Formulary",
            "What do you need to apply for? You may be rejected if you do not comply with the rules.",
            "Top 100", "Mod Role",
            [](auto, bool accepted) {
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
            Notification::create("Main levels will be ready soon", NotificationIcon::Info, 2.5f)->show();
        } else {
            MenuLayer::onPlay(sender);
        }
    }
};
