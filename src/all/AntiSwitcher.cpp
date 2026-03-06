#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>

using namespace geode::prelude;

void testAuth() {
    std::string token = "EJEMPLODESEGURIDAD";

    web::WebRequest request;
    request.header("Authorization", token);

    auto future = request.send(
        web::RequestType::Get,
        "https://salamandra.ps.fhgdps.com/incl/auth.php"
    );

    future.onResolve([](web::WebResponse& response) {
        log::info("Respuesta del servidor: {}", response.string());
    });

    future.onReject([](std::string const& error) {
        log::error("Error en request: {}", error);
    });
}
