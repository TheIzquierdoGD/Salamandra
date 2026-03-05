#include <Geode/Geode.hpp>
#include <Geode/modify/GJGarageLayer.hpp>

using namespace geode::prelude;

// Ocultar nodo de forma segura
inline void hideNode(CCNode* node) {
    if (!node) return;

    node->setVisible(false);
    node->setScale(0.f);

    if (auto rgba = typeinfo_cast<CCRGBAProtocol*>(node)) {
        rgba->setOpacity(0);
    }
}

// Eliminar botón por ID dentro de un CCMenu
static void removeMenuItemByID(CCNode* parent, const std::string& id) {
    if (!parent) return;

    if (auto menu = typeinfo_cast<CCMenu*>(parent)) {
        std::vector<CCMenuItemSpriteExtra*> toRemove;

        for (auto child : CCArrayExt<CCNode*>(menu->getChildren())) {
            if (auto item = typeinfo_cast<CCMenuItemSpriteExtra*>(child)) {
                if (item->getID() == id) {
                    toRemove.push_back(item);
                }
            }
        }

        for (auto item : toRemove) {
            menu->removeChild(item, true);
        }
    }

    for (auto child : CCArrayExt<CCNode*>(parent->getChildren())) {
        removeMenuItemByID(child, id);
    }
}

// Ocultar elementos del Garage
static void hideGarageElements(GJGarageLayer* layer) {
    if (!layer) return;

    const std::vector<std::string> nodeIDs = {
        "orbs-icon",
        "orbs-label",
        "diamonds-icon",
        "diamonds-label",
        "diamond-shards-icon",
        "diamond-shards-label",
        "tap-more-hint"
    };

    for (const auto& id : nodeIDs) {
        if (auto node = layer->getChildByID(id)) {
            hideNode(node);
        }
    }

    removeMenuItemByID(layer, "shop-button");
    removeMenuItemByID(layer, "shards-button");
}

class $modify(GJGarageLayer) {
    bool init() {
        if (!GJGarageLayer::init())
            return false;

        hideGarageElements(this);

        return true;
    }
};