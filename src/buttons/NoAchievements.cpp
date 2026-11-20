// Salamandra: remove Achievements from the main menu - Geode v6
#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>

using namespace geode::prelude;

class $modify(SalamandraNoAchievementsV6, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;

        if (auto menu = getChildByID("bottom-menu")) {
            if (auto button = menu->getChildByID("achievements-button")) {
                button->removeFromParent();
                menu->updateLayout();
            }
        }
        return true;
    }
};
