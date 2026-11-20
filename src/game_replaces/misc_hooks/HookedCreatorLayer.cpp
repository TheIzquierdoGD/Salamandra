#include "HookedCreatorLayer.h"

using namespace geode::prelude;

bool HookedCreatorLayer::init() {
    if (!CreatorLayer::init()) {
        return false;
    }

    // Wait one frame so Geode / other hooks can finish setting up the menu.
    this->scheduleOnce(
        schedule_selector(HookedCreatorLayer::setupSalamandraLogo),
        0.05f
    );

    return true;
}

void HookedCreatorLayer::setupSalamandraLogo(float) {
    auto* menu = this->getChildByID("creator-buttons-menu");
    if (!menu) {
        return;
    }

    auto* featuredBtn = menu->getChildByID("featured-button");
    if (!featuredBtn || featuredBtn->getChildByID("salamandra-logo-layer"_spr)) {
        return;
    }

    auto* logo = CCSprite::createWithSpriteFrameName("AwardsButton.png"_spr);
    if (!logo) {
        log::warn("Could not load Salamandra sprite: AwardsButton.png");
        return;
    }

    logo->setID("salamandra-logo-layer"_spr);
    logo->setScale(0.75f);
    logo->setPosition(featuredBtn->getContentSize() / 2);
    featuredBtn->addChild(logo);

    if (auto* btn = typeinfo_cast<CCMenuItemSpriteExtra*>(featuredBtn)) {
        btn->updateSprite();
    }
}
