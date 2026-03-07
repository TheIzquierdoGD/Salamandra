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

    matjson::Value body = matjson::Value::object({
        {"password", password},
        {"hwid", hwid},
        {"timestamp", timestamp}
    });

    req.bodyJSON(body);

    req.post("https://salamandra.ps.fhgdps.com/api/gate.php")
    .listen([](web::WebResponse* res) {

        if (!res) {
            log::error("Auth request failed");
            return;
        }

        if (!res->ok()) {
            log::error("Auth server returned error");
            return;
        }

        auto result = res->string().unwrapOr("error");

        if (result == "OK") {
            log::info("Auth success");
        } else {
            log::error("Auth rejected");
        }
    });
}
