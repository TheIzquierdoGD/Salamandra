#include <Geode/Geode.hpp>
#include <Geode/modify/CreatorLayer.hpp>

using namespace geode::prelude;

// Ocultar nodo
inline void hideNode(CCNode* node) {
    if (!node) return;
    node->setVisible(false);
    node->setScale(0.f);
}

// Eliminar CCMenuItemSpriteExtra por ID (forma segura)
static void hideMenuItemByID(CCNode* parent, const std::string& id) {
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
        hideMenuItemByID(child, id);
    }
}

// Ocultar elementos del layer
static void hideLayerElements(CCLayer* layer) {
    if (!layer) return;

    const std::vector<std::string> buttonIDs = {
        "shop-button",
        "shards-button",
        "vault-button",
        "treasure-room-button"
    };

    for (const auto& id : buttonIDs) {
        hideMenuItemByID(layer, id);
    }
}

// Hook
class $modify(CreatorLayer) {
    bool init() {
        if (!CreatorLayer::init())
            return false;

        hideLayerElements(this);

        return true;
    }
};