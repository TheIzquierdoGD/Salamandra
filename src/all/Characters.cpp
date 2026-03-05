// Text Bypass
#include <Geode/Geode.hpp>
#include <Geode/modify/CCTextInputNode.hpp>
#include <string>

using namespace geode::prelude;

bool g_charLimitBypass = true;
bool g_charFilterBypass = true;

class $modify(CCTextInputNode) {
    
    void updateLabel(gd::string p0) {

        // Convertir a std::string para evitar errores de ZStringView
        std::string id = std::string(this->getID());

        if (id.rfind("IGNOREBYPASSES", 0) == 0) {
            CCTextInputNode::updateLabel(p0);
            return;
        }

        if (g_charLimitBypass) {
            this->setMaxLabelLength(6969);
        }

        if (g_charFilterBypass) {
            this->setAllowedChars(
                "!\"#$%&'()*+,-./0123456789:;<=>?@"
                "ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`"
                "abcdefghijklmnopqrstuvwxyz{|}~ "
            );
        }

        CCTextInputNode::updateLabel(p0);
    }
};