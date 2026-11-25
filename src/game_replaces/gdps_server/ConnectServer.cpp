#include <Geode/Geode.hpp>
#include <Geode/modify/CCHttpClient.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/utils/async.hpp>
#include <Geode/utils/web.hpp>
#include <algorithm>
#include <chrono>
#include <cctype>
#include <string>
#include <string_view>
#include <vector>

using namespace geode::prelude;
using namespace cocos2d::extension;

namespace {
    constexpr std::string_view kBase = "https://salamandra.ps.fhgdps.com";
    constexpr auto kTokenURL = "https://salamandra.ps.fhgdps.com/salamandra_token.php";
    constexpr auto kBootstrapKey = "1e4051718b5371d11aa0bf89282224f404d615cb58d77ba1efa80ce268c173fb";
    using Clock = std::chrono::steady_clock;
    using namespace std::chrono_literals;

    std::string s_token;
    Clock::time_point s_expiry{};
    Clock::time_point s_refresh{};
    Clock::time_point s_retry{};
    bool s_fetching = false;
    bool s_timerStarted = false;
    bool s_probeStarted = false;
    unsigned int s_attempt = 0;
    std::vector<CCHttpRequest*> s_pending;
    thread_local CCHttpRequest* s_replay = nullptr;

    bool isHexToken(std::string const& token) {
        return token.size() == 64 && std::all_of(token.begin(), token.end(), [](char c) {
            return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f');
        });
    }

    bool hasToken() {
        return !s_token.empty() && Clock::now() < s_expiry;
    }

    bool isTokenHeader(std::string_view header) {
        auto colon = header.find(':');
        if (colon == std::string_view::npos) return false;
        auto name = header.substr(0, colon);
        constexpr std::string_view expected = "x-salamandra-token";
        return name.size() == expected.size() && std::equal(name.begin(), name.end(), expected.begin(), [](char a, char b) {
            return std::tolower(static_cast<unsigned char>(a)) == b;
        });
    }

    std::string safePath(std::string_view url) {
        auto scheme = url.find("://");
        auto start = scheme == std::string_view::npos ? 0 : url.find('/', scheme + 3);
        if (start == std::string_view::npos) return "/";
        auto path = url.substr(start);
        return std::string(path.substr(0, path.find_first_of("?#")));
    }

    void setTokenHeader(CCHttpRequest* req) {
        auto old = req->getHeaders();
        auto headers = old;
        headers.clear();
        for (auto const& h : old) {
            if (!isTokenHeader(h.c_str())) headers.push_back(h);
        }
        if (hasToken()) {
            auto header = std::string("X-Salamandra-Token: ") + s_token;
            headers.push_back(header.c_str());
        }
        req->setHeaders(headers);
        bool present = false;
        for (auto const& h : req->getHeaders()) {
            if (isTokenHeader(h.c_str())) present = true;
        }
        log::info("Salamandra Auth: ruta={} cabecera_token={} token_valido={}",
                  safePath(req->getUrl()), present, hasToken());
    }

    void flushPending() {
        auto pending = std::move(s_pending);
        s_pending.clear();
        if (!pending.empty()) {
            log::info("Salamandra Auth: enviando {} peticiones pendientes; token valido={}", pending.size(), hasToken());
        }
        for (auto* req : pending) {
            setTokenHeader(req);
            auto* previous = s_replay;
            s_replay = req;
            CCHttpClient::getInstance()->send(req);
            s_replay = previous;
            req->release();
        }
    }

