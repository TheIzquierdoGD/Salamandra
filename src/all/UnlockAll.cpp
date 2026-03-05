#include <Geode/Geode.hpp>
#include <Geode/modify/GameManager.hpp>
#include <Geode/modify/GameStatsManager.hpp>

using namespace geode::prelude;

bool g_iconBypassEnabled = true; // Puedes controlar esto con un botón si quieres

class $modify(GameManager) {
    bool isIconUnlocked(int id, IconType type) {
        if (g_iconBypassEnabled)
            return true;
        return GameManager::isIconUnlocked(id, type);
    }

    bool isColorUnlocked(int id, UnlockType type) {
        if (g_iconBypassEnabled)
            return true;
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