#include "AuthGate.hpp"
#include "HWID.hpp"

#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>

#include <ctime>

using namespace geode::prelude;
using namespace geode::utils::web;

std::string AuthGate::m_session = "";

static const std::string SECRET = "S4l4m4ndr4_Auth_9F6D2C7E8B1A54C3D0F8";

void AuthGate::authenticate() {

    std::string hwid = HWID::get();
    std::string timestamp = std::to_string(std::time(nullptr));

    std::string hash = geode::utils::crypto::sha256(
        SECRET + hwid + timestamp
    );

    WebRequest req;

    req.bodyString(
        "hwid=" + hwid +
        "&timestamp=" + timestamp +
        "&hash=" + hash
    );

    req.post("https://salamandra.ps.fhgdps.com/api/gate.php")
    .then([](WebResponse* res) {

        if (!res || !res->ok()) {
            log::error("Gate auth failed");
            return;
        }

        AuthGate::m_session = res->string().unwrapOr("");
        log::info("Session: {}", AuthGate::m_session);

    });
}

std::string AuthGate::getSession() {
    return m_session;
}