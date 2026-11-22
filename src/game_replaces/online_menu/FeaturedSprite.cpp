#include <Geode/Geode.hpp>
#include <Geode/modify/LevelBrowserLayer.hpp>

using namespace geode::prelude;

class $modify(SalamandraFeaturedSprite, LevelBrowserLayer) {
    $override
    bool init(GJSearchObject* searchObject) {
        auto* cache = CCSpriteFrameCache::get();
        if (cache) {
            auto* originalFrame = cache->spriteFrameByName("featuredLabel_001.png");
            auto* salamandraFrame = cache->spriteFrameByName(
                "theizquierdogd.salamandra/Awards_Title.png"
            );

            if (originalFrame && salamandraFrame && originalFrame != salamandraFrame) {
                originalFrame->setTexture(salamandraFrame->getTexture());
                originalFrame->setRect(salamandraFrame->getRect());
                originalFrame->setRotated(salamandraFrame->isRotated());
                originalFrame->setOffset(salamandraFrame->getOffset());
                originalFrame->setOriginalSize(salamandraFrame->getOriginalSize());
            }
        }

        return LevelBrowserLayer::init(searchObject);
    }
};
