#include "SalamandraBadges.hpp"
#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/binding/FLAlertLayer.hpp>
#include <Geode/utils/web.hpp>

using namespace geode::prelude;

#define BADGES_CSV_URL "https://docs.google.com/spreadsheets/d/e/2PACX-1vREvZamVP7j7KHfJOSjQzOT7Z5zMnxLF8ER-sIppTnGDZQa29Cxe0xR3k0ANHOFR85fwGd5lQnFU5gl/pub?output=csv"

std::map<int, SalamandraBadge> SalamandraBadges::salamandraBadges;

void parseBadgesCSV(const std::string& data) {
    auto lines = string::split(data, "\n");
    if (lines.size() < 2) return;

    auto keys = string::split(lines[0], ",");
    SalamandraBadges::salamandraBadges.clear();

    for (size_t i = 1; i < lines.size(); ++i) {
        auto line = string::trim(lines[i]);
        if (line.empty()) continue;

        auto values = string::split(line, ",");
        int id = 0;
        std::string name;
        std::vector<int> types;

        for (size_t j = 0; j < keys.size(); ++j) {
            if (j >= values.size()) continue;
            auto key = string::trim(keys[j]);
            auto value = string::trim(values[j]);

            if (key == "ID") {
                id = numFromString<int>(value).unwrapOr(0);
            } else if (key == "Name") {
                name = value;
            } else if (key == "Badge") {
                auto badgeParts = string::split(value, ";");
                for (auto& part : badgeParts) {
                    int t = numFromString<int>(string::trim(part)).unwrapOr(0);
                    if (t > 0) types.push_back(t);
                }
            }
        }

        if (id > 0 && !name.empty() && !types.empty()) {
            SalamandraBadges::salamandraBadges[id] = { id, types, name };
        }
    }
}

void requestBadgesFromServer() {
    spawn(
        web::WebRequest().get(BADGES_CSV_URL),
        [](web::WebResponse res) {
            if (res.ok()) {
                auto data = res.string().unwrapOr("");
                if (!data.empty()) {
                    parseBadgesCSV(data);
                }
            }
        }
    );
}

class $modify(MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;
        
        requestBadgesFromServer();

        return true;
    }
};

SalamandraBadge* SalamandraBadges::badgeForUser(int id) {
    auto it = salamandraBadges.find(id);
    return (it != salamandraBadges.end()) ? &it->second : nullptr;
}

static constexpr std::array names = {
    "None", "Moderator", "Elder Mod", "Rank Mod", "Supervisor", "Donator", "Content Creator"
};

static constexpr std::array descriptions = {
    "does not have a defined rank",
    "is a <cp>Moderator</c> of the <cy>GDPS.</c>\nThey help manage the community",
    "is a <cp>Elder Mod</c> of the <cy>GDPS.</c>\nThey provide <cy>leadership</c>",
    "is a <cp>Rank Mod</c> of the <cy>GDPS.</c>\nThey manage the <cy>leaderboards</c>",
    "is a <cp>Supervisor</c> of the <cy>GDPS.</c>\nThey oversee the <cy>moderator operations</c>",
    "is a <cp>Donator</c> of the <cy>GDPS.</c>\nThey provide <cy>financial support</c>",
    "is an <cd>Approved Content Creator</c>.\nThey promote the server on <cy>social media</c>",
};

void SalamandraBadges::showBadgeInfo(std::string_view username, int type) {
    size_t idx = static_cast<size_t>(type);
    if (idx >= names.size()) idx = 0;

    FLAlertLayer::create(
        names[idx],
        fmt::format("<cg>{}</c> {}.", username, descriptions[idx]),
        "OK"
    )->show();
}