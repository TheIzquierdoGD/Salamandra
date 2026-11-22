#include <Geode/Geode.hpp>

#ifdef GEODE_IS_WINDOWS
#include <Geode/modify/GameManager.hpp>
#include <Geode/modify/AchievementManager.hpp>
#include <Geode/modify/MenuLayer.hpp>

#include <Windows.h>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <unordered_set>

using namespace geode::prelude;

namespace {
    constexpr std::array<std::uint8_t, 3> kReturnFalse = {0x31, 0xC0, 0xC3};
    std::unordered_set<void*> s_patched;
    bool s_missingSteamReported = false;
    bool s_noSteamHooksReported = false;
    bool s_gameManagerReported = false;
    bool s_achievementManagerReported = false;

    bool blockFunction(void* address, char const* name) {
        if (!address) return false;
        if (s_patched.contains(address)) return true;

        auto result = Mod::get()->patch(
            address, ByteVector(kReturnFalse.begin(), kReturnFalse.end())
        );
        if (!result) {
            log::warn("SteamFix: no se pudo bloquear {}: {}", name, result.unwrapErr());
            return false;
        }
        s_patched.insert(address);
        log::info("SteamFix: bloqueado {}", name);
        return true;
    }

    struct Layout {
        char const* version;
        std::size_t setStatInt;
        std::size_t setStatFloat;
        std::size_t updateAvgRate;
        std::size_t setAchievement;
        std::size_t storeStats;
        std::size_t achievementProgress;
    };
    constexpr std::array<Layout, 2> kLayouts = {{
        {"STEAMUSERSTATS_INTERFACE_VERSION013", 2, 3, 4, 6, 9, 12},
        {"STEAMUSERSTATS_INTERFACE_VERSION012", 3, 4, 5, 7, 10, 13},
    }};

    bool blockStatsObject(void* object, Layout const& layout) {
        if (!object) return false;
        auto** vtable = *reinterpret_cast<void***>(object);
        if (!vtable) return false;

        bool patched = false;
        patched |= blockFunction(vtable[layout.setAchievement], "ISteamUserStats::SetAchievement");
        patched |= blockFunction(vtable[layout.storeStats], "ISteamUserStats::StoreStats");
        patched |= blockFunction(vtable[layout.achievementProgress], "ISteamUserStats::IndicateAchievementProgress");
        patched |= blockFunction(vtable[layout.setStatInt], "ISteamUserStats::SetStat(int)");
        patched |= blockFunction(vtable[layout.setStatFloat], "ISteamUserStats::SetStat(float)");
        patched |= blockFunction(vtable[layout.updateAvgRate], "ISteamUserStats::UpdateAvgRateStat");
        return patched;
    }

    void blockSteamAchievements() {
        static_assert(sizeof(void*) == 8, "SteamFix requiere Windows x64");
        HMODULE steam = GetModuleHandleW(L"steam_api64.dll");
        if (!steam) {
            if (!s_missingSteamReported) {
                log::warn("SteamFix: steam_api64.dll no cargada; bloqueo de GD activo; reintentaremos");
                s_missingSteamReported = true;
            }
            return;
        }

        bool any = false;
        constexpr std::array<char const*, 6> kFlatExports = {{
            "SteamAPI_ISteamUserStats_SetAchievement",
            "SteamAPI_ISteamUserStats_StoreStats",
            "SteamAPI_ISteamUserStats_IndicateAchievementProgress",
            "SteamAPI_ISteamUserStats_SetStatInt32",
            "SteamAPI_ISteamUserStats_SetStatFloat",
            "SteamAPI_ISteamUserStats_UpdateAvgRateStat",
        }};
        for (auto* name : kFlatExports) {
            any |= blockFunction(reinterpret_cast<void*>(GetProcAddress(steam, name)), name);
        }

        for (auto const& layout : kLayouts) {
            char exportName[64] = {};
            auto const* suffix = layout.version + sizeof("STEAMUSERSTATS_INTERFACE_VERSION") - 1;
            std::snprintf(exportName, sizeof(exportName), "SteamAPI_SteamUserStats_v%s", suffix);
            auto proc = GetProcAddress(steam, exportName);
            if (!proc) continue;
            using Getter = void* (__cdecl*)();
            auto* obj = reinterpret_cast<Getter>(proc)();
            any |= blockStatsObject(obj, layout);
        }

        using GetUser = int (__cdecl*)();
        using FindInterface = void* (__cdecl*)(int, char const*);
        auto getUser = reinterpret_cast<GetUser>(GetProcAddress(steam, "SteamAPI_GetHSteamUser"));
        auto findInterface = reinterpret_cast<FindInterface>(
            GetProcAddress(steam, "SteamInternal_FindOrCreateUserInterface")
        );
        if (getUser && findInterface) {
            int user = getUser();
            if (user > 0) {
                for (auto const& layout : kLayouts) {
                    auto* obj = findInterface(user, layout.version);
                    any |= blockStatsObject(obj, layout);
                }
            }
        }

        if (!any && !s_noSteamHooksReported) {
            log::warn("SteamFix: no se encontro una via de Steamworks para parchear. Revisa el log.");
            s_noSteamHooksReported = true;
        }
    }
}

class $modify(SalamandraSteamFixGameManager, GameManager) {
    bool init() {
        blockSteamAchievements();
        return GameManager::init();
    }

    void reportAchievementWithID(char const* id, int percent, bool dontNotify) {
        if (!s_gameManagerReported) {
            log::warn("SteamFix: GameManager::reportAchievementWithID interceptado (BLOQUEADO)");
            s_gameManagerReported = true;
        }
        (void)id;
        (void)percent;
        (void)dontNotify;
    }
};

class $modify(SalamandraSteamFixAchievementManager, AchievementManager) {
    void reportAchievementWithID(char const* id, int percent, bool dontNotify) {
        if (!s_achievementManagerReported) {
            log::warn("SteamFix: AchievementManager::reportAchievementWithID interceptado (BLOQUEADO)");
            s_achievementManagerReported = true;
        }
        (void)id;
        (void)percent;
        (void)dontNotify;
    }
};

class $modify(SalamandraSteamFixMenu, MenuLayer) {
    bool init() {
        blockSteamAchievements();
        return MenuLayer::init();
    }
};

$on_mod(Loaded) {
    log::info("SteamFix: Salamandra / bloqueo estricto de logros activado");
    blockSteamAchievements();
}

#endif
