#include <Geode/Geode.hpp>
#include <Geode/modify/LevelSearchLayer.hpp>

using namespace geode::prelude;

class $modify(PlatListBtn, LevelSearchLayer) {
public:
    bool init(int type) {
        if (!LevelSearchLayer::init(type))
            return false;

        // Ejecutar después de que GD termine de construir la UI
        this->scheduleOnce(
            schedule_selector(PlatListBtn::applyText),
            0.f
        );

        return true;
    }

    void applyText(float) {
        // Botón Followed / PlatList
        auto node = this->getChildByIDRecursive("followed-button");
        if (!node) return;

        auto btn = typeinfo_cast<CCMenuItemSpriteExtra*>(node);
        if (!btn) return;

        // Sprite visual real del botón
        auto sprite = btn->getNormalImage();
        if (!sprite) return;

        // Nodo interno que contiene las letras originales
        auto searchBtn = sprite->getChildByType<CCNode>(0);
        if (!searchBtn) return;

        // Ocultar TODAS las letras originales (CCFontSprite)
        if (auto children = searchBtn->getChildren()) {
            for (auto child : CCArrayExt<CCNode*>(children)) {
                if (typeinfo_cast<CCFontSprite*>(child)) {
                    child->setVisible(false);
                }
            }
        }

if (auto children = sprite->getChildren()) {
            for (auto child : CCArrayExt<CCNode*>(children)) {

                // Ignorar el nodo del texto original
                if (child == searchBtn)
                    continue;

                // El icono suele ser un CCSprite
                if (auto icon = typeinfo_cast<CCSprite*>(child)) {
                    auto pos = icon->getPosition();

                    icon->setPosition({
                        pos.x - 3.f, // mismo ajuste fino
                        pos.y
                    });

                    break;
                }
            }
        }

        // Crear texto nuevo
        auto label = CCLabelBMFont::create("Pemon List", "bigFont.fnt");
        if (!label) return;

        label->setAnchorPoint({0.5f, 0.5f});

        // 🔹 Tamaño REAL del texto (control principal)
        label->setScale(0.50f);

        // 🔹 Márgenes internos (evita tocar bordes)
        label->limitLabelWidth(
            75.f,   // ancho efectivo
            3.f,    // escala máxima permitida
            0.f
        );

        auto size = sprite->getContentSize();

        // 🔹 Posicionamiento sutil estilo GD
        label->setPosition(
            size.width  / 2.5f + 0.5f,
            size.height / 1.75f - 1.0f
        );

        // Añadir al sprite para que siga animaciones del botón
        sprite->addChild(label, 10);
    }
};

#include <Geode/Geode.hpp>
#include <Geode/modify/LevelSearchLayer.hpp>

using namespace geode::prelude;

class $modify(DemonListBtn, LevelSearchLayer) {
public:
    bool init(int type) {
        if (!LevelSearchLayer::init(type))
            return false;

        // Ejecutar después de que GD termine de construir la UI
        this->scheduleOnce(
            schedule_selector(DemonListBtn::applyText),
            0.f
        );

        return true;
    }

    void applyText(float) {
        // Botón Awarded
        auto node = this->getChildByIDRecursive("awarded-button");
        if (!node) return;

        auto btn = typeinfo_cast<CCMenuItemSpriteExtra*>(node);
        if (!btn) return;

        // Sprite visual real del botón
        auto sprite = btn->getNormalImage();
        if (!sprite) return;

        // Nodo interno que contiene las letras originales
        auto searchBtn = sprite->getChildByType<CCNode>(0);
        if (!searchBtn) return;

        // Ocultar TODAS las letras originales
        if (auto children = searchBtn->getChildren()) {
            for (auto child : CCArrayExt<CCNode*>(children)) {
                if (typeinfo_cast<CCFontSprite*>(child)) {
                    child->setVisible(false);
                }
            }
        }

        // Texto nuevo
        auto label = CCLabelBMFont::create("Demon List", "bigFont.fnt");
        if (!label) return;

        label->setAnchorPoint({0.5f, 0.5f});

        // Tamaño real del texto
        label->setScale(0.50f);

        // Márgenes internos (idéntico al otro botón)
        label->limitLabelWidth(
            75.f,
            3.f,
            0.f
        );

        auto size = sprite->getContentSize();

        // Posicionamiento estilo GD
        label->setPosition(
            size.width  / 2.5f + 0.5f,
            size.height / 1.75f - 1.0f
        );

        // Añadir al sprite para que siga animaciones
        sprite->addChild(label, 10);
    }
};

#include <Geode/Geode.hpp>
#include <Geode/modify/LevelSearchLayer.hpp>

using namespace geode::prelude;

class $modify(DislikesBtn, LevelSearchLayer) {
public:
    bool init(int type) {
        if (!LevelSearchLayer::init(type))
            return false;

        // Ejecutar después de que GD termine de construir la UI
        this->scheduleOnce(
            schedule_selector(DislikesBtn::applyText),
            0.f
        );

        return true;
    }

    void applyText(float) {
        // Botón Friends
        auto node = this->getChildByIDRecursive("friends-button");
        if (!node) return;

        auto btn = typeinfo_cast<CCMenuItemSpriteExtra*>(node);
        if (!btn) return;

        // Sprite visual real del botón
        auto sprite = btn->getNormalImage();
        if (!sprite) return;

        // Nodo interno que contiene las letras originales
        auto searchBtn = sprite->getChildByType<CCNode>(0);
        if (!searchBtn) return;

        // Ocultar TODAS las letras originales
        if (auto children = searchBtn->getChildren()) {
            for (auto child : CCArrayExt<CCNode*>(children)) {
                if (typeinfo_cast<CCFontSprite*>(child)) {
                    child->setVisible(false);
                }
            }
        }

        // Texto nuevo
        auto label = CCLabelBMFont::create(" Dislikes ", "bigFont.fnt");
        if (!label) return;

        label->setAnchorPoint({0.5f, 0.5f});

        // Tamaño real del texto
        label->setScale(0.50f);

        // Márgenes internos (consistentes con GD)
        label->limitLabelWidth(
            75.f,
            3.f,
            0.f
        );

        auto size = sprite->getContentSize();

        // Posicionamiento estilo GD
        label->setPosition(
            size.width  / 2.5f + 0.5f,
            size.height / 1.75f - 1.0f
        );

        // Añadir al sprite para conservar animaciones
        sprite->addChild(label, 10);
    }
};

