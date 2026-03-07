#include "AuthGate.hpp"

#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>
#include <matjson.hpp>

using namespace geode::prelude;

std::string getHWID();

void AuthGate::sendAuth() {
    web::WebRequest req;

    std::string password = "S4l4m4ndr4_Str0ng_P4ss";
    std::string hwid = getHWID();
    std::string timestamp = std::to_string(time(nullptr));

    // Crear JSON
    matjson::Value body = matjson::Value::object();
    body["password"] = password;
    body["hwid"] = hwid;
    body["timestamp"] = timestamp;

    req.bodyJSON(body);

    // Enviar request
    auto future = req.post("https://salamandra.ps.fhgdps.com/api/gate.php");

    // Esperar respuesta
    auto result = future.text();

    if (!result) {
        log::error("Auth request failed");
        return;
    }

    if (result.unwrap() == "OK") {
        log::info("Auth success");
    } else {
        log::error("Auth rejected");
    }
}
