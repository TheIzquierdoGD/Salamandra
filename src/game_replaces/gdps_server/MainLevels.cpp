#include <Geode/Geode.hpp>
#include <Geode/modify/LevelTools.hpp>
#include <Geode/modify/LevelSelectLayer.hpp>
#include <Geode/binding/GameLevelManager.hpp>
#include <Geode/binding/LocalLevelManager.hpp>
#include <Geode/binding/GameManager.hpp>

#include <string>
#include <vector>

using namespace geode::prelude;

class $modify(TRLevelTools, LevelTools) {
    static bool verifyLevelIntegrity(std::string verifyString, int levelID) {
        return true;
    }

    $override static std::string getAudioTitle(int levelID) {
        switch (levelID) {
            case 601: return "Faded";
            case 602: return "Nevada";
            case 603: return "Sky High";
            case 604: return "Link";
            case 605: return "Alone";
            default: return LevelTools::getAudioTitle(levelID);
        }
    }

    $override static std::string getAudioFileName(int levelID) {
        switch (levelID) {
            case 601: return "Faded.ogg"_spr;
            case 602: return "Nevada.ogg"_spr;
            case 603: return "Sky High.ogg"_spr;
            case 604: return "Link.ogg"_spr;
            case 605: return "Alone.ogg"_spr;
            default: return LevelTools::getAudioFileName(levelID);
        }
    }

    $override static GJGameLevel* getLevel(int levelID, bool loaded) {
        if (levelID < 601 || levelID > 605) {
            return LevelTools::getLevel(levelID, loaded);
        }

        GJGameLevel* level = GJGameLevel::create();

        switch (levelID) {
            case 601:
                level->m_levelName = getAudioTitle(601);
                level->m_audioTrack = 601;
                level->m_stars = 2;
                level->m_difficulty = GJDifficulty::Easy;
                level->m_coins = 3;
                level->m_levelLength = 3;
                level->m_requiredCoins = 0;
                break;
            case 602:
                level->m_levelName = getAudioTitle(602);
                level->m_audioTrack = 602;
                level->m_stars = 3;
                level->m_difficulty = GJDifficulty::Normal;
                level->m_coins = 3;
                level->m_levelLength = 3;
                level->m_requiredCoins = 0;
                break;
            case 603:
                level->m_levelName = getAudioTitle(603);
                level->m_audioTrack = 603;
                level->m_stars = 5;
                level->m_difficulty = GJDifficulty::Hard;
                level->m_coins = 3;
                level->m_levelLength = 3;
                level->m_requiredCoins = 0;
                break;
            case 604:
                level->m_levelName = getAudioTitle(604);
                level->m_audioTrack = 604;
                level->m_stars = 7;
                level->m_difficulty = GJDifficulty::Harder;
                level->m_coins = 3;
                level->m_levelLength = 3;
                level->m_requiredCoins = 0;
                break;
            case 605:
                level->m_levelName = getAudioTitle(605);
                level->m_audioTrack = 605;
                level->m_stars = 8;
                level->m_difficulty = GJDifficulty::Insane;
                level->m_demon = 3;
                level->m_coins = 3;
                level->m_levelLength = 3;
                level->m_requiredCoins = 0;
                break;
        }

        if (!loaded) {
            level->m_levelString = LocalLevelManager::sharedState()->getMainLevelString(levelID);
        }

        level->m_levelID = levelID;
        level->m_levelType = GJLevelType::Main;
        return level;
    }
};

class $modify(TRLevelSelectLayer, LevelSelectLayer) {
    bool init(int pageID) {
        if (!LevelSelectLayer::init(pageID)) {
            return false;
        }

        int currentPage = m_scrollLayer->m_page;
        m_scrollLayer->m_dynamicObjects->removeAllObjects();

        const std::vector<int> levels = { 601, 602, 603, 604, 605 };
        for (int id : levels) {
            auto level = GameLevelManager::get()->getMainLevel(id, true);
            if (level) {
                m_scrollLayer->m_dynamicObjects->addObject(level);
            }
        }

        auto dotsArray = CCArrayExt<CCSprite*>(m_scrollLayer->m_dots);
        for (auto dot : dotsArray) {
            dot->removeFromParent();
        }
        m_scrollLayer->m_dots->removeAllObjects();

        int levelCount = m_scrollLayer->m_dynamicObjects->count();
        auto batchNode = CCSpriteBatchNode::create("smallDot.png", levelCount);
        m_scrollLayer->addChild(batchNode, 5);

        for (int i = 0; i < levelCount; i++) {
            auto dot = CCSprite::create("smallDot.png");
            batchNode->addChild(dot);
            m_scrollLayer->m_dots->addObject(dot);
        }

        m_scrollLayer->updateDots(0.f);
        m_scrollLayer->updatePages();
        m_scrollLayer->m_page = currentPage;
        m_scrollLayer->moveToPage(currentPage);
        m_scrollLayer->repositionPagesLooped();
        return true;
    }

    ccColor3B colorForPage(int pageID) {
        auto GM = GameManager::sharedState();
        const int colors[5] = { 5, 7, 8, 10, 11 };
        const int index = (pageID % 5 + 5) % 5;
        return GM->colorForIdx(colors[index]);
    }
};
