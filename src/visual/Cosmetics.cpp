#include <Geode/Geode.hpp>
#include <Geode/modify/GameManager.hpp>
#include <Geode/modify/GameStatsManager.hpp>
#include <Geode/modify/CCTextInputNode.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/AchievementBar.hpp>
#include <Geode/modify/GameToolbox.hpp>
#include <Geode/modify/MenuLayer.hpp>

using namespace geode::prelude;

// Global Toggles
bool g_iconBypassEnabled = true;
bool g_charLimitBypass = true;
bool g_charFilterBypass = true;

// Unloock all icons hacks by TheSillyLogo
class $modify(GameManager) {
    bool isIconUnlocked(int id, IconType type) {
        if (g_iconBypassEnabled) return true;
        return GameManager::isIconUnlocked(id, type);
    }

    bool isColorUnlocked(int id, UnlockType type) {
        if (g_iconBypassEnabled) return true;
        return GameManager::isColorUnlocked(id, type);
    }
};

class $modify(GameStatsManager) {
    bool isItemUnlocked(UnlockType type, int id) {
        if (g_iconBypassEnabled && type == UnlockType::GJItem) {
            // Unlocking specific item IDs
            if (id == 18 || id == 19 || id == 20 || id == 16 || id == 17)
                return true;
        }
        return GameStatsManager::isItemUnlocked(type, id);
    }
};

// Text bypass
class $modify(CCTextInputNode) {
    void updateLabel(gd::string p0) {
        std::string id = this->getID();

        if (id.find("IGNOREBYPASSES") != std::string::npos) {
            CCTextInputNode::updateLabel(p0);
            return;
        }

        if (g_charLimitBypass) {
            this->setMaxLabelLength(6969);
        }

        if (g_charFilterBypass) {
            this->setAllowedChars(
                "!\"#$%&'()*+,-./0123456789:;<=>?@"
                "ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`"
                "abcdefghijklmnopqrstuvwxyz{|}~ "
            );
        }

        CCTextInputNode::updateLabel(p0);
    }
};

// No short numbers
class $modify(GameToolbox) {
    static gd::string intToShortString(int num) {
        return gd::string(std::to_string(num).c_str());
    }
};

// Achievement old animation
struct HookAchievementBar : geode::Modify<HookAchievementBar, AchievementBar> {
	bool init(char const* title, char const* desc, char const* icon, bool quest) {
		if (!AchievementBar::init(title, desc, icon, quest))
			return false;
		
		m_achievementGlowSprite->setVisible(false);
		m_bg->setContentSize({ 300.f, 70.f });
		m_layerColor->setPositionY(35.f);
		m_titleLabel->setPositionY(11.5);

		if (!geode::Mod::get()->getSettingValue<bool>("squareColour"))
			return true;
		
		std::string iconStr = icon;
		if (!quest && iconStr.starts_with("color")) {
			auto sfc = cocos2d::CCSpriteFrameCache::get();
			m_achievementDescription->setString("Unlocked new Color!");

			if (auto item = geode::cast::typeinfo_cast<GJItemIcon*>(m_achievementSprite)) {
				item->m_player->setDisplayFrame(sfc->spriteFrameByName("playerSquare_001.png"));
			}
		}
	
		return true;
	}
	
	void show() {
		auto scene = cocos2d::CCScene::get();
		auto director = cocos2d::CCDirector::get();
		
		int highestZ = scene->getHighestChildZ();
		highestZ++;
		
		scene->addChild(this, highestZ);
		
		this->setPositionY(director->getScreenTop() + 2.f);
		
		auto moveDown = cocos2d::CCEaseInOut::create(cocos2d::CCMoveBy::create(1.f, ccp(.0f, -74)), 2.f);
		auto delay1P5 = cocos2d::CCDelayTime::create(1.5f);
		
		auto moveUp = cocos2d::CCEaseInOut::create(cocos2d::CCMoveBy::create(1.f, ccp(.0f, 74)), 2.f);
		auto callFunc = cocos2d::CCCallFunc::create(AchievementNotifier::sharedState(), callfunc_selector(AchievementNotifier::achievementDisplayFinished));
		
		auto sequence = cocos2d::CCSequence::create(
			moveDown,
			delay1P5,
			moveUp,
			callFunc,
			0
		);
		this->runAction(sequence);
	}
};

#ifdef DEBUG_BUILD

class $modify(MenuLayer) {
	void keyDown(cocos2d::enumKeyCodes key) {
		if (key == cocos2d::enumKeyCodes::KEY_G) {
			AchievementNotifier::sharedState()->notifyAchievement("Quest Complete!", "Collect 200 Mana Orbs", "currencyOrbIcon_001.png", true);
		} 

		if (key == cocos2d::enumKeyCodes::KEY_D) {
			AchievementNotifier::sharedState()->notifyAchievement("Test Achievement", "aaaaaa", "color_11", false);
		}
		MenuLayer::keyDown(key);
	}
};
#endif

// Coins on practice mode
class $modify(GJBaseGameLayer) {
    void collisionCheckObjects(PlayerObject* p0, gd::vector<GameObject*>* p1, int p2, float p3) {
        // Run original logic first to handle normal collisions
        GJBaseGameLayer::collisionCheckObjects(p0, p1, p2, p3);

        if (m_isPracticeMode && p0 && p1) {
            for (auto obj : *p1) {
                if (p0->getObjectRect().intersectsRect(obj->getObjectRect())) {
                    if (obj->m_objectType == GameObjectType::SecretCoin || obj->m_objectType == GameObjectType::UserCoin) {
                        if (obj->getOpacity() != 0) {
                            this->destroyObject(obj);
                        }
                    }
                }
            }
        }
    }
};