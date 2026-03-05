#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/CreatorLayer.hpp>
#include <Geode/utils/web.hpp>

using namespace geode::prelude;

// =====================
// MenuLayer
// =====================
class $modify(MyMenuLayer, MenuLayer) {
    static void onModify(auto& self) {
        (void)self.setHookPriority("MenuLayer::init", -10000);
    }

    bool init() {
        if (!MenuLayer::init())
            return false;

        auto bottomMenu = this->getChildByID("bottom-menu");

        if (bottomMenu) {
            auto layout = RowLayout::create();
            layout->setGap(7.f);
            layout->setAutoScale(false);
            layout->setAxisAlignment(AxisAlignment::Center);

            bottomMenu->setLayout(layout);

//            if (auto geodeBtn =
  //              bottomMenu->getChildByID("geode.loader/geode-button")) {
    //            geodeBtn->setVisible(false);
      //          geodeBtn->setContentSize({ 0.f, 0.f });
        //    }

            bottomMenu->updateLayout();
        }

        if (auto socialMediaMenu =
            this->getChildByID("social-media-menu")) {
            socialMediaMenu->setVisible(false);
        }

        auto profileMenu   = this->getChildByID("profile-menu");
        auto usernameLabel = this->getChildByID("player-username");

        if (profileMenu && bottomMenu && usernameLabel) {
            float newY = bottomMenu->getPositionY();
            float offset = newY - profileMenu->getPositionY();

            profileMenu->setPositionY(newY);
            usernameLabel->setPositionY(
                usernameLabel->getPositionY() + offset
            );
        }

        if (bottomMenu) {
            auto achBtn = typeinfo_cast<CCMenuItemSpriteExtra*>(
                bottomMenu->getChildByID("achievements-button")
            );

            if (achBtn) {
                if (auto sprite =
                    CCSprite::createWithSpriteFrameName(
                        "GJ_gpgBtn_001.png")) {

                    achBtn->setNormalImage(sprite);
                    achBtn->setSelectedImage(
                        CCSprite::createWithSpriteFrameName(
                            "GJ_gpgBtn_001.png"
                        )
                    );
                }

                achBtn->setTarget(
                    this,
                    menu_selector(MyMenuLayer::onSupportDialog)
                );
            }
        }

        return true;
    }

    void onSupportDialog(CCObject*) {
        geode::createQuickPopup(
            "External Comunity",
            "Do you want to join the official Discord for talk?",
            "Cancel",
            "Open",
            [](auto, bool accepted) {
                if (accepted) {
                    geode::utils::web::openLinkInBrowser(
                        "https://discord.gg/drs93ZnFNb"
                    );
                }
            }
        );
    }
};

// =====================
// CreatorLayer
// =====================
class $modify(MyCreatorLayer, CreatorLayer) {
    bool init() {
        if (!CreatorLayer::init())
            return false;

        if (auto menu =
            this->getChildByID("creator-buttons-menu")) {
            menu->updateLayout();
        }

        return true;
    }
};