#include "SalamandraBadges.hpp"
#include <Geode/binding/GJComment.hpp>
#include <Geode/modify/CommentCell.hpp>

//Disclaimer: All the code was created by Hiimjasmine00 and adapted by TheIzquierdoGD from the "Developer Badges" mod
using namespace geode::prelude;

class $modify(SBCommentCell, CommentCell) {
    struct Fields {
        std::string m_badgeName;
    };

    static void onModify(ModifyBase<ModifyDerive<SBCommentCell, CommentCell>>& self) {
        (void)self.setHookPriorityBeforePost("CommentCell::loadFromComment", "thesillydoggo.comment_emojis");
        (void)self.setHookPriorityBeforePost("CommentCell::loadFromComment", "prevter.comment_emojis");
    }

    void loadFromComment(GJComment* comment) {
        CommentCell::loadFromComment(comment);

        auto badge = SalamandraBadges::badgeForUser(comment->m_accountID);
        if (!badge || badge->types.empty()) return;

        auto usernameMenu = m_mainLayer->getChildByIDRecursive("username-menu");
        if (!usernameMenu) return;

        m_fields->m_badgeName = badge->name;

        CCNode* modBadge = usernameMenu->getChildByID("mod-badge");

        for (int bType : badge->types) {
            auto frameName = fmt::format("GDSBadge{:02}.png"_spr, bType);
            auto badgeSprite = CCSprite::createWithSpriteFrameName(frameName.c_str());
            
            if (badgeSprite) {
                badgeSprite->setScale(0.7f);
                auto badgeButton = CCMenuItemSpriteExtra::create(badgeSprite, this, menu_selector(SBCommentCell::onBadge));
                badgeButton->setID(fmt::format("sgdps-badge-{}"_spr, bType));
                badgeButton->setTag(bType);
                usernameMenu->addChild(badgeButton);
            }
        }

        if (modBadge) {
            modBadge->retain();
            modBadge->removeFromParent();
            usernameMenu->addChild(modBadge);
            modBadge->release();
        }

        usernameMenu->updateLayout();
    }

    void onBadge(CCObject* sender) {
        SalamandraBadges::showBadgeInfo(m_fields->m_badgeName, sender->getTag());
    }
};