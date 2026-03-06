#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>

using namespace geode::prelude;

void testAuth() {
    web::WebRequest request;

    request.header("Authorization", "EJEMPLODESEGURIDAD");

    auto task = request
        .get("https://salamandra.ps.fhgdps.com/incl/auth.php");

    auto response = task.send();  // <- aquí está la clave

    if (response) {
        log::info("Respuesta: {}", response->string());
    }
    else {
        log::error("Fallo en la petición");
    }
}
