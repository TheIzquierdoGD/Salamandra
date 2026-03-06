#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>

using namespace geode::prelude;

void testAuth() {
    std::string token = "EJEMPLODESEGURIDAD";

    web::WebRequest request;
    request.header("Authorization", token);

    request.get("https://salamandra.ps.fhgdps.com/incl/auth.php")
        .then([](web::WebResponse response) {
            log::info("Respuesta del servidor: {}", response.string());
        })
        .expect([](std::string const& error) {
            log::error("Error en request: {}", error);
        });
}
