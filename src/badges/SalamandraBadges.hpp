#pragma once
#include <ccTypes.h>
#include <string>
#include <map>
#include <vector>
#include <string_view>

//Disclaimer: All the code was created by Hiimjasmine00 and adapted by TheIzquierdoGD from the "Developer Badges" mod
struct SalamandraBadge {
    int id;
    std::vector<int> types; 
    std::string name;
};

namespace SalamandraBadges {
    extern std::map<int, SalamandraBadge> salamandraBadges;
    SalamandraBadge* badgeForUser(int id);
    void showBadgeInfo(std::string_view username, int type);
}