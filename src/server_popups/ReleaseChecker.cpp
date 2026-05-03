#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>
#include <Geode/utils/file.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <filesystem>
#include <ctime>

using namespace geode::prelude;

#define VERSION_LOCAL "v2.1.1 [Release]"
#define VERSION_CHECK_URL "https://docs.google.com/spreadsheets/d/e/2PACX-1vTyDwd9A1SJUAVe0FKDf_UVKs6dw0FA4N-1yLeROVKZNdRjJI1DTWqK-0m9JT0WM1dk_e9LGRwsAWwZ/pub?output=csv"
#define DOWNLOAD_URL "https://github.com/TheIzquierdoGD/Salamandra/releases/latest/download/theizquierdogd.salamandra.geode"

class VersionChecker {
public:
    static void downloadAndInstall(const std::string& remoteVersion) {
        // Get the path where the current mod is located to overwrite it
        std::filesystem::path modsPath = Mod::get()->getMetadata().getPath().parent_path();
        std::filesystem::path outputFile = modsPath / "theizquierdogd.salamandra.geode";

        spawn(
            web::WebRequest().get(DOWNLOAD_URL),
            [outputFile, remoteVersion](web::WebResponse res) {
                if (!res.ok()) return;

                auto bytes = res.data();
                if (bytes.empty()) return;

                // Attempt to save the downloaded binary file
                if (file::writeBinary(outputFile, bytes)) {
                    std::string msg = fmt::format(
                        "A new update has been installed in the background!\n"
                        "<cr>{}</c> -> <cg>{}</c>\n\n"
                        "Select an option to apply changes:",
                        VERSION_LOCAL, remoteVersion
                    );

                    // Popup with final update options
                    geode::createQuickPopup(
                        "Server Updated!", 
                        msg, 
                        "Restart", "Exit", 
                        [](auto, bool btn2) {
                            if (btn2) {
                                // Right Button: Restart the game
                                utils::game::restart(true);
                            } else {
                                // Left Button: Close the game
                                utils::game::exit(true);
                            }
                        }
                    );
                }
            }
        );
    }

    static void checkVersion(const std::string& data) {
        auto lines = string::split(data, "\n");
        if (lines.size() < 2) return;

        auto values = string::split(lines[1], ",");
        if (values.size() < 2) return;

        auto clean = [](std::string s) {
            s = string::trim(s);
            if (s.size() >= 2 && s.front() == '"' && s.back() == '"')
                s = s.substr(1, s.size() - 2);
            return s;
        };

        std::string remoteVersion = clean(values[0]);

        // If the server version differs from the local version, trigger download
        if (remoteVersion != VERSION_LOCAL) {
            VersionChecker::downloadAndInstall(remoteVersion);
        }
    }

    static void requestVersion() {
        // Add a timestamp to the URL to bypass web cache
        std::string antiCacheUrl = std::string(VERSION_CHECK_URL) + "&t=" + std::to_string(std::time(nullptr));

        spawn(
            web::WebRequest().get(antiCacheUrl),
            [](web::WebResponse res) {
                if (res.ok()) {
                    auto data = res.string().unwrapOr("");
                    if (!data.empty()) VersionChecker::checkVersion(data);
                }
            }
        );
    }
};

class $modify(MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;
        
        // Execute version check when the main menu finishes loading
        VersionChecker::requestVersion();

        return true;
    }
};