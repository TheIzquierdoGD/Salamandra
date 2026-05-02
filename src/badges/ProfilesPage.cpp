#include "SalamandraBadges.hpp"
#include <Geode/modify/ProfilePage.hpp>
#include <Geode/binding/GJUserScore.hpp>

//Disclaimer: All the code was created by Hiimjasmine00 and adapted by TheIzquierdoGD from the "Developer Badges" mod
using namespace geode::prelude;

class $modify(SBProfilePage, ProfilePage) {
    struct Fields {
        std::string m_badgeName;
        bool m_hasBadge = false;
    };

    void loadPageFromUserInfo(GJUserScore* score) {
        ProfilePage::loadPageFromUserInfo(score);

        if (m_fields->m_hasBadge) return;

        auto badge = SalamandraBadges::badgeForUser(score->m_accountID);
        if (!badge || badge->types.empty()) return;

        auto usernameMenu = m_mainLayer->getChildByID("username-menu");
        if (!usernameMenu) return;

        m_fields->m_badgeName = badge->name;

        for (int bType : badge->types) {
            auto frameName = fmt::format("GDSBadge{:02}.png"_spr, bType);
            auto sprite = CCSprite::createWithSpriteFrameName(frameName.c_str());
            
            if (sprite) {
                auto btn = CCMenuItemSpriteExtra::create(sprite, this, menu_selector(SBProfilePage::onBadge));
                btn->setTag(bType);
                btn->setID(fmt::format("sgdps-badge-{}"_spr, bType));
                usernameMenu->addChild(btn);
            }
        }

        usernameMenu->updateLayout();
        m_fields->m_hasBadge = true;
    }

    void onBadge(CCObject* sender) {
        SalamandraBadges::showBadgeInfo(m_fields->m_badgeName, sender->getTag());
    }
};