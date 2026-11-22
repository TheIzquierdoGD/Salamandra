#include <Geode/Geode.hpp>

#if defined(GEODE_IS_WINDOWS) || defined(GEODE_IS_MACOS)
#include <Geode/modify/GameManager.hpp>
#include <Geode/modify/AchievementManager.hpp>
#include <Geode/modify/MenuLayer.hpp>

#if defined(GEODE_IS_WINDOWS)
#include <Windows.h>
#else
#include <Geode/modify/PlatformToolbox.hpp>
#include <dlfcn.h>
#include <mach-o/dyld.h>
#include <string_view>
#endif

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <unordered_set>

using namespace geode::prelude;

namespace {
#if defined(__aarch64__) || defined(__arm64__)
    constexpr std::array<std::uint8_t, 8> kReturnFalse = {
        0x00, 0x00, 0x80, 0x52, 0xC0, 0x03, 0x5F, 0xD6
    };
#elif defined(__x86_64__) || defined(_M_X64)
    constexpr std::array<std::uint8_t, 3> kReturnFalse = {0x31, 0xC0, 0xC3};
#else
#error Unsupported desktop architecture for Salamandra SteamFix
#endif
    static_assert(sizeof(void*) == 8);

    std::unordered_set<void*> s_patched;
    bool s_missingSteamReported = false;
    bool s_noSteamHooksReported = false;
    bool s_gameManagerReported = false;
    bool s_achievementManagerReported = false;

    struct SteamLibrary {
#if defined(GEODE_IS_WINDOWS)
        HMODULE handle = GetModuleHandleW(L"steam_api64.dll");
        void* symbol(char const* name) const {
            return handle ? reinterpret_cast<void*>(GetProcAddress(handle, name)) : nullptr;
        }
#else
        void* handle = nullptr;
        SteamLibrary() {
            auto count = _dyld_image_count();
            for (std::uint32_t i = 0; i < count; ++i) {
                auto* path = _dyld_get_image_name(i);
                if (!path) continue;
                std::string_view name(path);
                auto slash = name.find_last_of('/');
                if (slash != std::string_view::npos) name.remove_prefix(slash + 1);
                if (name != "libsteam_api.dylib" && name != "libsteam_api64.dylib") continue;
                handle = dlopen(path, RTLD_LAZY | RTLD_NOLOAD);
                if (handle) break;
            }
            if (!handle) handle = dlopen("libsteam_api.dylib", RTLD_LAZY | RTLD_NOLOAD);
        }
        ~SteamLibrary() { if (handle) dlclose(handle); }
        void* symbol(char const* name) const {
            return handle ? dlsym(handle, name) : nullptr;
        }
#endif
    };

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
        SteamLibrary steam;
        if (!steam.handle) {
            if (!s_missingSteamReported) {
                log::warn("SteamFix: biblioteca Steam no cargada; bloqueo de GD activo; reintentaremos");
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
            any |= blockFunction(steam.symbol(name), name);
        }

        for (auto const& layout : kLayouts) {
            char exportName[64] = {};
            auto const* suffix = layout.version + sizeof("STEAMUSERSTATS_INTERFACE_VERSION") - 1;
            std::snprintf(exportName, sizeof(exportName), "SteamAPI_SteamUserStats_v%s", suffix);
            auto proc = steam.symbol(exportName);
            if (!proc) continue;
            using Getter = void* (*)();
            auto* obj = reinterpret_cast<Getter>(proc)();
            any |= blockStatsObject(obj, layout);
        }

        using GetUser = int (*)();
        using FindInterface = void* (*)(int, char const*);
        auto getUser = reinterpret_cast<GetUser>(steam.symbol("SteamAPI_GetHSteamUser"));
        auto findInterface = reinterpret_cast<FindInterface>(
            steam.symbol("SteamInternal_FindOrCreateUserInterface")
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

#ifdef GEODE_IS_MACOS
    void reportPlatformAchievementWithID(char const*, int) {
        static bool reported = false;
        if (!reported) {
            log::warn("SteamFix: AchievementManager::reportPlatformAchievementWithID interceptado (BLOQUEADO)");
            reported = true;
        }
    }
#endif
};

#ifdef GEODE_IS_MACOS
class $modify(SalamandraSteamFixPlatformToolbox, PlatformToolbox) {
    static void reportAchievementWithID(char const*, int) {
        static bool reported = false;
        if (!reported) {
            log::warn("SteamFix: PlatformToolbox::reportAchievementWithID interceptado (BLOQUEADO)");
            reported = true;
        }
    }
};
#endif

class $modify(SalamandraSteamFixMenu, MenuLayer) {
    bool init() {
        blockSteamAchievements();
        auto result = MenuLayer::init();
        if (result) blockSteamAchievements();
        return result;
    }
};

$on_mod(Loaded) {
#if defined(GEODE_IS_WINDOWS)
    log::info("SteamFix: Salamandra / bloqueo estricto de logros activado (Windows x64)");
#elif defined(__aarch64__) || defined(__arm64__)
    log::info("SteamFix: Salamandra / bloqueo estricto de logros activado (macOS ARM64)");
#else
    log::info("SteamFix: Salamandra / bloqueo estricto de logros activado (macOS Intel)");
#endif
    blockSteamAchievements();
}

#endif
