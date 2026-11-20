// Salamandra featured icon replacement - Geode v6
#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>

using namespace geode::prelude;

class $modify(SalamandraFeaturedButtonV6, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;
        scheduleOnce(schedule_selector(SalamandraFeaturedButtonV6::performGlobalReplaces), 0.01f);
        return true;
    }

    void performGlobalReplaces(float) {
        auto cache = CCSpriteFrameCache::get();
        if (!cache) return;

        auto original = cache->spriteFrameByName("GJ_featuredBtn_001.png");
        auto replacement = cache->spriteFrameByName("geode.loader/baseCategory_Big_Green.png");
        if (!original || !replacement) {
            log::debug("Salamandra: Featured frame or Geode replacement not found");
            return;
        }

        // Preserve original behavior: replace the global frame, not just one sprite.
        original->setTexture(replacement->getTexture());
        original->setRect(replacement->getRect());
        original->setRotated(replacement->isRotated());
        original->setOffset(replacement->getOffset());
        original->setOriginalSize(replacement->getOriginalSize());
    }
};
