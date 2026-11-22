#pragma once

#include <map>
#include <string>
#include <string_view>
#include <vector>

struct SalamandraBadge {
    int id = 0;
    std::vector<int> types;
    std::string name;
};

namespace SalamandraBadges {
    extern std::map<int, SalamandraBadge> salamandraBadges;

    SalamandraBadge* badgeForUser(int id);
    void showBadgeInfo(std::string_view username, int type);
}
