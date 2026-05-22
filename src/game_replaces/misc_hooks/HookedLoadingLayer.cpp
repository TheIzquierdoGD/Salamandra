#include "HookedLoadingLayer.h"

using namespace geode::prelude;

bool HookedLoadingLayer::init(bool fromReload) {
    if(!LoadingLayer::init(fromReload))
        return false;
    auto SFC = CCSpriteFrameCache::get();

    auto director = CCDirector::get();
    auto winSize = director->getWinSize();
    auto GM = GameManager::sharedState();

    auto searchPathRoot = dirs::getModRuntimeDir() / Mod::get()->getID() / "resources";
    CCFileUtils::sharedFileUtils()->addSearchPath(searchPathRoot.string().c_str()); 

    SFC->addSpriteFramesWithFile("Salamandra_Principal.plist"_spr);

    auto mainTitle = getChildByType<CCSprite>(1);
    if(!mainTitle) return true;
    
    //Custom subTitle from my resources mod folder
    auto SalamandraTitle = CCSprite::createWithSpriteFrameName("SubTitle.png"_spr);
    if(!SalamandraTitle) return true;
    SalamandraTitle->setPosition({350, -18});
    SalamandraTitle->setScale(0.85f);

    mainTitle->addChild(SalamandraTitle);
    mainTitle->setPositionY(mainTitle->getPositionY() + 22.f);
    
    //Custom RobTop logo from my resources mod folder
    CCSprite* robtopLogo = getChildByType<CCSprite>(2);
    if(!robtopLogo) return true;
    robtopLogo->setDisplayFrame(SFC->spriteFrameByName("xTIGDx_Logo_Main.png"_spr));
    robtopLogo->setPositionY(robtopLogo->getPositionY() + 10.f);

    robtopLogo->setScale(0.85f);

    if(!GM->getUGV("30")) {
        GM->setUGV("30", true);
    }

    return true;
}