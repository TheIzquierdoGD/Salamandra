// Geode V6 - Redirect local saves to the Salamandra directory.
#include "SaveManager.hpp"
#include <asp/fs/Path.hpp>
#include <asp/fs/fs.hpp>

void GSGManager::setup() {
    // Avoid adding the prefix again if setup() is called more than once.
    if (!m_fields->m_originalFileName.empty()) {
        return GManager::setup();
    }

    asp::fs::Path dir = geode::dirs::getSaveDir() / "Salamandra";
    if (!dir.exists()) {
        auto result = asp::fs::createDirAll(dir);
        if (!result) {
            geode::log::warn(
                "Could not create Salamandra save directory: {}",
                result.unwrapErr()
            );
            // Match the original fallback: use the default game save path.
            return GManager::setup();
        }
    }

    m_fields->m_originalFileName = m_fileName;
    m_fileName = fmt::format("Salamandra/{}", m_fields->m_originalFileName);
    GManager::setup();
}
