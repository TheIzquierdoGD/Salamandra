#include "HookedCreatorLayer.h"

using namespace geode::prelude;

bool HookedCreatorLayer::init() {
    if(!CreatorLayer::init())
        return false;

    this->scheduleOnce(schedule_selector(HookedCreatorLayer::setupSalamandraLogo), 0.05f);
    
    return true;
}
void HookedCreatorLayer::setupSalamandraLogo(float dt) {
    auto menu = this->getChildByID("creator-buttons-menu");
    if (!menu) return;

    auto featuredBtn = menu->getChildByID("featured-button");
    
    if(featuredBtn) {
        auto SalamandraLogo = CCSprite::createWithSpriteFrameName("AwardsButton.png"_spr); // Custom featured sprite from my resources mod folder

        if (SalamandraLogo) {
            SalamandraLogo->setScale(0.75f); 

            SalamandraLogo->setPosition(featuredBtn->getContentSize() / 2);
            
            SalamandraLogo->setID("salamandra-logo-layer"_spr);

            featuredBtn->addChild(SalamandraLogo);

            if (auto btn = typeinfo_cast<CCMenuItemSpriteExtra*>(featuredBtn)) {
                btn->updateSprite();
            }
        }
    }
}