#include <geode.core.hpp>
#include <geode.utils/web.hpp>

using namespace geode::prelude;

void testAuth() {
    // Su clave (debe coincidir con $SECRET en auth.php)
    std::string token = "EJEMPLODESEGURIDAD";

    auto req = web::WebRequest();

    // Agregar header Authorization
    req.header("Authorization", token);

    // Hacer petición GET
    auto result = req.get("https://salamandra.ps.fhgdps.com/incl/auth.php");

    result
        .then([](web::WebResponse* response) {
            log::info("Respuesta del servidor: {}", response->string());
        })
        .except([](std::string const& error) {
            log::error("Error en request: {}", error);
        });
}
