#pragma once

#include <map>
#include <string>
#include <string_view>
#include <vector>

// Original badges system by Hiimjasmine00, adapted for Salamandra by TheIzquierdoGD.
struct SalamandraBadge {
    int id = 0;
    std::vector<int> types;
    std::string name;
};

namespace SalamandraBadges {
    extern std::map<int, SalamandraBadge> salamandraBadges;

    // The pointer remains valid only until the badges table is refreshed.
    SalamandraBadge* badgeForUser(int id);
    void showBadgeInfo(std::string_view username, int type);
}
