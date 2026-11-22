#include <Geode/Geode.hpp>
#include <Geode/modify/GameManager.hpp>
#include <Geode/modify/GameStatsManager.hpp>
#include <Geode/modify/CCTextInputNode.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/AchievementBar.hpp>
#include <Geode/modify/GameToolbox.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/binding/GJItemIcon.hpp>
#include <vector>
#include <string>

using namespace geode::prelude;

namespace {
 bool g_iconBypassEnabled = true;
bool g_charLimitBypass = true;
bool g_charFilterBypass = true;
}

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
            if (id == 18 || id == 19 || id == 20 || id == 16 || id == 17)
                return true;
        }
        return GameStatsManager::isItemUnlocked(type, id);
    }
};

class $modify(CCTextInputNode) {
    void updateLabel(std::string p0) {
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

class $modify(GameToolbox) {
    static std::string intToShortString(int num, bool) {
        return std::to_string(num);
    }
};

struct HookAchievementBar : geode::Modify<HookAchievementBar, AchievementBar> {
	bool init(char const* title, char const* desc, char const* icon, bool quest) {
		if (!AchievementBar::init(title, desc, icon, quest))
			return false;
		
		if (m_achievementGlowSprite) m_achievementGlowSprite->setVisible(false);
		if (m_bg) m_bg->setContentSize({ 300.f, 70.f });
		if (m_layerColor) m_layerColor->setPositionY(35.f);
		if (m_titleLabel) m_titleLabel->setPositionY(11.5f);

		if (!geode::Mod::get()->getSettingValue<bool>("squareColour"))
			return true;
		
		std::string iconStr = icon ? icon : "";
		if (!quest && iconStr.starts_with("color")) {
			auto sfc = cocos2d::CCSpriteFrameCache::get();
			if (m_achievementDescription) m_achievementDescription->setString("Unlocked new Color!");

			if (auto item = typeinfo_cast<GJItemIcon*>(m_achievementSprite)) {
				if (item->m_player) {
                    if (auto frame = sfc->spriteFrameByName("playerSquare_001.png"))
                        item->m_player->setDisplayFrame(frame);
                }
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
	void keyDown(cocos2d::enumKeyCodes key, double timestamp) {
		if (key == cocos2d::enumKeyCodes::KEY_G) {
			AchievementNotifier::sharedState()->notifyAchievement("Quest Complete!", "Collect 200 Mana Orbs", "currencyOrbIcon_001.png", true);
		} 

		if (key == cocos2d::enumKeyCodes::KEY_D) {
			AchievementNotifier::sharedState()->notifyAchievement("Test Achievement", "aaaaaa", "color_11", false);
		}
		MenuLayer::keyDown(key, timestamp);
	}
};
#endif

class $modify(GJBaseGameLayer) {
    void collisionCheckObjects(PlayerObject* p0, std::vector<GameObject*>* p1, int p2, float p3) {
        GJBaseGameLayer::collisionCheckObjects(p0, p1, p2, p3);

        if (m_isPracticeMode && p0 && p1) {
            for (auto obj : *p1) {
                if (obj && p0->getObjectRect().intersectsRect(obj->getObjectRect())) {
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