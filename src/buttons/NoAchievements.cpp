#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>

using namespace geode::prelude;

class $modify(MyExternalButtonLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init())
            return false;

        auto bottomMenu = this->getChildByID("bottom-menu");
        if (!bottomMenu)
            return true;

        if (auto achBtn = bottomMenu->getChildByID("achievements-button")) {
            achBtn->removeFromParent();
        }

        bottomMenu->updateLayout();

        return true;
    }
};