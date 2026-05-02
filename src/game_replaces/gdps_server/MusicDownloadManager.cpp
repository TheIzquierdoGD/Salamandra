#include <Geode/Geode.hpp>
#include <Geode/modify/MusicDownloadManager.hpp>

using namespace geode::prelude;

class $modify(GDPSMDL, MusicDownloadManager) {
    
    std::string pathForSFXFolder(int id) {
        auto path = dirs::getSaveDir() / "Salamandra"; // SFX folder path
        return geode::utils::string::pathToString(path / "");
    }

    std::string pathForSongFolder(int id) {
        auto path = dirs::getSaveDir() / "Salamandra"; // Song folder path
        return geode::utils::string::pathToString(path / "");
    }
};