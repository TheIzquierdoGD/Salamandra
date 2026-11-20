#pragma once

// Geode V6 - GManager save file redirection.
#include <Geode/Geode.hpp>
#include <Geode/modify/GManager.hpp>

#include <string>

using namespace geode::prelude;

class $modify(GSGManager, GManager) {
    struct Fields {
        std::string m_originalFileName = "";
    };

    void setup() override;
};
