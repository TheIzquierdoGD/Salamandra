#include <Geode/Geode.hpp>
#include <Geode/modify/LevelSearchLayer.hpp>
#include <Geode/ui/Popup.hpp>
#include <Geode/ui/Notification.hpp>

using namespace geode::prelude;

static bool g_shownThisSession = false;

class $modify(RecentLevelsWarning, LevelSearchLayer) {
public:
    $override
    bool init(int type) {
        if (!LevelSearchLayer::init(type))
            return false;

        bool permanentlySkipped = Mod::get()->getSavedValue<bool>("skip_recent_warning", false);

        if (!permanentlySkipped && !g_shownThisSession) {
            this->scheduleOnce(
                schedule_selector(RecentLevelsWarning::showEntryWarning),
                0.7f 
            );
        }

        return true;
    }

    void showEntryWarning(float) {
        if (!this->isVisible() || !this->getParent()) return;

        geode::createQuickPopup(
            "Alert! Varied Content :<",
            "Some levels may not be <cg>Reviewed</c> yet.\n\n"
            "You may encounter in <ca>Recent Page </c>and more:\n"
            "<cr>Inappropriate Content</c> or <c4>NSFW.</c>\n\n"
            "Levels marked as <cy>N/A</c> or <co>No Star</c> are not safe.",
            "Continue. I Agree",
            "Don't Show Again",
            [](auto, bool btn2) {
                if (!btn2) {
                    g_shownThisSession = true;
                    
                    Notification::create(
                        "Warning accepted for this session",
                        NotificationIcon::Success
                    )->show();
                } 
                else {
                    Mod::get()->setSavedValue("skip_recent_warning", true);
                    g_shownThisSession = true;
                    
                    Notification::create(
                        "Warning disabled permanently",
                        NotificationIcon::Success
                    )->show();
                }
            }
        );
    }
};