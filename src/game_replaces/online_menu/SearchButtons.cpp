#include <Geode/Geode.hpp>
#include <Geode/modify/LevelSearchLayer.hpp>

using namespace geode::prelude;

class $modify(PlatListBtn, LevelSearchLayer) {
public:
    bool init(int type) {
        if (!LevelSearchLayer::init(type))
            return false;

        this->scheduleOnce(
            schedule_selector(PlatListBtn::applyText),
            0.f
        );

        return true;
    }

    void applyText(float) {
        auto node = this->getChildByIDRecursive("followed-button");
        if (!node) return;

        auto btn = typeinfo_cast<CCMenuItemSpriteExtra*>(node);
        if (!btn) return;

        auto sprite = btn->getNormalImage();
        if (!sprite) return;

        auto searchBtn = sprite->getChildByType<CCNode>(0);
        if (!searchBtn) return;

        if (auto children = searchBtn->getChildren()) {
            for (auto child : CCArrayExt<CCNode*>(children)) {
                if (typeinfo_cast<CCFontSprite*>(child)) {
                    child->setVisible(false);
                }
            }
        }

        if (auto children = sprite->getChildren()) {
            for (auto child : CCArrayExt<CCNode*>(children)) {
                if (child == searchBtn) continue;

                if (auto icon = typeinfo_cast<CCSprite*>(child)) {
                    auto newFrame = CCSpriteFrameCache::get()->spriteFrameByName("GJ_moonsIcon_001.png");
                    
                    if (newFrame) {
                        icon->setDisplayFrame(newFrame);
                        icon->setScale(0.800f);
                        icon->setPosition({
                            icon->getPositionX() - 3.0f,
                            icon->getPositionY()
                        });
                    }
                    break;
                }
            }
        }

        auto label = CCLabelBMFont::create("Pemon List", "bigFont.fnt");
        if (!label) return;

        label->setAnchorPoint({0.5f, 0.5f});
        label->setScale(0.50f);
        label->limitLabelWidth(75.f, 3.f, 0.f);

        auto size = sprite->getContentSize();
        label->setPosition(
            size.width  / 2.5f + 0.5f,
            size.height / 1.75f - 1.0f
        );

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

        this->scheduleOnce(
            schedule_selector(DemonListBtn::applyText),
            0.f
        );

        return true;
    }

    void applyText(float) {
        // Awarded button
        auto node = this->getChildByIDRecursive("awarded-button");
        if (!node) return;

        auto btn = typeinfo_cast<CCMenuItemSpriteExtra*>(node);
        if (!btn) return;

        // Button sprite
        auto sprite = btn->getNormalImage();
        if (!sprite) return;

        auto searchBtn = sprite->getChildByType<CCNode>(0);
        if (!searchBtn) return;

        if (auto children = searchBtn->getChildren()) {
            for (auto child : CCArrayExt<CCNode*>(children)) {
                if (typeinfo_cast<CCFontSprite*>(child)) {
                    child->setVisible(false);
                }
            }
        }

        // New text
        auto label = CCLabelBMFont::create("Demon List", "bigFont.fnt");
        if (!label) return;

        label->setAnchorPoint({0.5f, 0.5f});

        // Real size
        label->setScale(0.50f);

        // Limits
        label->limitLabelWidth(
            75.f,
            3.f,
            0.f
        );

        auto size = sprite->getContentSize();

        // Subtle GD-style positioning
        label->setPosition(
            size.width  / 2.5f + 0.5f,
            size.height / 1.75f - 1.0f
        );

        sprite->addChild(label, 10);
    }
};

//And so it went with all the following ones, I got tired of repeating myself XD

#include <Geode/Geode.hpp>
#include <Geode/modify/LevelSearchLayer.hpp>

using namespace geode::prelude;

class $modify(DislikesBtn, LevelSearchLayer) {
public:
    bool init(int type) {
        if (!LevelSearchLayer::init(type))
            return false;

        // Schedule the text and icon replacement for the next frame
        this->scheduleOnce(
            schedule_selector(DislikesBtn::applyText),
            0.f
        );

        return true;
    }

