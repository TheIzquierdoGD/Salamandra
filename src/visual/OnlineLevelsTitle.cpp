// Salamandra titles for Demon List and Pemon List - Geode v6
#include <Geode/Geode.hpp>
#include <Geode/modify/LevelBrowserLayer.hpp>
#include <Geode/binding/GJListLayer.hpp>
#include <Geode/binding/GJSearchObject.hpp>
#include <string>

using namespace geode::prelude;

class $modify(SalamandraOnlineTitles, LevelBrowserLayer) {
    struct Fields {
        bool m_specialList = false;
        int m_searchType = -1;
    };

    $override bool init(GJSearchObject* search) {
        if (!LevelBrowserLayer::init(search)) return false;
        syncState(search);
        schedule(schedule_selector(SalamandraOnlineTitles::heartbeat), 0.1f);
        return true;
    }

    $override void loadPage(GJSearchObject* search) {
        LevelBrowserLayer::loadPage(search);
        syncState(search);
    }

    void syncState(GJSearchObject* search) {
        if (!search) return;
        auto type = static_cast<int>(search->m_searchType);
        m_fields->m_searchType = type;
        m_fields->m_specialList = (type == 11 || type == 12);
        updatePresentation();
    }

    void heartbeat(float) {
        updatePresentation();
    }

    void updatePresentation() {
        auto backMenu = getChildByID("back-menu");
        if (!backMenu) return;
        // En Geode V6 m_fields es FieldIntermediate y no se puede copiar.
        // operator->() devuelve el puntero al struct Fields.
        auto* fields = m_fields.operator->();
        if (!fields) return;

        auto anchor = backMenu->getChildByID("salamandra-anchor"_spr);
        if (!anchor && fields->m_specialList) {
            anchor = CCNode::create();
            if (!anchor) return;
            anchor->setID("salamandra-anchor"_spr);
            backMenu->addChild(anchor);

            auto title = CCSprite::create();
            if (!title) return;
            title->setID("salamandra-title-sprite"_spr);
            auto size = CCDirector::get()->getWinSize();
            title->setPosition(backMenu->convertToNodeSpace({size.width / 2.f, size.height - 26.f}));
            anchor->addChild(title);
        }

        bool customVisible = false;
        if (anchor) {
            auto title = typeinfo_cast<CCSprite*>(anchor->getChildByID("salamandra-title-sprite"_spr));
            if (title && fields->m_specialList) {
                auto frameName = fields->m_searchType == 11
                    ? "theizquierdogd.salamandra/DemonListTitle.png"
                    : "theizquierdogd.salamandra/PemonListTitle.png";
                if (auto frame = CCSpriteFrameCache::get()->spriteFrameByName(frameName)) {
                    title->setDisplayFrame(frame);
                    title->setScale(1.f);
                    customVisible = true;
                }
            }
            anchor->setVisible(customVisible);
        }

        // Restore vanilla UI when leaving the custom lists or a sprite is unavailable.
        if (auto listLayer = getChildByType<GJListLayer>(0)) {
            if (auto title = listLayer->getChildByID("title"))
                title->setVisible(!customVisible);
            if (auto info = listLayer->getChildByID("info-button"))
                info->setVisible(!customVisible);
        }
    }
};
