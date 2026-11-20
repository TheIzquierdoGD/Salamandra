// Salamandra logo instead of RobTop social button - Geode v6
#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/utils/web.hpp>

using namespace geode::prelude;

class $modify(SalamandraRobTopButtonV6, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;

        auto menu = getChildByID("social-media-menu");
        if (!menu || menu->getChildByID("salamandra-logo-button"_spr)) return true;

        auto icon = CCSprite::createWithSpriteFrameName("xTIGDx_Logo_Menu.png"_spr);
        if (!icon) icon = CCSprite::create("xTIGDx_Logo_Menu.png"_spr);
        if (!icon) {
            // Do not remove RobTop's button if the custom image isn't available.
            log::warn("Salamandra: xTIGDx_Logo_Menu.png was not loaded");
            return true;
        }

        icon->setScale(0.85f);
        auto button = CCMenuItemSpriteExtra::create(
            icon, this, menu_selector(SalamandraRobTopButtonV6::onSalamandraClick));
        if (!button) return true;

        if (auto robtop = menu->getChildByID("robtop-logo-button")) {
            robtop->removeFromParent();
        }
        button->setID("salamandra-logo-button"_spr);
        menu->addChild(button);

        if (auto fb = menu->getChildByID("facebook-button")) fb->setZOrder(0);
        if (auto discord = menu->getChildByID("discord-button")) discord->setZOrder(1);
        if (auto yt = menu->getChildByID("youtube-button")) yt->setZOrder(2);
        button->setZOrder(3);

        menu->updateLayout();
        button->setPosition({41.f, 9.f}); // Preserve the original custom position.
        return true;
    }

    void onSalamandraClick(CCObject*) {
        geode::utils::web::openLinkInBrowser("https://salamandra.ps.fhgdps.com/main");
    }
};
