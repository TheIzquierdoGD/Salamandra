// Salamandra social links - Geode v6
#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/CreatorLayer.hpp>
#include <Geode/utils/web.hpp>

using namespace geode::prelude;

class $modify(SalamandraSocialMenuV6, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;

        auto menu = getChildByID("social-media-menu");
        if (!menu) return true;

        if (auto discord = menu->getChildByID("discord-button")) discord->setVisible(false);
        if (auto twitch = menu->getChildByID("twitch-button")) twitch->setVisible(false);

        if (auto twitter = typeinfo_cast<CCMenuItemSpriteExtra*>(
                menu->getChildByID("twitter-button"))) {
            if (auto icon = CCSprite::createWithSpriteFrameName("gj_discordIcon_001.png")) {
                twitter->setSprite(icon);
            }
            twitter->setScale(0.8f);
            twitter->m_baseScale = 0.8f;
            twitter->setTarget(this, menu_selector(SalamandraSocialMenuV6::onDiscord));
        }

        if (auto youtube = typeinfo_cast<CCMenuItemSpriteExtra*>(
                menu->getChildByID("youtube-button"))) {
            youtube->setTarget(this, menu_selector(SalamandraSocialMenuV6::onYoutube));
        }

        if (auto facebook = typeinfo_cast<CCMenuItemSpriteExtra*>(
                menu->getChildByID("facebook-button"))) {
            facebook->setTarget(this, menu_selector(SalamandraSocialMenuV6::onFacebook));
            facebook->setPosition({12.600f, 39.500f});
        }

        return true;
    }

    // Explicit callbacks, not raw user-data pointers or internal selector fields.
    void onDiscord(CCObject*) {
        geode::utils::web::openLinkInBrowser("https://discord.gg/2Jq8QuhwqF");
    }
    void onYoutube(CCObject*) {
        geode::utils::web::openLinkInBrowser("https://www.youtube.com/@TheIzquierdoGD");
    }
    void onFacebook(CCObject*) {
        geode::utils::web::openLinkInBrowser("https://www.facebook.com/The1zquierdoGD");
    }
};

class $modify(SalamandraCreatorMenuLayoutV6, CreatorLayer) {
    bool init() {
        if (!CreatorLayer::init()) return false;
        if (auto menu = getChildByID("creator-buttons-menu")) menu->updateLayout();
        return true;
    }
};
