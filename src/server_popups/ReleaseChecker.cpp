// Salamandra Release Checker - Geode v6 / Geometry Dash 2.2091
#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/utils/web.hpp>
#include <Geode/utils/file.hpp>
#include <Geode/utils/async.hpp>
#include <Geode/ui/Popup.hpp>
#include <Geode/binding/FLAlertLayer.hpp>
#include <ctime>
#include <string>
#include <vector>

using namespace geode::prelude;

namespace {
    constexpr auto VERSION_LOCAL = "v2.1.2 [Release]";
    // Geode v6.0.0-alpha.1 targets GD 2.2091. Keep synchronized with mod.json.
    constexpr auto CLIENT_LOCAL = "2.2091";
    constexpr auto VERSION_CHECK_URL = "https://docs.google.com/spreadsheets/d/e/2PACX-1vTyDwd9A1SJUAVe0FKDf_UVKs6dw0FA4N-1yLeROVKZNdRjJI1DTWqK-0m9JT0WM1dk_e9LGRwsAWwZ/pub?output=csv";
    constexpr auto DOWNLOAD_URL = "https://github.com/TheIzquierdoGD/Salamandra/releases/latest/download/theizquierdogd.salamandra.geode";
    bool g_checkStarted = false;
    bool g_downloadStarted = false;

    // CSV reader for one row; handles quoted values and escaped quotes.
    std::vector<std::string> csvRow(std::string_view line) {
        std::vector<std::string> values;
        std::string current;
        bool quoted = false;
        for (size_t i = 0; i < line.size(); ++i) {
            char c = line[i];
            if (c == '"') {
                if (quoted && i + 1 < line.size() && line[i + 1] == '"') {
                    current += '"';
                    ++i;
                } else quoted = !quoted;
            } else if (c == ',' && !quoted) {
                values.push_back(string::trim(current));
                current.clear();
            } else if (c != '\r' || quoted) current += c;
        }
        values.push_back(string::trim(current));
        return values;
    }
}

class SalamandraVersionChecker {
public:
    static void downloadAndInstall(std::string const& remoteVersion) {
        if (g_downloadStarted) return;
        g_downloadStarted = true;

        async::spawn(web::WebRequest().get(DOWNLOAD_URL),
            [remoteVersion](web::WebResponse res) {
                if (!res.ok()) {
                    log::warn("Salamandra update download failed (HTTP {})", res.code());
                    g_downloadStarted = false;
                    return;
                }
                auto const& bytes = res.data();
                // .geode archives are ZIP files; reject empty/HTML error responses.
                if (bytes.size() < 4 || bytes.size() > 100 * 1024 * 1024 ||
                    bytes[0] != 'P' || bytes[1] != 'K' ||
                    bytes[2] != 3 || bytes[3] != 4) {
                    log::warn("Invalid Salamandra update package (not a ZIP file)");
                    g_downloadStarted = false;
                    return;
                }

                auto outputFile = dirs::getModsDir() / "theizquierdogd.salamandra.geode";
                // In V6, file utils accept asp::fs::Path and return a Result.
                auto writeResult = file::writeBinarySafe(outputFile, bytes);
                if (!writeResult) {
                    log::warn("Could not replace the running mod package; install the update manually");
                    FLAlertLayer::create("Update Ready",
                        "Close the game and install the newest Salamandra .geode from GitHub.",
                        "OK")->show();
                    g_downloadStarted = false;
                    return;
                }

                createQuickPopup("Server Updated!",
                    fmt::format("Salamandra update downloaded!\n<cr>{}</c> -> <cg>{}</c>\n\nRestart to apply it.",
                                VERSION_LOCAL, remoteVersion),
                    "Exit", "Restart",
                    [](auto, bool restart) {
                        if (restart) utils::game::restart(true, false);
                        else utils::game::exit(true);
                    });
            });
    }

    static void checkVersion(std::string const& data) {
        auto lines = string::split(data, "\n");
        if (lines.size() < 2) return;
        auto values = csvRow(lines[1]);
        if (values.size() < 2) return;

        std::string remoteVersion = values[0];
        std::string remoteClient = values[1];
        if (remoteVersion.empty() || remoteClient.empty()) return;

        if (remoteClient != CLIENT_LOCAL) {
            FLAlertLayer::create("Incompatible Client",
                fmt::format("Current client: <cy>{}</c>\nRequired by server: <cg>{}</c>\n"
                            "Verify your GD version and update mod.json if needed.",
                            CLIENT_LOCAL, remoteClient), "OK")->show();
            return;
        }
        if (remoteVersion != VERSION_LOCAL) downloadAndInstall(remoteVersion);
    }

    static void requestVersion() {
        if (g_checkStarted) return;
        g_checkStarted = true;
        auto url = fmt::format("{}&t={}", VERSION_CHECK_URL, std::time(nullptr));
        async::spawn(web::WebRequest().get(url), [](web::WebResponse res) {
            if (!res.ok()) {
                log::debug("Salamandra version check unavailable (HTTP {})", res.code());
                return;
            }
            if (auto data = res.string(); data) checkVersion(data.unwrap());
        });
    }
};

class $modify(SalamandraReleaseCheckerHook, MenuLayer) {
    $override bool init() {
        if (!MenuLayer::init()) return false;
        SalamandraVersionChecker::requestVersion();
        return true;
    }
};
