// Do not touch if you don't know what you're doing [Consult the code owner]
#include <Geode/modify/CCHttpClient.hpp>
#include <Geode/Geode.hpp>
#include <string>
#include <algorithm>

using namespace geode::prelude;
using namespace cocos2d::extension;

static std::string global_salt = "BjsUEdf0";

std::string getAlphanumericAlphabet(std::string seed) {
    std::string base = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    unsigned int seed_val = 0;
    for (char c : seed) seed_val = seed_val * 31 + c;

    int n = base.length();
    for (int i = n - 1; i > 0; i--) {
        seed_val = (seed_val * 1103515245 + 12345) & 0x7fffffff;
        int j = seed_val % (i + 1);
        std::swap(base[i], base[j]);
    }
    return base;
}

std::string getDynamicSuffix(std::string seed) {
    std::string alpha = getAlphanumericAlphabet(seed + "_suffix");
    return alpha.substr(0, 8);
}

std::string secure_encode_final(std::string const& s, std::string seed) {
    std::string ret;
    std::string alphabet = getAlphanumericAlphabet(seed);
    
    for (unsigned char c : s) {
        ret += alphabet[c % 62];
        ret += alphabet[(c * 3) % 62];
    }
    
    return ret + getDynamicSuffix(seed);
}

class $modify(MyHttpClient, CCHttpClient) {
    void send(CCHttpRequest* req) {
        if (!req || !req->getUrl()) return CCHttpClient::send(req);

        std::string url = req->getUrl();
        std::string baseUrl = "https://salamandra.ps.fhgdps.com";

        if (url.find("newgrounds.com/audio/download/") != std::string::npos) {
            size_t pos = url.find_last_of("/");
            if (pos != std::string::npos) {
                std::string songID = url.substr(pos + 1);
                req->setUrl((baseUrl + "/music/" + songID + ".mp3").c_str());
            }
            return CCHttpClient::send(req);
        }

        if (url.find("boomlings.com") != std::string::npos) {
            std::string newUrl = baseUrl;
            size_t dbPos = url.find("/database/");

            if (dbPos != std::string::npos) {
                std::string pathAfterDb = url.substr(dbPos + 10);
                
                size_t lastSlash = pathAfterDb.find_last_of("/");
                
                if (lastSlash == std::string::npos) {
                    if (pathAfterDb.find(".php") != std::string::npos) {
                        std::string baseName = pathAfterDb.substr(0, pathAfterDb.find(".php"));
                        std::string encodedName = secure_encode_final(baseName, global_salt);
                        newUrl += "/" + encodedName + ".php";
                    } else {
                        newUrl += "/" + pathAfterDb;
                    }
                } else {
                    newUrl += "/" + pathAfterDb;
                }

            } else {
                newUrl += "/"; 
            }

            req->setUrl(newUrl.c_str());
            return CCHttpClient::send(req);
        }
        return CCHttpClient::send(req);
    }
};