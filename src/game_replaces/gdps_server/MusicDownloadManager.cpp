#include <Geode/Geode.hpp>
#include <Geode/modify/MusicDownloadManager.hpp>
#include <asp/fs/Path.hpp>
#include <asp/fs/fs.hpp>

#include <string>

using namespace geode::prelude;

namespace {
    std::string salamandraMusicDirectory() {
        asp::fs::Path path = geode::dirs::getSaveDir() / "Salamandra";

        if (!path.exists()) {
            auto result = asp::fs::createDirAll(path);
            if (!result) {
                log::warn("Could not create Salamandra music directory: {}", result.unwrapErr());
            }
        }

        std::string directory = path.string();
        if (!directory.empty() && directory.back() != '/' && directory.back() != '\\') {
            directory += '/';
        }
        return directory;
    }
}

class $modify(GDPSMDL, MusicDownloadManager) {
    std::string pathForSFXFolder(int id) {
        return salamandraMusicDirectory();
    }

    std::string pathForSongFolder(int id) {
        return salamandraMusicDirectory();
    }
};
