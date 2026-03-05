#include <string>
#include <unordered_map>
#include <random>
#include <chrono>

class TokenManager {
public:
    std::string generateToken(const std::string& userId) {
        std::string token = createRandomToken();
        tokens[userId] = token;
        return token;
    }

    bool validateToken(const std::string& userId, const std::string& token) {
        auto it = tokens.find(userId);
        if (it == tokens.end()) return false;
        return it->second == token;
    }

    void revokeToken(const std::string& userId) {
        tokens.erase(userId);
    }

private:
    std::unordered_map<std::string, std::string> tokens;

    std::string createRandomToken() {
        static const char charset[] =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";

        static thread_local std::mt19937 rng(
            static_cast<unsigned>(
                std::chrono::steady_clock::now().time_since_epoch().count()
            )
        );

        static std::uniform_int_distribution<> dist(0, sizeof(charset) - 2);

        std::string token;
        token.reserve(32);

        for (int i = 0; i < 32; ++i) {
            token += charset[dist(rng)];
        }

        return token;
    }
};