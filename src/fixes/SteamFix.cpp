// Salamandra - SteamFix (Geometry Dash 2.2091 / Windows x64)
//
// Proteccion ESTRICTA: impide que los logros originados en Salamandra
// lleguen al administrador de logros de Geometry Dash y a Steam.
//
// IMPORTANTE: para evitar enviar logros por accidente, esta version
// desactiva tambien la concesion/notificacion de logros internos de GD
// mientras Salamandra este activo. Las estadisticas normales del juego
// (estrellas, monedas, etc.) no se interceptan aqui.
//
// No modifica Steam en disco ni otras instalaciones de Geometry Dash.
// No elimina logros obtenidos previamente.

#include <Geode/Geode.hpp>

#ifdef GEODE_IS_WINDOWS
    #include <Geode/modify/AchievementManager.hpp>
    #include <Geode/modify/MenuLayer.hpp>
    #include <Windows.h>
    #include <array>
    #include <cstdint>
    #include <unordered_set>
    #include <utility>

using namespace geode::prelude;

namespace {
    // Windows x64: xor eax, eax; ret  (false para funciones bool).
    constexpr std::array<std::uint8_t, 3> RETURN_FALSE = { 0x31, 0xC0, 0xC3 };
    std::unordered_set<void*> s_patched;
    bool s_dllWarningShown = false;
    bool s_hookWarningShown = false;
    bool s_apiWarningShown = false;

    void patchSteamApi() {
        static_assert(sizeof(void*) == 8, "SteamFix requiere Windows x64");

        HMODULE steam = GetModuleHandleW(L"steam_api64.dll");
        if (!steam) {
            if (!s_dllWarningShown) {
                log::warn("SteamFix: steam_api64.dll aun no esta cargada; bloqueo interno activo");
                s_dllWarningShown = true;
            }
            return;
        }

        // Barrera secundaria: evita llamadas directas a los exports planos
        // de Steamworks. El bloqueo principal esta en AchievementManager.
        constexpr std::array<char const*, 6> exports = {{
            "SteamAPI_ISteamUserStats_SetAchievement",
            "SteamAPI_ISteamUserStats_StoreStats",
            "SteamAPI_ISteamUserStats_IndicateAchievementProgress",
            "SteamAPI_ISteamUserStats_SetStatInt32",
            "SteamAPI_ISteamUserStats_SetStatFloat",
            "SteamAPI_ISteamUserStats_UpdateAvgRateStat",
        }};

        int found = 0;
        for (auto* name : exports) {
            auto* address = reinterpret_cast<void*>(GetProcAddress(steam, name));
            if (!address) continue;
            ++found;
            if (s_patched.contains(address)) continue;
            auto result = Mod::get()->patch(address, ByteVector(RETURN_FALSE.begin(), RETURN_FALSE.end()));
            if (result) {
                s_patched.insert(address);
                log::info("SteamFix: API de Steam bloqueada: {}", name);
            } else {
                log::warn("SteamFix: fallo al bloquear {}: {}", name, result.unwrapErr());
            }
        }
        if (found == 0 && !s_apiWarningShown) {
            log::warn("SteamFix: sin exports planos detectados; se mantiene el bloqueo interno estricto");
            s_apiWarningShown = true;
        }
    }
}

// BLOQUEO PRINCIPAL: nunca permite que GD publique un logro de Salamandra.
// No llamamos al metodo original a proposito: en algunas versiones de GD,
// este metodo comunica el logro a Steam mediante rutas distintas a los
// exports de steam_api64.dll (incluidas llamadas virtuales).
class $modify(SalamandraSteamAchievementGuard, AchievementManager) {
    void reportAchievementWithID(char const* id, int percent, bool dontNotify) {
        patchSteamApi();
        if (!s_hookWarningShown) {
            log::info("SteamFix: AchievementManager::reportAchievementWithID bloqueado (modo estricto)");
            s_hookWarningShown = true;
        }
        // No pasar a AchievementManager::reportAchievementWithID().
        // El progreso normal de GameStatsManager no se modifica aqui.
        (void)id;
        (void)percent;
        (void)dontNotify;
    }
};

// Segundo intento al abrir el menu, por si Steam no estaba cargado antes.
class $modify(SalamandraSteamFixMenu, MenuLayer) {
    bool init() {
        patchSteamApi();
        return MenuLayer::init();
    }
};

$on_mod(Loaded) {
    patchSteamApi();
    log::info("SteamFix: modo estricto configurado para Salamandra");
}

#endif  // GEODE_IS_WINDOWS
