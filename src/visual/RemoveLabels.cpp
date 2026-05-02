#include <Geode/Geode.hpp>
#include <Geode/modify/GJGarageLayer.hpp>
#include <Geode/modify/GJDropDownLayer.hpp>
#include <Geode/modify/CreatorLayer.hpp>
#include <Geode/modify/MenuLayer.hpp>

using namespace geode::prelude;

// Utility Functions
inline void hideNode(CCNode* node) {
    if (!node) return;

    node->setVisible(false);
    node->setScale(0.f);

    if (auto rgba = typeinfo_cast<CCRGBAProtocol*>(node)) {
        rgba->setOpacity(0);
    }
}

static void removeNode(CCNode* node) {
    if (!node) return;
    if (node->getParent()) {
        node->getParent()->removeChild(node, true);
    }
}

static void removeMenuItemByID(CCNode* parent, const std::string& id) {
    if (!parent) return;

    if (auto menu = typeinfo_cast<CCMenu*>(parent)) {
        std::vector<CCNode*> toRemove;

        for (auto child : CCArrayExt<CCNode*>(menu->getChildren())) {
            if (child->getID() == id) {
                toRemove.push_back(child);
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

// Elements lists
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

// Element Lists (Shared for CreatorLayer)
static void hideLayerElements(CCLayer* layer) {
    if (!layer) return;

    const std::vector<std::string> buttonIDs = {
        "shop-button",
        "shards-button",
        "vault-button",
        "treasure-room-button"
    };

    for (const auto& id : buttonIDs) {
        removeMenuItemByID(layer, id);
    }
}

// Hooks
class $modify(GJGarageLayer) {
    bool init() {
        if (!GJGarageLayer::init())
            return false;

        hideGarageElements(this);

        return true;
    }
};

// Bye vault-button
class $modify(GJDropDownLayer) {
    bool init(const char* title, float height, bool something) {
        if (!GJDropDownLayer::init(title, height, something))
            return false;

        removeMenuItemByID(this, "vault-button");

        return true;
    }
};

// Bye bottom-left-corner & Hide Creator Elements
class $modify(CreatorLayer) {
    bool init() {
        if (!CreatorLayer::init())
            return false;

        // Logic from "Bye bottom-left-corner"
        if (auto node = this->getChildByID("bottom-left-corner")) {
            removeNode(node);
        }

        // Logic from "Hook" and "Element Lists"
        hideLayerElements(this);

        return true;
    }
};

// Bye daily-chest-button
class $modify(MenuLayer) {
    bool init() {
        if (!MenuLayer::init())
            return false;

        removeMenuItemByID(this, "daily-chest-button");

        return true;
    }
};