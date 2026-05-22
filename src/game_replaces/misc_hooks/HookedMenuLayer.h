#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>

class $modify(HookedMenuLayer, MenuLayer) {
    bool init();

    #ifdef DEBUG_BUILD
    virtual void keyDown(cocos2d::enumKeyCodes);
    #endif
    
};