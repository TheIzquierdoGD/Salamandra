// Fix steam achievements alert bug in GDPS in Windows and MacOS
#include <Geode/Geode.hpp>

#if defined(GEODE_IS_WINDOWS) || defined(GEODE_IS_MACOS)

#ifdef GEODE_IS_WINDOWS
    #include <Windows.h>
#else
    #include <dlfcn.h>
#endif

using namespace geode::prelude;

void silenceSteam() {
    void* handle = nullptr;
    
    #ifdef GEODE_IS_WINDOWS
        handle = (void*)GetModuleHandleA("steam_api64.dll");
    #else
        handle = dlopen("libsteam_api.dylib", RTLD_LAZY | RTLD_NOLOAD);
    #endif

    if (!handle) return;

    const char* FuctionsBool[] = {
        "SteamAPI_Init",
        "SteamAPI_RestartAppIfNecessary",
        "SteamAPI_IsSteamRunning"
    };

    const char* fuctionsVoid[] = {
        "SteamAPI_RunCallbacks",
        "SteamAPI_Shutdown"
    };

    for (const char* name : FuctionsBool) {
        void* addr = nullptr;

        #ifdef GEODE_IS_WINDOWS
            addr = (void*)GetProcAddress((HMODULE)handle, name);
        #else
            addr = dlsym(handle, name);
        #endif
        
        if (addr) {
            #if defined(GEODE_IS_WINDOWS) || defined(GEODE_IS_INTEL)
                (void)Mod::get()->patch(addr, { 0x31, 0xC0, 0xC3 });
            #endif
        }
    }

    for (const char* name : fuctionsVoid) {
        void* addr = nullptr;

        #ifdef GEODE_IS_WINDOWS
            addr = (void*)GetProcAddress((HMODULE)handle, name);
        #else
            addr = dlsym(handle, name);
        #endif
        
        if (addr) {
            #if defined(GEODE_IS_WINDOWS) || defined(GEODE_IS_INTEL)
                (void)Mod::get()->patch(addr, { 0xC3 }); 
            #endif
        }
    }

    #ifdef GEODE_IS_MACOS
        if (handle) dlclose(handle);
    #endif
}

$on_mod(Loaded) {
    silenceSteam();
}

#endif