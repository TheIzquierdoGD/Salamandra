#include "SalamandraBadges.hpp"

#include <Geode/Geode.hpp>
#include <Geode/binding/GJComment.hpp>
#include <Geode/modify/CommentCell.hpp>

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

class $modify(SBCommentCell, CommentCell) {
    struct Fields {
        std::string m_badgeName;
        std::vector<int> m_displayedTypes;
    };

    static void onModify(auto& self) {

        (void)self.setHookPriorityBeforePost(
            "CommentCell::loadFromComment", "thesillydoggo.comment_emojis"
        );
        (void)self.setHookPriorityBeforePost(
            "CommentCell::loadFromComment", "prevter.comment_emojis"
        );
    }

    void loadFromComment(GJComment* comment) {
        CommentCell::loadFromComment(comment);
        if (!m_mainLayer) return;

        auto usernameMenu = m_mainLayer->getChildByIDRecursive("username-menu");
        if (!usernameMenu) return;

        for (int type : m_fields->m_displayedTypes) {
            if (auto previous = usernameMenu->getChildByID(badgeNodeID(type))) {
                previous->removeFromParent();
            }
        }
        m_fields->m_displayedTypes.clear();
        m_fields->m_badgeName.clear();

        if (!comment) {
            usernameMenu->updateLayout();
            return;
        }
        auto badge = SalamandraBadges::badgeForUser(comment->m_accountID);
        if (!badge || badge->types.empty()) {
            usernameMenu->updateLayout();
            return;
        }

        m_fields->m_badgeName = badge->name;
        auto modBadge = usernameMenu->getChildByID("mod-badge");

        for (int type : badge->types) {
            auto sprite = createBadgeSprite(type);
            if (!sprite) continue;

            sprite->setScale(0.7f);
            auto button = CCMenuItemSpriteExtra::create(
                sprite, this, menu_selector(SBCommentCell::onBadge)
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

        if (modBadge && !m_fields->m_displayedTypes.empty()) {
            modBadge->retain();
            modBadge->removeFromParent();
            usernameMenu->addChild(modBadge);
            modBadge->release();
        }
        usernameMenu->updateLayout();
    }

    void onBadge(CCObject* sender) {
        if (!sender) return;
        SalamandraBadges::showBadgeInfo(m_fields->m_badgeName, sender->getTag());
    }
};
