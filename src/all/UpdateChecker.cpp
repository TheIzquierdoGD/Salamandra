#include "UpdateChecker.hpp"
#include <Geode/ui/GeodeUI.hpp>
#include <cocos2d.h>
#include <sstream>

using namespace geode::prelude;

#define VERSION_LOCAL "v1.0.0 [Release]"

#define VERSION_CHECK_URL \
    "https://docs.google.com/spreadsheets/d/e/2PACX-1vTyDwd9A1SJUAVe0FKDf_UVKs6dw0FA4N-1yLeROVKZNdRjJI1DTWqK-0m9JT0WM1dk_e9LGRwsAWwZ/pub?output=csv"

#define DISCORD_LINK "https://discord.gg/drs93ZnFNb"

void UpdateChecker::checkForUpdate() {
    // Usa spawn exactamente como hace DeveloperBadges
    spawn(
        web::WebRequest()
            .get(VERSION_CHECK_URL),
        [](web::WebResponse res) {
            if (!res.ok()) return;

            auto csv = res.string().unwrapOr("");
            if (csv.empty()) return;

            std::stringstream ss(csv);
            std::string remoteVersion;
            std::string description;

            // Primera línea = versión
            if (!std::getline(ss, remoteVersion))
                return;

            // Segunda línea = descripción
            if (!std::getline(ss, description))
                description = "";

            if (remoteVersion == VERSION_LOCAL)
                return;

            if (auto scene = cocos2d::CCDirector::sharedDirector()->getRunningScene()) {
                geode::createQuickPopup(
                    "Update Available!",
                    fmt::format(
                        "Current version: <cr>{}</c>\n"
                        "Latest version: <cg>{}</c>\n\n"
                        "<cy>Changes:</c>\n{}\n",
                        VERSION_LOCAL,
                        remoteVersion,
                        description
                    ),
                    "Later",
                    "Discord",
                    [](auto, bool btn2) {
                        if (btn2) {
                            geode::utils::web::openLinkInBrowser(DISCORD_LINK);
                        }
                    }
                );
            }
        }
    );
}