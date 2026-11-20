// Salamandra / GDPS - Steam achievements & stats guard (Windows x64, Geode v6).
// This file intentionally does NOT modify SteamAPI_Init/Shutdown/RunCallbacks:
// disabling them after game startup cannot undo an initialized Steam session.
//
// Blocks the functions used to grant Steam achievements and write Steam stats.
// Supports both Steamworks flat (C) exports and C++ ISteamUserStats vtables
// for the SDK's documented v012 and v013 interfaces.
//
// IMPORTANT: This is process-local, does not erase existing Steam achievements,
// and does not disable Steam client playtime tracking / Steam's game detection.

#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>

#ifdef GEODE_IS_WINDOWS
    #include <Windows.h>
    #include <array>
    #include <unordered_set>
    #include <utility>

using namespace geode::prelude;

namespace {
    // 64-bit Windows: xor eax, eax; ret -> bool false.
    // Do not use a bare 'ret' for functions returning bool.
    constexpr std::array<unsigned char, 3> kReturnFalse = {0x31, 0xC0, 0xC3};

    std::unordered_set<void*> s_patchedFunctions;
    bool s_missingDllLogged = false;
    bool s_successLogged = false;
    bool s_partialLogged = false;
    bool s_noFunctionsLogged = false;

    bool blockFunction(void* address, const char* label) {
        if (!address) return false;
        if (s_patchedFunctions.contains(address)) return true;

        ByteVector bytes(kReturnFalse.begin(), kReturnFalse.end());
        auto result = Mod::get()->patch(address, std::move(bytes));
        if (!result) {
            log::warn("SteamFix: no se pudo bloquear {}: {}", label, result.unwrapErr());
            return false;
        }

        s_patchedFunctions.insert(address);
        log::info("SteamFix: bloqueado {}", label);
        return true;
    }

    // Steamworks interface layout is versioned. Never guess an index across
    // versions: v012 has RequestCurrentStats at slot 0; v013 removes it.
    struct StatsInterface {
        const char* accessor;
        std::size_t setStatInt;
        std::size_t setStatFloat;
        std::size_t updateAvgRate;
        std::size_t setAchievement;
        std::size_t storeStats;
        std::size_t progress;
    };

    constexpr std::array<StatsInterface, 2> kStatsInterfaces = {{
        {"SteamAPI_SteamUserStats_v012", 3, 4, 5, 7, 10, 13},
        {"SteamAPI_SteamUserStats_v013", 2, 3, 4, 6, 9, 12},
    }};

    void blockSteamStats() {
        // Target GD 2.2091 on Windows x64; leave other architectures untouched.
        static_assert(sizeof(void*) == 8, "SteamFix requires 64-bit Windows");

        auto module = GetModuleHandleA("steam_api64.dll");
        if (!module) {
            if (!s_missingDllLogged) {
                log::warn("SteamFix: steam_api64.dll no esta cargada; reintentaremos al abrir el menu");
                s_missingDllLogged = true;
            }
            return;
        }

        // Flat API entry points (used by some Steamworks clients/wrappers).
        constexpr std::array<const char*, 6> kFlatExports = {{
            "SteamAPI_ISteamUserStats_SetAchievement",
            "SteamAPI_ISteamUserStats_StoreStats",
            "SteamAPI_ISteamUserStats_IndicateAchievementProgress",
            "SteamAPI_ISteamUserStats_SetStatInt32",
            "SteamAPI_ISteamUserStats_SetStatFloat",
            "SteamAPI_ISteamUserStats_UpdateAvgRateStat",
        }};
        bool flatInstalled = false;
        for (const char* name : kFlatExports) {
            auto entry = reinterpret_cast<void*>(GetProcAddress(module, name));
            flatInstalled |= blockFunction(entry, name);
        }

        // C++ SteamUserStats()->SetAchievement() calls the interface vtable,
        // NOT necessarily the flat SteamAPI_ISteamUserStats_SetAchievement export.
        bool virtualInstalled = false;
        using StatsGetter = void* (*)();
        for (auto const& layout : kStatsInterfaces) {
            auto proc = GetProcAddress(module, layout.accessor);
            if (!proc) continue;
            auto getter = reinterpret_cast<StatsGetter>(proc);
            void* stats = getter();
            if (!stats) continue;
            void** vtable = *reinterpret_cast<void***>(stats);
            if (!vtable) continue;

            virtualInstalled |= blockFunction(vtable[layout.setAchievement], "ISteamUserStats::SetAchievement");
            virtualInstalled |= blockFunction(vtable[layout.storeStats], "ISteamUserStats::StoreStats");
            virtualInstalled |= blockFunction(vtable[layout.progress], "ISteamUserStats::IndicateAchievementProgress");
            virtualInstalled |= blockFunction(vtable[layout.setStatInt], "ISteamUserStats::SetStat(int)");
            virtualInstalled |= blockFunction(vtable[layout.setStatFloat], "ISteamUserStats::SetStat(float)");
            virtualInstalled |= blockFunction(vtable[layout.updateAvgRate], "ISteamUserStats::UpdateAvgRateStat");
        }

        if (virtualInstalled && !s_successLogged) {
            log::info("SteamFix: bloqueo de ISteamUserStats virtual instalado");
            s_successLogged = true;
        }
        else if (flatInstalled && !virtualInstalled && !s_partialLogged) {
            log::warn("SteamFix: solo se bloquearon funciones planas de Steam; la via C++ no esta confirmada");
            s_partialLogged = true;
        }
        else if (!flatInstalled && !virtualInstalled && !s_noFunctionsLogged) {
            log::warn("SteamFix: no se encontraron funciones de Steam para bloquear");
            s_noFunctionsLogged = true;
        }
    }
}

// First attempt as soon as Geode loads the mod.
$on_mod(Loaded) {
    blockSteamStats();
}

// Retry before menu setup, if Steam was not yet loaded during $on_mod(Loaded).
class $modify(SalamandraSteamFixMenuLayer, MenuLayer) {
    bool init() {
        blockSteamStats();
        return MenuLayer::init();
    }
};

#endif // GEODE_IS_WINDOWS
