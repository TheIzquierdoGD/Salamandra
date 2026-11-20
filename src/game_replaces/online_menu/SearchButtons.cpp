#include <Geode/Geode.hpp>
#include <Geode/modify/LevelSearchLayer.hpp>
#include <Geode/ui/Label.hpp>

using namespace geode::prelude;

namespace {
    struct ButtonStyle {
        char const* buttonID;
        char const* text;
        char const* replacementFrame; // nullptr: mantener icono existente
        float iconScale;               // 0: mantener escala existente
        float iconOffsetX;
        float iconOffsetY;
        float labelScale;
    };

    // Los valores se mantienen iguales a los de Geode v5.
    constexpr ButtonStyle kButtons[] = {
        { "followed-button", "Pemon List", "GJ_moonsIcon_001.png",    0.800f, -3.0f,  0.0f, 0.50f },
        { "awarded-button",  "Demon List", nullptr,                    0.0f,  0.0f,  0.0f, 0.50f },
        { "friends-button",  " Dislikes ", "GJ_dislikesIcon_001.png", 0.700f,  0.0f,  3.75f, 0.50f },
        { "magic-button",    "New Rates",  nullptr,                    0.0f, 12.0f,  0.0f, 0.42f },
        { "sent-button",     "Mod Sents",  nullptr,                    0.0f, 17.0f,  0.0f, 0.42f },
    };

    void updateSearchButton(LevelSearchLayer* layer, ButtonStyle const& style) {
        auto* node = layer->getChildByIDRecursive(style.buttonID);
        auto* button = typeinfo_cast<CCMenuItemSpriteExtra*>(node);
        if (!button) return;

        auto* sprite = button->getNormalImage();
        if (!sprite) return;

        // Si otra llamada intenta volver a aplicar el cambio, no lo duplica.
        if (sprite->getChildByID("salamandra-search-label"_spr)) return;

        // Ocultar los caracteres originales de los botones de Geometry Dash.
        auto* content = sprite->getChildByType<CCNode>(0);
        if (!content) return;

        if (auto* children = content->getChildren()) {
            for (auto* child : CCArrayExt<CCNode*>(children)) {
                if (typeinfo_cast<CCFontSprite*>(child)) {
                    child->setVisible(false);
                }
            }
        }

        // Reemplazar o mover el primer icono, sin alterar la estructura del botón.
        if (auto* children = sprite->getChildren()) {
            for (auto* child : CCArrayExt<CCNode*>(children)) {
                if (child == content) continue;

                if (auto* icon = typeinfo_cast<CCSprite*>(child)) {
                    bool canAdjust = true;
                    if (style.replacementFrame) {
                        auto* frame = CCSpriteFrameCache::get()->spriteFrameByName(
                            style.replacementFrame
                        );
                        if (!frame) {
                            canAdjust = false;
                        } else {
                            icon->setDisplayFrame(frame);
                        }
                    }

                    if (canAdjust) {
                        if (style.iconScale > 0.0f) {
                            icon->setScale(style.iconScale);
                        }
                        icon->setPosition({
                            icon->getPositionX() + style.iconOffsetX,
                            icon->getPositionY() + style.iconOffsetY
                        });
                    }
                    break;
                }
            }
        }

        // Geode v6: Label reemplaza opcionalmente a CCLabelBMFont.
        auto* label = geode::Label::create(style.text, "bigFont.fnt");
        if (!label) return;

        label->setID("salamandra-search-label"_spr);
        label->setAnchorPoint({0.5f, 0.5f});
        label->setLimitLabelWidth(75.f, 3.f, 0.f);
        label->setScale(style.labelScale);

        auto size = sprite->getContentSize();
        label->setPosition({
            size.width / 2.5f + 0.5f,
            size.height / 1.75f - 1.0f
        });
        sprite->addChild(label, 10);
    }
}

// En v5 había cinco hooks diferentes del mismo init(). Se unen en uno para
// reducir interacciones entre hooks y ejecutar una sola actualización diferida.
class $modify(SalamandraSearchButtons, LevelSearchLayer) {
    $override
    bool init(int type) {
        if (!LevelSearchLayer::init(type)) return false;

        this->scheduleOnce(
            schedule_selector(SalamandraSearchButtons::applyCustomButtons),
            0.f
        );
        return true;
    }

    void applyCustomButtons(float) {
        for (auto const& style : kButtons) {
            updateSearchButton(this, style);
        }
    }
};
