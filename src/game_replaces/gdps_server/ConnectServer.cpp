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
    }

    void flushPending() {
        auto pending = std::move(s_pending);
        s_pending.clear();
        for (auto* req : pending) {
            setTokenHeader(req);
            auto* previous = s_replay;
            s_replay = req;
            CCHttpClient::getInstance()->send(req);
            s_replay = previous;
            req->release();
        }
    }

    void requestToken() {
        auto started = Clock::now();
        if (s_fetching || started < s_retry) return;
        s_fetching = true;
        async::spawn(
            web::WebRequest()
                .header("X-Salamandra-Key", kBootstrapKey)
                .followRedirects(false)
                .timeout(asp::Duration::fromSecs(15))
                .post(kTokenURL),
            [started](web::WebResponse response) {
                s_fetching = false;
                if (response.ok()) {
                    auto body = response.string();
                    if (body && isHexToken(body.unwrap()) && Clock::now() < started + 840s) {
                        s_token = body.unwrap();
                        s_expiry = started + 840s;
                        s_refresh = started + 600s;
                        s_retry = {};
                        log::info("Salamandra: token temporal renovado");
                        flushPending();
                        return;
                    }
                }
                s_retry = Clock::now() + 30s;
                log::warn("Salamandra: no se pudo renovar el token (HTTP {})", response.code());
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
            req->setUrl(target.c_str());
            url = std::move(target);
        }

        if (!isProtected(url)) return CCHttpClient::send(req);

        refreshIfNeeded();
        if (hasToken()) {
            setTokenHeader(req);
            return CCHttpClient::send(req);
        }

        if (s_fetching && s_pending.size() < 64) {
            req->retain();
            s_pending.push_back(req);
            return;
        }

        setTokenHeader(req);
        return CCHttpClient::send(req);
    }
};
