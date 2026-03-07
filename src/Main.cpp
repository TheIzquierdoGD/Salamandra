#include <Geode/Geode.hpp>
#include "AuthGate.hpp"

using namespace geode::prelude;

$on_mod(Loaded) {
    log::info("Salamandra loaded");
    AuthGate::sendAuth();
}
