#include <Geode/Geode.hpp>
#include <Geode/modify/LevelSearchLayer.hpp>

using namespace geode::prelude;

static bool g_shownThisSession = false;

class $modify(RecentLevelsWarning, LevelSearchLayer) {
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
        if (!this->isVisible()) return;

        g_shownThisSession = true;

        // INTERCAMBIAMOS LOS BOTONES:
        // El botón 1 (izquierdo) ahora es "Continue" (se activa con ESC)
        // El botón 2 (derecho) ahora es "Don't Show Again"
        geode::createQuickPopup(
            "Alert! Varied Content :<",
            "Some levels may not be <cg>Reviewed</c> yet.\n\n"
            "You may encounter in <ca>Recent Page </c>and more:\n"
            "<cr>Inappropriate Content</c> or <c4>NSFW.</c>\n\n"
            "Levels marked as <cy>N/A</c> or <co>No Star</c> are not safe.",
            "Continue. I Agreed",   // Botón Izquierdo (btn2 = false) -> AHORA ESTE RESPONDE AL ESC
            "Don't Show Again",     // Botón Derecho (btn2 = true)
            [](auto, bool btn2) {
                
                // Si btn2 es true, significa que presionó el botón de la derecha
                if (btn2) {
                    Mod::get()->setSavedValue("skip_recent_warning", true);
                    
                    Notification::create(
                        "Warning disabled",
                        NotificationIcon::Success
                    )->show();
                }
                
                // Si presionó el izquierdo (btn2 = false) o ESC, simplemente se cierra.
            }
        );
    }
};