#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>

using namespace geode::prelude;

void testAuth() {
    std::string token = "EJEMPLODESEGURIDAD";

    web::WebRequest request;
    request.header("Authorization", token);

    auto response = request
        .get("https://salamandra.ps.fhgdps.com/incl/auth.php")
        .send();  // <-- IMPORTANTE

    if (response) {
        log::info("Respuesta del servidor: {}", response->string());
    }
    else {
        log::error("Error en la petición");
    }
}
