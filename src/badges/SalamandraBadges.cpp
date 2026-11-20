#include "SalamandraBadges.hpp"

#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/binding/FLAlertLayer.hpp>
#include <Geode/utils/web.hpp>

#include <algorithm>
#include <array>
#include <charconv>
#include <cctype>
#include <optional>
#include <string_view>
#include <system_error>
#include <utility>

using namespace geode::prelude;

namespace {
    constexpr char kBadgesCsvUrl[] =
        "https://docs.google.com/spreadsheets/d/e/"
        "2PACX-1vREvZamVP7j7KHfJOSjQzOT7Z5zMnxLF8ER-sIppTnGDZQa29Cxe0xR3k0ANHOFR85fwGd5lQnFU5gl/"
        "pub?output=csv";

    // Keep only the newest request if MenuLayer is constructed repeatedly.
    async::TaskHolder<web::WebResponse> s_badgesRequest;

    std::string trimCopy(std::string_view text) {
        while (!text.empty() && std::isspace(static_cast<unsigned char>(text.front()))) {
            text.remove_prefix(1);
        }
        while (!text.empty() && std::isspace(static_cast<unsigned char>(text.back()))) {
            text.remove_suffix(1);
        }
        return std::string(text);
    }

    // Google Sheets may quote names with commas, double quotes, or line breaks.
    // Splitting the CSV directly on commas would corrupt those records.
    std::vector<std::vector<std::string>> parseCsvRows(std::string_view input) {
        std::vector<std::vector<std::string>> rows;
        std::vector<std::string> row;
        std::string field;
        bool inQuotes = false;

        for (size_t i = 0; i < input.size(); ++i) {
            char ch = input[i];
            if (ch == '"') {
                if (inQuotes && i + 1 < input.size() && input[i + 1] == '"') {
                    field += '"';
                    ++i;
                } else {
                    inQuotes = !inQuotes;
                }
            } else if (ch == ',' && !inQuotes) {
                row.push_back(std::move(field));
                field.clear();
            } else if ((ch == '\n' || ch == '\r') && !inQuotes) {
                if (ch == '\r' && i + 1 < input.size() && input[i + 1] == '\n') {
                    ++i;
                }
                row.push_back(std::move(field));
                field.clear();
                rows.push_back(std::move(row));
                row.clear();
            } else {
                field += ch;
            }
        }
        if (!row.empty() || !field.empty()) {
            row.push_back(std::move(field));
            rows.push_back(std::move(row));
        }
        return rows;
    }

    std::optional<int> parseInt(std::string_view text) {
        auto clean = trimCopy(text);
        int value = 0;
        auto result = std::from_chars(clean.data(), clean.data() + clean.size(), value);
        if (result.ec != std::errc{} || result.ptr != clean.data() + clean.size()) {
            return std::nullopt;
        }
        return value;
    }

    void parseBadgesCsv(std::string const& data) {
        auto rows = parseCsvRows(data);
        if (rows.size() < 2) return;

        size_t idColumn = std::string::npos;
        size_t nameColumn = std::string::npos;
        size_t badgeColumn = std::string::npos;

        for (size_t i = 0; i < rows[0].size(); ++i) {
            auto header = trimCopy(rows[0][i]);
            if (i == 0 && header.starts_with("\xEF\xBB\xBF")) {
                header.erase(0, 3); // UTF-8 BOM
            }
            if (header == "ID") idColumn = i;
            else if (header == "Name") nameColumn = i;
            else if (header == "Badge") badgeColumn = i;
        }

        if (idColumn == std::string::npos ||
            nameColumn == std::string::npos ||
            badgeColumn == std::string::npos) {
            log::warn("Salamandra Badges: invalid CSV headers (expected ID, Name, Badge)");
            return;
        }

        std::map<int, SalamandraBadge> updatedBadges;
        for (size_t i = 1; i < rows.size(); ++i) {
            auto const& columns = rows[i];
            if (idColumn >= columns.size() ||
                nameColumn >= columns.size() ||
                badgeColumn >= columns.size()) continue;

            auto accountID = parseInt(columns[idColumn]);
            auto name = trimCopy(columns[nameColumn]);
            if (!accountID || *accountID <= 0 || name.empty()) continue;

            std::vector<int> types;
            std::string_view raw = columns[badgeColumn];
            while (true) {
                auto separator = raw.find(';');
                auto part = raw.substr(0, separator);
                auto type = parseInt(part);
                if (type && *type > 0 &&
                    std::find(types.begin(), types.end(), *type) == types.end()) {
                    types.push_back(*type);
                }
                if (separator == std::string_view::npos) break;
                raw.remove_prefix(separator + 1);
            }

            if (!types.empty()) {
                updatedBadges[*accountID] = {*accountID, std::move(types), std::move(name)};
            }
        }

        // A temporary server error or malformed response must not erase
        // previously loaded badges from an earlier successful request.
        if (updatedBadges.empty()) {
            log::warn("Salamandra Badges: CSV contained no valid badges");
            return;
        }
        SalamandraBadges::salamandraBadges.swap(updatedBadges);
        log::debug("Salamandra Badges: loaded {} accounts", SalamandraBadges::salamandraBadges.size());
    }

    void requestBadgesFromServer() {
        s_badgesRequest.spawn(
            web::WebRequest().get(kBadgesCsvUrl),
            [](web::WebResponse response) {
                // TaskHolder completion callbacks execute on the main thread.
                if (!response.ok()) {
                    log::warn("Salamandra Badges: HTTP request failed (status {})", response.code());
                    return;
                }
                auto body = response.string();
                if (!body) {
                    log::warn("Salamandra Badges: could not read CSV response");
                    return;
                }
                parseBadgesCsv(body.unwrap());
            }
        );
    }

    constexpr std::array<std::string_view, 7> kBadgeNames = {
        "None", "Moderator", "Elder Mod", "Rank Mod", "Supervisor", "Donator", "Content Creator"
    };
    constexpr std::array<std::string_view, 7> kBadgeDescriptions = {
        "does not have a defined rank",
        "is a <cp>Moderator</c> of the <cy>GDPS.</c>\nThey help manage the community",
        "is a <cp>Elder Mod</c> of the <cy>GDPS.</c>\nThey provide <cy>leadership</c>",
        "is a <cp>Rank Mod</c> of the <cy>GDPS.</c>\nThey manage the <cy>leaderboards</c>",
        "is a <cp>Supervisor</c> of the <cy>GDPS.</c>\nThey oversee the <cy>moderator operations</c>",
        "is a <cp>Donator</c> of the <cy>GDPS.</c>\nThey provide <cy>financial support</c>",
        "is an <cd>Approved Content Creator</c>.\nThey promote the server on <cy>social media</c>"
    };
}

std::map<int, SalamandraBadge> SalamandraBadges::salamandraBadges;

class $modify(SBBadgesMenuFetch, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;
        requestBadgesFromServer();
        return true;
    }
};

SalamandraBadge* SalamandraBadges::badgeForUser(int id) {
    auto it = salamandraBadges.find(id);
    return it == salamandraBadges.end() ? nullptr : &it->second;
}

void SalamandraBadges::showBadgeInfo(std::string_view username, int type) {
    auto idx = type >= 0 && static_cast<size_t>(type) < kBadgeNames.size()
        ? static_cast<size_t>(type)
        : size_t{0};

    FLAlertLayer::create(
        kBadgeNames[idx].data(),
        fmt::format("<cg>{}</c> {}.", username, kBadgeDescriptions[idx]),
        "OK"
    )->show();
}
