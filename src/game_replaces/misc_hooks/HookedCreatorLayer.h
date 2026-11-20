#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/CreatorLayer.hpp>

class $modify(HookedCreatorLayer, CreatorLayer) {
    $override
    bool init();

    void setupSalamandraLogo(float dt);
};
