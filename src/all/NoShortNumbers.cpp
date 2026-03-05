//No Short Numbers
#include <Geode/Geode.hpp>
#include <Geode/modify/GameToolbox.hpp>

using namespace geode::prelude;

class $modify(GameToolbox) {
    static gd::string intToShortString(int num) {
        // Siempre mostrar número completo
        return gd::string(fmt::format("{}", num).c_str());
    }
};