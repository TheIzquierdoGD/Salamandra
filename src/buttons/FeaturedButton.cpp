#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>

using namespace geode::prelude;

class $modify(MyGlobalReplaces, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;

        this->scheduleOnce(schedule_selector(MyGlobalReplaces::performGlobalReplaces), 0.01f);

        return true;
    }

    void performGlobalReplaces(float dt) {
        auto sfc = CCSpriteFrameCache::get();

        auto originalFrame = sfc->spriteFrameByName("GJ_featuredBtn_001.png");
        auto geodeFrame = sfc->spriteFrameByName("geode.loader/baseCategory_Big_Green.png");

        if (originalFrame && geodeFrame) {
            originalFrame->setTexture(geodeFrame->getTexture());
            originalFrame->setRect(geodeFrame->getRect());
            originalFrame->setRotated(geodeFrame->isRotated());
            originalFrame->setOffset(geodeFrame->getOffset());
            originalFrame->setOriginalSize(geodeFrame->getOriginalSize());
        }
    }
};