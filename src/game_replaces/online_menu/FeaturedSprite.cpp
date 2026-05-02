#include <Geode/Geode.hpp>
#include <Geode/modify/LevelBrowserLayer.hpp>

using namespace geode::prelude;

class $modify(MyLevelBrowserLayer, LevelBrowserLayer) {
    bool init(GJSearchObject* p0) {
        auto sfc = CCSpriteFrameCache::get();
        
        auto originalFrame = sfc->spriteFrameByName("featuredLabel_001.png"); // Original featured title sprite from the game
        
        auto customFrame = sfc->spriteFrameByName("theizquierdogd.salamandra/Awards_Title.png"); // Replaces the featured title

        if (originalFrame && customFrame) {
            originalFrame->setTexture(customFrame->getTexture());
            originalFrame->setRect(customFrame->getRect());
            originalFrame->setRotated(customFrame->isRotated());
            originalFrame->setOffset(customFrame->getOffset());
            originalFrame->setOriginalSize(customFrame->getOriginalSize());
        }

        if (!LevelBrowserLayer::init(p0))
            return false;

        return true;
    }
};