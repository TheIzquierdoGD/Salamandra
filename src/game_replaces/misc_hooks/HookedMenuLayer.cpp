#include "HookedMenuLayer.h"

using namespace geode::prelude;

bool HookedMenuLayer::init() {
    if (!MenuLayer::init()) {
        return false;
    }

    auto* mainTitle = getChildByID("main-title");
    if (!mainTitle || mainTitle->getChildByID("salamandra-menu-subtitle"_spr)) {
        return true;
    }

    auto* subtitle = CCSprite::createWithSpriteFrameName("SubTitle.png"_spr);
    if (!subtitle) {
        log::warn("Could not load Salamandra sprite: SubTitle.png");
        return true;
    }

    subtitle->setID("salamandra-menu-subtitle"_spr);
    subtitle->setScale(0.85f);
    subtitle->setPosition({350.f, -18.f});
    mainTitle->addChild(subtitle);

    return true;
}
