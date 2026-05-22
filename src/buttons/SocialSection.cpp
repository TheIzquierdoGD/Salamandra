#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/CreatorLayer.hpp>
#include <Geode/utils/web.hpp>

using namespace geode::prelude;

class $modify(MyMenuLayer, MenuLayer) {
    static void onModify(auto& self) {
        (void)self.setHookPriority("MenuLayer::init", -10000);
    }

    bool init() {
        if (!MenuLayer::init())
            return false;

        if (auto socialMediaMenu = this->getChildByID("social-media-menu")) {
            
            if (auto oldDiscord = socialMediaMenu->getChildByID("discord-button")) oldDiscord->setVisible(false);
            if (auto twitchBtn = socialMediaMenu->getChildByID("twitch-button")) twitchBtn->setVisible(false);

            if (auto twitterBtn = static_cast<CCMenuItemSpriteExtra*>(socialMediaMenu->getChildByID("twitter-button"))) {
                auto discordSprite = CCSprite::createWithSpriteFrameName("gj_discordIcon_001.png");
                if (discordSprite) {
                    twitterBtn->setNormalImage(discordSprite);
                }
                
                twitterBtn->setScale(0.8f); 
                twitterBtn->m_baseScale = 0.8f;

                twitterBtn->m_pfnSelector = menu_selector(MyMenuLayer::onCustomURL);
                twitterBtn->setUserData(const_cast<char*>("https://discord.gg/2Jq8QuhwqF"));
            }

            if (auto youtubeBtn = static_cast<CCMenuItemSpriteExtra*>(socialMediaMenu->getChildByID("youtube-button"))) {
                youtubeBtn->m_pfnSelector = menu_selector(MyMenuLayer::onCustomURL);
                youtubeBtn->setUserData(const_cast<char*>("https://www.youtube.com/@TheIzquierdoGD"));
            }

            if (auto facebookBtn = static_cast<CCMenuItemSpriteExtra*>(socialMediaMenu->getChildByID("facebook-button"))) {
                facebookBtn->m_pfnSelector = menu_selector(MyMenuLayer::onCustomURL);
                facebookBtn->setUserData(const_cast<char*>("https://www.facebook.com/The1zquierdoGD"));
                
                facebookBtn->setPosition({12.600f, 39.500f});
            }
        }

        return true;
    }

    void onCustomURL(CCObject* sender) {
        auto url = static_cast<char*>(static_cast<CCNode*>(sender)->getUserData());
        if (url) {
            geode::utils::web::openLinkInBrowser(url);
        }
    }
};

class $modify(MyCreatorLayer, CreatorLayer) {
    bool init() {
        if (!CreatorLayer::init())
            return false;

        if (auto menu = this->getChildByID("creator-buttons-menu")) {
            menu->updateLayout();
        }

        return true;
    }
};