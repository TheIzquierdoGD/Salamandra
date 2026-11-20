#include "SalamandraBadges.hpp"

#include <Geode/Geode.hpp>
#include <Geode/modify/ProfilePage.hpp>
#include <Geode/binding/GJUserScore.hpp>

// Original badge code by Hiimjasmine00, adapted by TheIzquierdoGD.
using namespace geode::prelude;

namespace {
    std::string badgeNodeID(int type) {
        return fmt::format("{}/sgdps-badge-{}", Mod::get()->getID(), type);
    }

    CCSprite* createBadgeSprite(int type) {
        auto localName = fmt::format("GDSBadge{:02}.png", type);
        auto frameName = Mod::get()->expandSpriteName(localName);
        return CCSprite::createWithSpriteFrameName(frameName.c_str());
    }
}

class $modify(SBProfilePage, ProfilePage) {
    struct Fields {
        std::string m_badgeName;
        std::vector<int> m_displayedTypes;
    };

    void loadPageFromUserInfo(GJUserScore* score) {
        ProfilePage::loadPageFromUserInfo(score);
        if (!m_mainLayer) return;

        auto usernameMenu = m_mainLayer->getChildByIDRecursive("username-menu");
        if (!usernameMenu) return;

        // Refresh instead of creating duplicate buttons on profile reload.
        for (int type : m_fields->m_displayedTypes) {
            if (auto previous = usernameMenu->getChildByID(badgeNodeID(type))) {
                previous->removeFromParent();
            }
        }
        m_fields->m_displayedTypes.clear();
        m_fields->m_badgeName.clear();

        if (!score) {
            usernameMenu->updateLayout();
            return;
        }
        auto badge = SalamandraBadges::badgeForUser(score->m_accountID);
        if (!badge || badge->types.empty()) {
            usernameMenu->updateLayout();
            return;
        }

        m_fields->m_badgeName = badge->name;
        for (int type : badge->types) {
            auto sprite = createBadgeSprite(type);
            if (!sprite) continue;

            auto button = CCMenuItemSpriteExtra::create(
                sprite, this, menu_selector(SBProfilePage::onBadge)
            );
            if (!button) continue;

            auto id = badgeNodeID(type);
            if (auto existing = usernameMenu->getChildByID(id)) {
                existing->removeFromParent();
            }
            button->setID(id);
            button->setTag(type);
            usernameMenu->addChild(button);
            m_fields->m_displayedTypes.push_back(type);
        }
        usernameMenu->updateLayout();
    }

    void onBadge(CCObject* sender) {
        if (!sender) return;
        SalamandraBadges::showBadgeInfo(m_fields->m_badgeName, sender->getTag());
    }
};
