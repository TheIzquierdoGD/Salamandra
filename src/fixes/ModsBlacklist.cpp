// Salamandra — Geode v6
// IMPORTANTE: este módulo desactiva mods de terceros y solicita reiniciar el juego.
#include <Geode/Geode.hpp>
#include <Geode/loader/Loader.hpp>

#include <array>
#include <cstdlib>
#include <string>
#include <vector>

using namespace geode::prelude;

namespace {

class ISecureMod {
public:
    virtual ~ISecureMod() = default;
    virtual bool isIntact() const = 0;
    virtual std::string getString() const = 0;
};

template <size_t N>
class SecureMod final : public ISecureMod {
public:
    std::array<char, N> data{};
    int expectedSum = 0;

    constexpr explicit SecureMod(const char (&str)[N]) {
        for (size_t i = 0; i < N; ++i) {
            data[i] = str[i];
            if (str[i] != '\0') {
                expectedSum += static_cast<unsigned char>(str[i]);
            }
        }
    }

    bool isIntact() const override {
        int sum = 0;
        for (char c : data) {
            if (c != '\0') {
                sum += static_cast<unsigned char>(c);
            }
        }
        return sum == expectedSum;
    }

    std::string getString() const override {
        return { data.data(), N - 1 };
    }
};

struct SecureID {
    int value;
    unsigned int checksum;

    constexpr explicit SecureID(int id)
        : value(id), checksum(static_cast<unsigned int>(id) ^ 0xDEADBEEFu) {}

    bool isIntact() const {
        return (static_cast<unsigned int>(value) ^ 0xDEADBEEFu) == checksum;
    }
};

void checkConflicts() {
    static constexpr SecureID developerID{71};

    if (!developerID.isIntact()) {
        log::error("Salamandra: error de integridad del ID de desarrollador");
        std::exit(EXIT_FAILURE);
    }

    auto* account = GJAccountManager::sharedState();
    if (account && account->m_accountID == developerID.value) {
        return;
    }

    // Conserva la lista de incompatibilidades del código original.
    static std::vector<ISecureMod*> const banned = {
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

    for (auto const* entry : banned) {
        if (!entry->isIntact()) {
            tamperingDetected = true;
            break;
        }

        auto name = entry->getString();
        auto* mod = Loader::get()->getLoadedMod(name);
        if (!mod) continue;

        // Mod::disable() devuelve Result<> en Geode v6.
        auto result = mod->disable();
        if (result) {
            log::info("Salamandra: mod desactivado: {}", name);
            mustRestart = true;
        } else {
            log::warn("Salamandra: no se pudo desactivar {}: {}", name, result.unwrapErr());
        }
    }

    if (!mustRestart && !tamperingDetected) return;

    if (auto* app = AppDelegate::get()) {
        app->trySaveGame(true);
    }
    if (auto* audio = FMODAudioEngine::sharedEngine()) {
        audio->stopAllMusic(true);
        audio->stopAllEffects();
    }

    if (tamperingDetected) {
        log::error("Salamandra: integridad de la lista inválida");
        std::exit(EXIT_FAILURE);
    }

    // Geode v6: restart(bool saveData, bool safeMode).
    utils::game::restart(true, false);
}

} // namespace

$on_game(Loaded) {
    checkConflicts();
}
