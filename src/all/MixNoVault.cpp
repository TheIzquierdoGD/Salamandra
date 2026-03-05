#include <Geode/Geode.hpp>
#include <Geode/modify/GJDropDownLayer.hpp>
#include <Geode/modify/CreatorLayer.hpp>
#include <Geode/modify/MenuLayer.hpp>

using namespace geode::prelude;

//////////////////////////////////////////////////////
// Utilidad: eliminar CCMenuItemSpriteExtra por ID
//////////////////////////////////////////////////////

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

//////////////////////////////////////////////////////
// Utilidad: eliminar nodo completo
//////////////////////////////////////////////////////

static void removeNode(CCNode* node) {
    if (!node) return;
    if (node->getParent()) {
        node->getParent()->removeChild(node, true);
    }
}

//////////////////////////////////////////////////////
// GJDropDownLayer
//////////////////////////////////////////////////////

class $modify(GJDropDownLayer) {
    bool init(const char* title, float height, bool something) {
        if (!GJDropDownLayer::init(title, height, something))
            return false;

        removeMenuItemByID(this, "vault-button");

        return true;
    }
};

//////////////////////////////////////////////////////
// CreatorLayer
//////////////////////////////////////////////////////

class $modify(CreatorLayer) {
    bool init() {
        if (!CreatorLayer::init())
            return false;

        if (auto node = this->getChildByID("bottom-left-corner")) {
            removeNode(node);
        }

        return true;
    }
};

//////////////////////////////////////////////////////
// MenuLayer
//////////////////////////////////////////////////////

class $modify(MenuLayer) {
    bool init() {
        if (!MenuLayer::init())
            return false;

        removeMenuItemByID(this, "daily-chest-button");

        return true;
    }
};