#pragma once

#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>

using namespace geode::prelude;

class UpdateChecker {
public:
    static void checkForUpdate();
};