#include "HookedLoadingLayer.h"

using namespace geode::prelude;

bool HookedLoadingLayer::init(bool fromReload) {
    if (!LoadingLayer::init(fromReload)) {
        return false;
    }

    auto* frames = CCSpriteFrameCache::get();
    auto* gameManager = GameManager::sharedState();

    // Geode registers the mod's resources automatically.
    // Keep Salamandra_Principal.plist + its PNG in mod.json resources.
    // Use _spr because Geode namespaces resources with the mod ID.
    frames->addSpriteFramesWithFile("Salamandra_Principal.plist"_spr);

    // Ensure this setting is applied even if a graphical node is absent.
    if (gameManager && !gameManager->getUGV("30")) {
        gameManager->setUGV("30", true);
    }

    if (auto* mainTitle = getChildByType<CCSprite>(1)) {
        if (!mainTitle->getChildByID("salamandra-loading-subtitle"_spr)) {
            if (auto* subtitle = CCSprite::createWithSpriteFrameName("SubTitle.png"_spr)) {
                subtitle->setID("salamandra-loading-subtitle"_spr);
                subtitle->setPosition({350.f, -18.f});
                subtitle->setScale(0.85f);
                mainTitle->addChild(subtitle);
                mainTitle->setPositionY(mainTitle->getPositionY() + 22.f);
            } else {
                log::warn("Could not load Salamandra sprite: SubTitle.png");
            }
        }
    }

    if (auto* robtopLogo = getChildByType<CCSprite>(2)) {
        if (auto* frame = frames->spriteFrameByName("xTIGDx_Logo_Main.png"_spr)) {
            robtopLogo->setDisplayFrame(frame);
            robtopLogo->setPositionY(robtopLogo->getPositionY() + 10.f);
            robtopLogo->setScale(0.85f);
        } else {
            log::warn("Could not load Salamandra sprite: xTIGDx_Logo_Main.png");
        }
    }

    return true;
}
