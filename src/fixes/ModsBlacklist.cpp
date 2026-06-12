// This disables mods and restarts the game [It doesn't do anything unusual]
#include <Geode/Geode.hpp>
#include <Geode/loader/Loader.hpp>
#include <string>
#include <vector>
#include <array>

using namespace geode::prelude;

class ISecureMod {
public:
    virtual ~ISecureMod() = default;
    virtual bool isIntact() const = 0;
    virtual std::string getString() const = 0;
};

template<size_t N>
class SecureMod : public ISecureMod {
public:
    std::array<char, N> data;
    int expectedSum;

    constexpr SecureMod(const char(&str)[N]) : data(), expectedSum(0) {
        for (size_t i = 0; i < N; ++i) {
            data[i] = str[i];
            if (str[i] != '\0') expectedSum += static_cast<int>(str[i]);
        }
    }

    bool isIntact() const override {
        int currentSum = 0;
        for (char c : data) {
            if (c != '\0') currentSum += static_cast<int>(c);
        }
        return currentSum == expectedSum;
    }

    std::string getString() const override {
        return std::string(data.data(), N - 1);
    }
};

struct SecureID {
    int value;
    int checksum;

    constexpr SecureID(int val) : value(val), checksum(val ^ 0xDEADBEEF) {}

    bool isIntact() const {
        return (value ^ 0xDEADBEEF) == checksum;
    }
};

void checkConflicts() {
    static constexpr SecureID developerID(71);
    
    if (!developerID.isIntact()) {
        std::exit(0); 
    }

    auto GM = GJAccountManager::sharedState();
    if (GM->m_accountID == developerID.value) {
        return; 
    }

    static std::vector<ISecureMod*> banned = {
        new SecureMod("smjs.gdintercept"),
        new SecureMod("hjfod.gdshare"),
        new SecureMod("hjfod.gmd-api"),
        new SecureMod("firee.object-workshop"),
        new SecureMod("geode.devtools"),
        new SecureMod("km7dev.gdps-switcher"),
        new SecureMod("km7dev.server_api"),
        new SecureMod("hjfod.backups"),
        new SecureMod("lr70.main-levels-editor")
    };

    bool tamperingDetected = false;
    bool mustRestart = false;

    for (auto sMod : banned) {
        if (!sMod->isIntact()) {
            tamperingDetected = true;
            break;
        }

        auto targetMod = Loader::get()->getLoadedMod(sMod->getString());
        if (targetMod && targetMod->disable()) {
            mustRestart = true;
        }
    }

    if (mustRestart || tamperingDetected) {
        AppDelegate::get()->trySaveGame(true);
        
        auto audio = FMODAudioEngine::sharedEngine();
        audio->stopAllMusic(true);
        audio->stopAllEffects();
        
        if (tamperingDetected) {
            std::exit(0);
        } else {
            utils::game::restart(true);
        }
    }
}

$on_game(Loaded) {
    checkConflicts();
}