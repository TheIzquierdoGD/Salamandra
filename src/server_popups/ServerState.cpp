#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/utils/web.hpp>

using namespace geode::prelude;

#define INDICATOR_LOCAL "0"
#define MAINTENANCE_URL "https://docs.google.com/spreadsheets/d/e/2PACX-1vQmw1WkABQ9bXY4vXdvBO7yc30rJCgD7CU8mXKeG_UVqlBqIcJRfAe81ogwDur3njxcDnPlByMeltZa/pub?output=csv"
#define DISCORD_LINK "https://discord.gg/drs93ZnFNb"

class MaintenanceChecker {
public:
    static void checkMaintenance(const std::string& data) {
        auto lines = string::split(data, "\n");
        if (lines.size() < 2) return;

        auto values = string::split(lines[1], ",");
        if (values.size() < 5) return;

        auto clean = [](std::string s) {
            s = string::trim(s);
            if (s.size() >= 2 && s.front() == '"' && s.back() == '"')
                s = s.substr(1, s.size() - 2);
            return s;
        };

        std::string indicator = clean(values[0]);

        // If the indicator is not local ("0"), we show the maintenance
        if (indicator != INDICATOR_LOCAL) {
            std::string status    = clean(values[1]);
            std::string type      = clean(values[2]);
            std::string time      = clean(values[3]);
            std::string problem   = clean(values[4]);

            std::string msg = fmt::format(
                "Current status: <cr>{}</c>\n"
                "Maintenance type: <cg>{}</c>\n"
                "Problem main: <c4>{}</c>\n"
                "Estimated time: <cy>{}</c>\n\n"
                "<co>Check details in the official </c><cd>Discord server!</c>",
                status, type, problem, time
            );

            geode::createQuickPopup(
                "Server Maintenance",
                msg,
                "Later", "Discord", 
                [](auto, bool btn2) {
                    if (btn2) web::openLinkInBrowser(DISCORD_LINK);
                }
            );
        }
    }

    static void requestMaintenance() {
        // Anti-cache to ensure it reads the change from the Excel instantly
        auto now = std::chrono::system_clock::now().time_since_epoch().count();
        std::string url = fmt::format("{}&t={}", MAINTENANCE_URL, now);

        spawn(
            web::WebRequest().get(url),
            [](web::WebResponse res) {
                if (res.ok()) {
                    auto data = res.string().unwrapOr("");
                    if (!data.empty()) MaintenanceChecker::checkMaintenance(data);
                }
            }
        );
    }
};

// We use a UNIQUE class name (SalamandraMaint) so that Geode doesn't get confused
class $modify(SalamandraMaint, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;
        
        // Llamamos al request igual que en tu VersionChecker
        MaintenanceChecker::requestMaintenance();

        return true;
    }
};