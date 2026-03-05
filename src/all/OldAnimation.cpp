#include <Geode/Geode.hpp>
#include <Geode/modify/AchievementBar.hpp>

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
#include <Geode/modify/MenuLayer.hpp>

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