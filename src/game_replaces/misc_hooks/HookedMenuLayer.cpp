#include "HookedMenuLayer.h"

using namespace geode::prelude;

bool HookedMenuLayer::init() {
    if(!MenuLayer::init())
        return false;
    
    auto mainTitle = getChildByID("main-title");
    
    if(mainTitle) {
        auto SalamandraTitle = CCSprite::createWithSpriteFrameName("SubTitle.png"_spr); // SubTitle sprite to resources mod folder

        SalamandraTitle->setScale(0.85f); 

        SalamandraTitle->setPosition({350, -18});
        mainTitle->addChild(SalamandraTitle);
    }
    
    return true;
}