    void probeGuard() {
        if (!hasToken()) return;
        log::info("Salamandra Auth: PROBE POST /salamandra_probe.php");
        async::spawn(
            web::WebRequest()
                .userAgent("Salamandra/2.1.2")
                .header("X-Salamandra-Token", s_token)
                .followRedirects(false)
                .timeout(asp::Duration::fromSecs(15))
                .post("https://salamandra.ps.fhgdps.com/salamandra_probe.php"),
            [](web::WebResponse response) {
                auto body = response.string();
                bool accepted = response.ok() && body && string::trim(body.unwrap()) == "GUARD_OK";
                log::info("Salamandra Auth: PROBE HTTP {} ({} bytes), guard={}",
                          response.code(), response.data().size(), accepted ? "OK" : "ERROR");
                if (!accepted && body && string::trim(body.unwrap()) == "-1") {
                    log::warn("Salamandra Auth: PROBE token rechazado por PHP; revisar error_log");
                }
            }
        );
    }

    void requestToken() {
        auto started = Clock::now();
        if (s_fetching || started < s_retry) return;
        s_fetching = true;
        auto attempt = ++s_attempt;
        log::info("Salamandra Auth: POST salamandra_token.php (intento {}, {})", attempt,
                  hasToken() ? "renovacion" : "token inicial");
        async::spawn(
            web::WebRequest()
                .userAgent("Salamandra/2.1.2")
                .header("X-Salamandra-Key", kBootstrapKey)
                .header("Content-Type", "application/json")
                .bodyString(fmt::format("{{\"key\":\"{}\"}}", kBootstrapKey))
                .followRedirects(false)
                .timeout(asp::Duration::fromSecs(15))
                .post(kTokenURL),
            [started, attempt](web::WebResponse response) {
                s_fetching = false;
                auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - started).count();
                log::info("Salamandra Auth: respuesta intento {} HTTP {} ({} ms, {} bytes)",
                          attempt, response.code(), elapsed, response.data().size());
                auto body = response.string();
                if (response.ok() && body) {
                    auto value = string::trim(body.unwrap());
                    if (isHexToken(value) && Clock::now() < started + 840s) {
                        s_token = std::move(value);
                        s_expiry = started + 840s;
                        s_refresh = started + 600s;
                        s_retry = {};
                        log::info("Salamandra Auth: TOKEN OK; renovacion en 600 s, margen local 60 s");
                        if (!s_probeStarted) {
                            s_probeStarted = true;
                            probeGuard();
                        }
                        flushPending();
                        return;
                    }
                    log::warn("Salamandra Auth: HTTP exitoso pero token invalido ({} caracteres)", value.size());
                } else if (response.code() == 403) {
                    log::warn("Salamandra Auth: HTTP 403; clave rechazada o firewall del hosting");
                } else if (response.code() == 404) {
                    log::warn("Salamandra Auth: HTTP 404; revisar ruta de salamandra_token.php");
                } else if (response.code() == 405) {
                    log::warn("Salamandra Auth: HTTP 405; el servidor no acepta POST");
                } else if (response.code() == 429) {
                    log::warn("Salamandra Auth: HTTP 429; limite de emision alcanzado");
                } else if (response.code() == 503 || response.code() == 500) {
                    log::warn("Salamandra Auth: error PHP/SQL; revisar error_log de cPanel");
                } else if (response.redirected()) {
                    log::warn("Salamandra Auth: redireccion HTTP no permitida; revisar URL y SSL");
                } else {
                    log::warn("Salamandra Auth: fallo de red o respuesta no esperada (codigo {})", response.code());
                }
                if (body) {
                    auto value = string::trim(body.unwrap());
                    for (auto code : {"ERR_METHOD", "ERR_KEY_MISSING", "ERR_KEY_INVALID", "ERR_DB_CONFIG", "ERR_DB_CONNECT", "ERR_RATE_LIMIT", "ERR_SQL"}) {
                        if (value == code) {
                            log::warn("Salamandra Auth: diagnostico PHP {}", code);
                            break;
                        }
                    }
                }
                s_retry = Clock::now() + 30s;
                log::warn("Salamandra Auth: reintento en 30 s; token anterior valido={}", hasToken());
                flushPending();
            }
        );
    }

    void refreshIfNeeded() {
        if (!hasToken() || Clock::now() >= s_refresh) requestToken();
    }

    class TokenTimer : public CCObject {
    public:
        void tick(float) {
            refreshIfNeeded();
        }
    };

    void startTimer() {
        if (s_timerStarted) return;
        auto* scheduler = CCDirector::get()->getScheduler();
        if (!scheduler) return;
        auto* timer = new TokenTimer();
        scheduler->scheduleSelector(schedule_selector(TokenTimer::tick), timer, 5.f, false);
        timer->release();
        s_timerStarted = true;
        log::info("Salamandra Auth: temporizador de renovacion activo (5 s)");
    }

    std::string_view afterHost(std::string_view url, std::string_view host) {
        auto schemeEnd = url.find("://");
        if (schemeEnd == std::string_view::npos) return {};
        auto scheme = url.substr(0, schemeEnd);
        if (scheme != "https" && scheme != "http") return {};
        url.remove_prefix(schemeEnd + 3);
        auto end = url.find_first_of("/?#");
        if (url.substr(0, end) != host || end == std::string_view::npos) return {};
        return url.substr(end);
    }

    bool isProtected(std::string_view url) {
        if (!url.starts_with(kBase) || url.size() <= kBase.size() || url[kBase.size()] != '/') return false;
        auto path = url.substr(kBase.size());
        path = path.substr(0, path.find_first_of("?#"));
        return path.ends_with(".php") && path != "/salamandra_token.php";
    }
}