#include <Geode/Geode.hpp>
#include <Geode/modify/LevelSearchLayer.hpp>

using namespace geode::prelude;

class $modify(MagicBtnText, LevelSearchLayer) {
public:
    bool init(int type) {
        if (!LevelSearchLayer::init(type))
            return false;

        // Ejecutar cuando la UI ya esté completamente construida
        this->scheduleOnce(
            schedule_selector(MagicBtnText::applyText),
            0.f
        );

        return true;
    }

    void applyText(float) {
        // =========================
        // Obtener botón "Magic"
        // =========================
        auto node = this->getChildByIDRecursive("magic-button");
        if (!node) return;

        auto btn = typeinfo_cast<CCMenuItemSpriteExtra*>(node);
        if (!btn) return;

        auto sprite = btn->getNormalImage();
        if (!sprite) return;

        // Nodo que contiene las letras originales
        auto searchBtn = sprite->getChildByType<CCNode>(0);
        if (!searchBtn) return;

        // =========================
        // Ocultar texto original
        // =========================
        if (auto children = searchBtn->getChildren()) {
            for (auto child : CCArrayExt<CCNode*>(children)) {
                if (typeinfo_cast<CCFontSprite*>(child)) {
                    child->setVisible(false);
                }
            }
        }

        // =========================
        // Mover icono lateral (MISMA FÓRMULA)
        // =========================
        if (auto children = sprite->getChildren()) {
            for (auto child : CCArrayExt<CCNode*>(children)) {

                // Ignorar el nodo del texto original
                if (child == searchBtn)
                    continue;

                // El icono suele ser un CCSprite
                if (auto icon = typeinfo_cast<CCSprite*>(child)) {
                    auto pos = icon->getPosition();

                    icon->setPosition({
                        pos.x - -12.f, // mismo ajuste fino
                        pos.y
                    });

                    break;
                }
            }
        }

        // =========================
        // Texto nuevo
        // =========================
        auto label = CCLabelBMFont::create("New Rates", "bigFont.fnt");
        if (!label) return;

        label->setAnchorPoint({0.5f, 0.5f});

        label->limitLabelWidth(
            75.f,
            3.f,
            0.f
        );

        label->setScale(0.42f);

        auto size = sprite->getContentSize();

        label->setPosition(
            size.width  / 2.5f + 0.5f,
            size.height / 1.75f - 1.0f
        );

        // Añadir al sprite para conservar animaciones
        sprite->addChild(label, 10);
    }
};



#include <Geode/Geode.hpp>
#include <Geode/modify/LevelSearchLayer.hpp>

using namespace geode::prelude;

class $modify(SentsBtn, LevelSearchLayer) {
public:
    bool init(int type) {
        if (!LevelSearchLayer::init(type))
            return false;

        // Ejecutar cuando la UI ya esté completamente construida
        this->scheduleOnce(
            schedule_selector(SentsBtn::applyText),
            0.f
        );

        return true;
    }

    void applyText(float) {
        // =========================
        // Obtener botón "Sent"
        // =========================
        auto node = this->getChildByIDRecursive("sent-button");
        if (!node) return;

        auto btn = typeinfo_cast<CCMenuItemSpriteExtra*>(node);
        if (!btn) return;

        auto sprite = btn->getNormalImage();
        if (!sprite) return;

        // Nodo que contiene las letras originales
        auto searchBtn = sprite->getChildByType<CCNode>(0);
        if (!searchBtn) return;

        // =========================
        // Ocultar texto original
        // =========================
        if (auto children = searchBtn->getChildren()) {
            for (auto child : CCArrayExt<CCNode*>(children)) {
                if (typeinfo_cast<CCFontSprite*>(child)) {
                    child->setVisible(false);
                }
            }
        }

        // =========================
        // Mover icono lateral
        // =========================
        if (auto children = sprite->getChildren()) {
            for (auto child : CCArrayExt<CCNode*>(children)) {

                // Ignorar el nodo del texto original
                if (child == searchBtn)
                    continue;

                // El icono suele ser un CCSprite
                if (auto icon = typeinfo_cast<CCSprite*>(child)) {
                    auto pos = icon->getPosition();

                    icon->setPosition({
                        pos.x - -17.f, // ajuste horizontal fino
                        pos.y
                    });

                    break;
                }
            }
        }

        // =========================
        // Texto nuevo: "Sents"
        // =========================
        auto label = CCLabelBMFont::create("Mod Sents", "bigFont.fnt");
        if (!label) return;

        label->setAnchorPoint({0.5f, 0.5f});

        // Ancho y compresión estilo GD
        label->limitLabelWidth(
            75.f,
            3.f,
            0.f
        );

        // Escala efectiva
        label->setScale(0.42f);

        auto size = sprite->getContentSize();

        // Posicionamiento consistente con botones nativos
        label->setPosition(
            size.width  / 2.5f + 0.5f,
            size.height / 1.75f - 1.0f
        );

        // Añadir al sprite para conservar animaciones
        sprite->addChild(label, 10);
    }
};
