#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/utils/web.hpp>

using namespace geode::prelude;

class $modify(SalamandraLogoLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init())
            return false;

        auto socialMenu = this->getChildByID("social-media-menu");
        if (!socialMenu)
            return true;

        if (auto robtopBtn = socialMenu->getChildByID("robtop-logo-button")) {
            robtopBtn->removeFromParent();
        }

        auto customSprite = CCSprite::createWithSpriteFrameName("theizquierdogd.salamandra/xTIGDx_Logo_Menu.png");
        
        if (!customSprite) {
            customSprite = CCSprite::create("theizquierdogd.salamandra/xTIGDx_Logo_Menu.png");
        }

        if (customSprite) {
            customSprite->setScale(0.85f);

            auto salamandraButton = CCMenuItemSpriteExtra::create(
                customSprite, 
                this, 
                menu_selector(SalamandraLogoLayer::onSalamandraClick)
            );

            salamandraButton->setID("salamandra-logo-button"_spr);
            socialMenu->addChild(salamandraButton);

            if (auto discordBtn = socialMenu->getChildByID("discord-button")) {
                if (auto fbBtn = socialMenu->getChildByID("facebook-button")) fbBtn->setZOrder(0);
                discordBtn->setZOrder(1);
                if (auto ytBtn = socialMenu->getChildByID("youtube-button")) ytBtn->setZOrder(2);
                salamandraButton->setZOrder(3);
            }

            socialMenu->updateLayout();
            salamandraButton->setPosition({ 41.0f, 9.0f });
        }

        return true;
    }

    void onSalamandraClick(CCObject*) {
        geode::utils::web::openLinkInBrowser("https://salamandra.ps.fhgdps.com/main");
    }
};