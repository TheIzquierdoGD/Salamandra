#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>

using namespace geode::prelude;

void testAuth() {
    std::string token = "EJEMPLODESEGURIDAD";

    auto req = web::WebRequest();
    req.header("Authorization", token);

    req.get("https://salamandra.ps.fhgdps.com/incl/auth.php")
        .then([](web::WebResponse* response) {
            log::info("Respuesta del servidor: {}", response->string());
        })
        .except([](std::string const& error) {
            log::error("Error en request: {}", error);
        });
}
