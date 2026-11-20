#include <Geode/Geode.hpp>
#include <Geode/modify/LevelBrowserLayer.hpp>

using namespace geode::prelude;

// Geode v6: estos métodos de CCSpriteFrame siguen disponibles.
// Se conserva el reemplazo previo a init(), porque el juego crea el título
// de Featured Levels durante LevelBrowserLayer::init().
class $modify(SalamandraFeaturedSprite, LevelBrowserLayer) {
    $override
    bool init(GJSearchObject* searchObject) {
        auto* cache = CCSpriteFrameCache::get();
        if (cache) {
            auto* originalFrame = cache->spriteFrameByName("featuredLabel_001.png");
            auto* salamandraFrame = cache->spriteFrameByName(
                "theizquierdogd.salamandra/Awards_Title.png"
            );

            // Mantener la sustitución de la versión anterior, sin desreferenciar
            // frames no cargados (por ejemplo, si falta el atlas de recursos).
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