    void applyText(float) {
        // Find the "friends-button" within the search menu
        auto node = this->getChildByIDRecursive("friends-button");
        if (!node) return;

        auto btn = typeinfo_cast<CCMenuItemSpriteExtra*>(node);
        if (!btn) return;

        auto sprite = btn->getNormalImage();
        if (!sprite) return;

        // Get the inner node that contains the original "Friends" font
        auto searchBtn = sprite->getChildByType<CCNode>(0);
        if (!searchBtn) return;

        // 1. Hide the original font sprites
        if (auto children = searchBtn->getChildren()) {
            for (auto child : CCArrayExt<CCNode*>(children)) {
                if (typeinfo_cast<CCFontSprite*>(child)) {
                    child->setVisible(false);
                }
            }
        }

        // 2. Replace the icon and fix its alignment/size
        if (auto children = sprite->getChildren()) {
            for (auto child : CCArrayExt<CCNode*>(children)) {
                if (child == searchBtn) continue;

                if (auto icon = typeinfo_cast<CCSprite*>(child)) {
                    auto newFrame = CCSpriteFrameCache::get()->spriteFrameByName("GJ_dislikesIcon_001.png");
                    
                    if (newFrame) {
                        icon->setDisplayFrame(newFrame);
                        
                        // Set fixed scale to 0.700f as requested
                        icon->setScale(0.700f);

                        // FIX: Translation from initial (87.25, 17.75) to final (91.25, 19.0)
                        // X difference is +4.0f, Y difference is +1.25f
                        // Combined with previous manual centering (+2.5f on Y)
                        icon->setPosition({
                            icon->getPositionX() + 0.0f, // X is now zeroed out based on your final position
                            icon->getPositionY() + 3.75f // Increased Y to reach the final 19.000 height
                        });
                    }
                    break;
                }
            }
        }

        // 3. Create and add the new " Dislikes " label
        auto label = CCLabelBMFont::create(" Dislikes ", "bigFont.fnt");
        if (!label) return;

        label->setAnchorPoint({0.5f, 0.5f});
        label->setScale(0.50f);

        // Limit the width so it fits within the button area
        label->limitLabelWidth(75.f, 3.f, 0.f);

        auto size = sprite->getContentSize();

        // Standard positioning for the label
        label->setPosition(
            size.width  / 2.5f + 0.5f,
            size.height / 1.75f - 1.0f
        );

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

        this->scheduleOnce(
            schedule_selector(MagicBtnText::applyText),
            0.f
        );

        return true;
    }

    void applyText(float) {

        auto node = this->getChildByIDRecursive("magic-button");
        if (!node) return;

        auto btn = typeinfo_cast<CCMenuItemSpriteExtra*>(node);
        if (!btn) return;

        auto sprite = btn->getNormalImage();
        if (!sprite) return;

        auto searchBtn = sprite->getChildByType<CCNode>(0);
        if (!searchBtn) return;

        if (auto children = searchBtn->getChildren()) {
            for (auto child : CCArrayExt<CCNode*>(children)) {
                if (typeinfo_cast<CCFontSprite*>(child)) {
                    child->setVisible(false);
                }
            }
        }

        if (auto children = sprite->getChildren()) {
            for (auto child : CCArrayExt<CCNode*>(children)) {

                if (child == searchBtn)
                    continue;

                if (auto icon = typeinfo_cast<CCSprite*>(child)) {
                    auto pos = icon->getPosition();

                    icon->setPosition({
                        pos.x - -12.f,
                        pos.y
                    });

                    break;
                }
            }
        }

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

        this->scheduleOnce(
            schedule_selector(SentsBtn::applyText),
            0.f
        );

        return true;
    }

    void applyText(float) {

        auto node = this->getChildByIDRecursive("sent-button");
        if (!node) return;

        auto btn = typeinfo_cast<CCMenuItemSpriteExtra*>(node);
        if (!btn) return;

        auto sprite = btn->getNormalImage();
        if (!sprite) return;

        auto searchBtn = sprite->getChildByType<CCNode>(0);
        if (!searchBtn) return;

        if (auto children = searchBtn->getChildren()) {
            for (auto child : CCArrayExt<CCNode*>(children)) {
                if (typeinfo_cast<CCFontSprite*>(child)) {
                    child->setVisible(false);
                }
            }
        }

        if (auto children = sprite->getChildren()) {
            for (auto child : CCArrayExt<CCNode*>(children)) {

                if (child == searchBtn)
                    continue;

                if (auto icon = typeinfo_cast<CCSprite*>(child)) {
                    auto pos = icon->getPosition();

                    icon->setPosition({
                        pos.x - -17.f,
                        pos.y
                    });

                    break;
                }
            }
        }

        auto label = CCLabelBMFont::create("Mod Sents", "bigFont.fnt");
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

        sprite->addChild(label, 10);
    }
};
