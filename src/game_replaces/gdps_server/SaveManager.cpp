// Local save redirection to Salamandra's directory
#include "SaveManager.hpp"

void GSGManager::setup() {
    if (!m_fields->m_originalFileName.empty()) return GManager::setup();
    
    m_fields->m_originalFileName = m_fileName;

    // Save directory
    auto dir = geode::dirs::getSaveDir() / "Salamandra"; // Save directory path

    std::error_code err;
    if (!std::filesystem::exists(dir, err)) {
        if (!std::filesystem::create_directories(dir, err)) {
            return GManager::setup();
        }
    }

    m_fileName = fmt::format("Salamandra/{}", m_fields->m_originalFileName);
    
    GManager::setup();
}