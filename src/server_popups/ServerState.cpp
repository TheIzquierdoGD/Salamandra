// Salamandra server maintenance notices - Geode v6
#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/utils/web.hpp>
#include <Geode/utils/async.hpp>
#include <Geode/ui/Popup.hpp>
#include <chrono>
#include <string>
#include <vector>

using namespace geode::prelude;

namespace {
    constexpr auto INDICATOR_LOCAL = "0";
    constexpr auto MAINTENANCE_URL = "https://docs.google.com/spreadsheets/d/e/2PACX-1vQmw1WkABQ9bXY4vXdvBO7yc30rJCgD7CU8mXKeG_UVqlBqIcJRfAe81ogwDur3njxcDnPlByMeltZa/pub?output=csv";
    constexpr auto DISCORD_LINK = "https://discord.gg/drs93ZnFNb";
    bool g_maintenanceRequested = false;

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

class SalamandraMaintenanceChecker {
public:
    static void checkMaintenance(std::string const& data) {
        auto lines = string::split(data, "\n");
        if (lines.size() < 2) return;
        auto values = csvRow(lines[1]);
        if (values.size() < 5 || values[0] == INDICATOR_LOCAL) return;

        auto msg = fmt::format(
            "Current status: <cr>{}</c>\n"
            "Maintenance type: <cg>{}</c>\n"
            "Problem main: <c4>{}</c>\n"
            "Estimated time: <cy>{}</c>\n\n"
            "<co>Check details in the official </c><cd>Discord server!</c>",
            values[1], values[2], values[4], values[3]);

        createQuickPopup("Server Maintenance", msg, "Later", "Discord",
            [](auto, bool discord) {
                if (discord) web::openLinkInBrowser(DISCORD_LINK);
            });
    }

    static void requestMaintenance() {
        if (g_maintenanceRequested) return;
        g_maintenanceRequested = true;
        auto timestamp = std::chrono::system_clock::now().time_since_epoch().count();
        auto url = fmt::format("{}&t={}", MAINTENANCE_URL, timestamp);
        async::spawn(web::WebRequest().get(url), [](web::WebResponse res) {
            if (!res.ok()) return;
            if (auto data = res.string(); data) checkMaintenance(data.unwrap());
        });
    }
};

class $modify(SalamandraMaintenanceHook, MenuLayer) {
    $override bool init() {
        if (!MenuLayer::init()) return false;
        SalamandraMaintenanceChecker::requestMaintenance();
        return true;
    }
};
