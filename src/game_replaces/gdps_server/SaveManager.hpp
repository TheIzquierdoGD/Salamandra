#pragma once
#include <Geode/Geode.hpp>
#include <Geode/modify/GManager.hpp>

using namespace geode::prelude;

class $modify(GSGManager, GManager) {
    struct Fields {
        std::string m_originalFileName = "";
    };

    void setup() override;
};