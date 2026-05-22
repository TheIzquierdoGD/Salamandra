#include <Geode/Geode.hpp>
#include <Geode/modify/LevelSearchLayer.hpp>

using namespace geode::prelude;

// Track if the user has accepted the warning for the current session
static bool g_shownThisSession = false;

class $modify(RecentLevelsWarning, LevelSearchLayer) {
    bool init(int type) {
        if (!LevelSearchLayer::init(type))
            return false;

        // Check if the user opted to never see the warning again
        bool permanentlySkipped = Mod::get()->getSavedValue<bool>("skip_recent_warning", false);

        // If not permanently skipped and not accepted this session, show it
        if (!permanentlySkipped && !g_shownThisSession) {
            this->scheduleOnce(
                schedule_selector(RecentLevelsWarning::showEntryWarning),
                0.7f 
            );
        }

        return true;
    }

    void showEntryWarning(float) {
        // Safety check to ensure the layer is still active
        if (!this->isVisible()) return;

        geode::createQuickPopup(
            "Alert! Varied Content :<",
            "Some levels may not be <cg>Reviewed</c> yet.\n\n"
            "You may encounter in <ca>Recent Page </c>and more:\n"
            "<cr>Inappropriate Content</c> or <c4>NSFW.</c>\n\n"
            "Levels marked as <cy>N/A</c> or <co>No Star</c> are not safe.",
            "Continue. I Agree",
            "Don't Show Again",
            [this](auto, bool btn2) {
                if (!btn2) {
                    // Option 1: Continue. I Agree (Accept for this session)
                    g_shownThisSession = true;
                    
                    Notification::create(
                        "Warning accepted for this session",
                        NotificationIcon::Success
                    )->show();
                } 
                else {
                    // Option 2: Don't Show Again (Permanent skip)
                    Mod::get()->setSavedValue("skip_recent_warning", true);
                    
                    Notification::create(
                        "Warning disabled permanently",
                        NotificationIcon::Success
                    )->show();
                }
            }
        );
    }
};