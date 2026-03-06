#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>

using namespace geode::prelude;

void testAuth() {
    std::string token = "EJEMPLODESEGURIDAD";

    web::WebRequest req;
    req.header("Authorization", token);

    auto task = req.get("https://salamandra.ps.fhgdps.com/incl/auth.php");

    task.wait(); // Espera la respuesta (forma simple)

    if (task.isResolved()) {
        auto response = task.get();
        log::info("Respuesta del servidor: {}", response.string());
    }
    else {
        log::error("Error en la petición");
    }
}
