#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/CreatorLayer.hpp>

class $modify(HookedCreatorLayer, CreatorLayer) {
    bool init();
    void setupSalamandraLogo(float dt);

    #ifdef DEBUG_BUILD
    virtual void keyDown(cocos2d::enumKeyCodes);
    #endif
};