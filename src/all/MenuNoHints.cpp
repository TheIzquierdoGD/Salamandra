#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/GJGarageLayer.hpp>

using namespace geode::prelude;

// ===============================
// Menu Principal
// ===============================
class $modify(MenuLayer) {
    void onEnter() {
        MenuLayer::onEnter();

        // Ocultar hint del editor
        if (auto editorHint = this->getChildByID("level-editor-hint")) {
            editorHint->setVisible(false);
        }

        // Ocultar hint de selección de personaje
        if (auto charHint = this->getChildByID("character-select-hint")) {
            charHint->setVisible(false);
        }
    }
};

// ===============================
// Garage
// ===============================
class $modify(GJGarageLayer) {
    void onEnter() {
        GJGarageLayer::onEnter();

        // Ocultar hint del nombre de usuario
        if (auto userHint = this->getChildByID("username-hint")) {
            userHint->setVisible(false);
        }
    }
};