class $modify(SalamandraTokenMenu, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;
        log::debug("Salamandra Auth: MenuLayer inicializado");
        startTimer();
        refreshIfNeeded();
        return true;
    }
};

class $modify(MyHttpClient, CCHttpClient) {
    void send(CCHttpRequest* req) {
        if (!req || !req->getUrl() || req == s_replay) return CCHttpClient::send(req);

        std::string url = req->getUrl();
        auto ngPath = afterHost(url, "www.newgrounds.com");
        if (ngPath.empty()) ngPath = afterHost(url, "newgrounds.com");
        constexpr std::string_view musicPrefix = "/audio/download/";
        if (ngPath.starts_with(musicPrefix)) {
            auto id = ngPath.substr(musicPrefix.size());
            id = id.substr(0, id.find_first_of("?#"));
            if (!id.empty() && std::all_of(id.begin(), id.end(), [](char c) { return c >= '0' && c <= '9'; })) {
                auto target = std::string(kBase) + "/music/" + std::string(id) + ".mp3";
                req->setUrl(target.c_str());
            }
            return CCHttpClient::send(req);
        }

        auto path = afterHost(url, "www.boomlings.com");
        if (path.empty()) path = afterHost(url, "boomlings.com");
        if (!path.empty()) {
            constexpr std::string_view dbPrefix = "/database/";
            std::string target = std::string(kBase) + "/";
            if (path.starts_with(dbPrefix)) target += path.substr(dbPrefix.size());
            log::info("Salamandra Auth: redireccion {} -> {}", safePath(url), safePath(target));
            req->setUrl(target.c_str());
            url = std::move(target);
        }

        if (!isProtected(url)) return CCHttpClient::send(req);

        refreshIfNeeded();
        if (hasToken()) {
            log::debug("Salamandra Auth: peticion PHP con token");
            setTokenHeader(req);
            return CCHttpClient::send(req);
        }

        if (s_fetching && s_pending.size() < 64) {
            req->retain();
            s_pending.push_back(req);
            log::info("Salamandra Auth: en cola {} ({} pendientes)", safePath(url), s_pending.size());
            return;
        }

        log::warn("Salamandra Auth: peticion sin token; servidor protegido debe rechazarla");
        setTokenHeader(req);
        return CCHttpClient::send(req);
    }
};

$on_mod(Loaded) {
    log::info("Salamandra Auth: modulo de autenticacion cargado");
}
