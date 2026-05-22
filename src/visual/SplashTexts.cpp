// Splash Texts [Main menu]
#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/LoadingLayer.hpp>
#include <random>

using namespace geode::prelude;

std::vector<std::string> splashTexts = {
    "Nuevo inicio, nuevos servidores.",
    "Resubir niveles sin permiso esta prohibido.",
    "Never gonna give you up, never gonna let you down.",
    "Peru es clave. Somos Peru.",
    "A please burguer chesse.",
    "Nuevo inicio, nuevos servidores.",
    "Omae wa mou shindeiru, Nani?",
    "Te gusta Salamandra? Entonces prueba Star Cheese.",
    "Descargando los virus...",
    "Sal que te paso a buscar y nos vamos a c****r a la orilla del mar.",
    "Geode definitivamente no aceptaria este mod.",
    "Six Seven. Six Seven. Six Seven.",
    "Multiplicarnos. Asi como Cristo multiplico, los p***s, perdon los peces.",
    "EA Sports. You are gay?",
    "Ah s**t, here we go again...",
    "The roses are red. Violets are blue. This isn't the 2.3",
    "Viva el imperio Peruano y Lima la capital del mundo.",
    "Da Dudu Da Dudu. >>>Tidal Wave<<<",
    "Is possible Salamandra with accurate hitboxes? -Geo-",
    "Es que dijeron p**o al que no haga un GDPS en ingles",
    "The World's Smallest Violin.",
    "Moon x Alex? I don't know, ask their owners.",
    "Me voy para Lema. Voy buscash fortuna. -La Paisana Jacinta-"
};

std::string getRandomSplashExcept(const std::string& exclude) {
    if (splashTexts.size() <= 1) return exclude;
    static std::mt19937 rng(std::random_device{}());
    std::string result;
    do {
        std::uniform_int_distribution<size_t> dist(0, splashTexts.size() - 1);
        result = splashTexts[dist(rng)];
    } while (result == exclude);
    return result;
}

std::string getRandomSplash() {
    static std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<size_t> dist(0, splashTexts.size() - 1);
    return splashTexts[dist(rng)];
}


class $modify(MyLoadingLayer, LoadingLayer) {
    bool init(bool fromReload) {
        if (!LoadingLayer::init(fromReload)) return false;

        auto textArea = typeinfo_cast<TextArea*>(getChildByID("text-area"));
        if (!textArea) return true;

        textArea->setVisible(false);

        std::string line1 = getRandomSplash();
        std::string line2 = getRandomSplashExcept(line1);

        auto basePos = textArea->getPosition();

        auto label1 = CCLabelBMFont::create(line1.c_str(), "goldFont.fnt");
        label1->setPosition(basePos + ccp(0, 18));
        label1->limitLabelWidth(420.f, textArea->getScale(), 0.25f);
        this->addChild(label1);

        auto label2 = CCLabelBMFont::create(line2.c_str(), "bigFont.fnt");
        label2->setPosition(basePos + ccp(0, 0));
        label2->limitLabelWidth(420.f, textArea->getScale(), 0.25f);
        this->addChild(label2);

        return true;
    }
};


bool menuSplashInit = false;
int splashIndex1 = 0;
int splashIndex2 = 1;

class $modify(MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;

        if (!menuSplashInit) {
            std::mt19937 gen(std::random_device{}());
            std::uniform_int_distribution<> dis(0, splashTexts.size() - 1);
            splashIndex1 = dis(gen);
            do {
                splashIndex2 = dis(gen);
            } while (splashIndex2 == splashIndex1);
            menuSplashInit = true;
        }

        auto winSize = CCDirector::sharedDirector()->getWinSize();

// Row 1
auto label1 = CCLabelBMFont::create(splashTexts[splashIndex1].c_str(), "goldFont.fnt");
label1->setScale(0.40f);
label1->setPosition({ winSize.width / 2, 110.f });
label1->setZOrder(15);
label1->setID("splash-menu-1");
label1->runAction(CCRepeatForever::create(
    CCSequence::create(
        CCScaleTo::create(0.6f, 0.45f),
        CCScaleTo::create(0.6f, 0.40f),
        nullptr
    )
));
addChild(label1);

// Row 2
auto label2 = CCLabelBMFont::create(splashTexts[splashIndex2].c_str(), "bigFont.fnt");
label2->setScale(0.40f);
label2->setPosition({ winSize.width / 2, 100.f });
label2->setZOrder(15);
label2->setID("splash-menu-2");
label2->runAction(CCRepeatForever::create(
    CCSequence::create(
        CCScaleTo::create(0.6f, 0.45f),
        CCScaleTo::create(0.6f, 0.40f),
        nullptr
    )
));
addChild(label2);

        return true;
    }
};

// Splash Texts [End menu]
#include <Geode/Geode.hpp>
#include <Geode/modify/EndLevelLayer.hpp>
#include <random>
#include <sstream>

using namespace geode::prelude;

std::vector<std::string> splashTextsEnd = {
    "Bien... Ahora pasate Everlasting.",
    "Muy bien para alguien que juega con las patas.",
    "Deberias replanterarte por que juegaste esto.",
    "Me aburro.",
    "Izi pizi.",
    "Hackeadme esta!",
    "You are the GOAT.",
    "SUS, Amogus.",
    "My name is John Cena"
};

std::string getRandomEndSplash() {
    static std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<size_t> dist(0, splashTextsEnd.size() - 1);
    return splashTextsEnd[dist(rng)];
}

std::string wrapText(const std::string& text, size_t limit) {
    std::stringstream ss(text);
    std::string word, result = "", currentLine = "";
    while (ss >> word) {
        if (word.length() > limit) {
            if (!currentLine.empty()) { result += currentLine + "\n"; currentLine = ""; }
            while (word.length() > limit) {
                result += word.substr(0, limit - 1) + "-\n";
                word = word.substr(limit - 1);
            }
            currentLine = word;
        }
        else if (currentLine.length() + word.length() + 1 <= limit) {
            if (!currentLine.empty()) currentLine += " ";
            currentLine += word;
        } 
        else {
            result += currentLine + "\n";
            currentLine = word;
        }
    }
    return result + currentLine;
}

class $modify(MyEndLevelLayer, EndLevelLayer) {
    void customSetup() {
        EndLevelLayer::customSetup();

        auto mainLayer = m_mainLayer;
        if (!mainLayer) return;

        auto endTextLabel = typeinfo_cast<CCLabelBMFont*>(mainLayer->getChildByID("end-text"));
        
        if (endTextLabel) {
            std::string rawText = getRandomEndSplash();
            std::string wrapped = wrapText(rawText, 26);
            
            endTextLabel->setString(wrapped.c_str());
            endTextLabel->setAlignment(CCTextAlignment::kCCTextAlignmentCenter);

            int lines = 1;
            for (char c : wrapped) if (c == '\n') lines++;

            float finalScale = 0.62f; 

            if (lines == 1) {
                if (rawText.length() < 12) {
                    finalScale = 1.0f;
                } else {

                    float t = (float)(rawText.length() - 12) / 14.0f;
                    finalScale = 1.0f - (t * 0.4f); 
                }
            } 
            else {
                finalScale = 0.62f;
            }

            endTextLabel->setScale(finalScale);
        }
    }
};