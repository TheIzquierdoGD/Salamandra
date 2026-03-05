#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/LoadingLayer.hpp>
#include <random>

using namespace geode::prelude;

/////////////////////////
// FRASES COMPARTIDAS
/////////////////////////

std::vector<std::string> splashTexts = {
    "*No hace nada para revivir. Momento Salamandra.",
    "Mira bien hijo de tu pta madre. Soy mencho.",
    "Solo 7 palabras ESENCIA. *Bring me to life song",
    "Hola amigos mi nombre es Giovani y hoy hablare de Lacky TIM.",
    "Salamandra. El mejor GDPS de la historia. Tiemblen.",
    "Star Cheese x Salamandra = Panas [Se aman en secreto XD]",
    "Te amo shionsito miamor <3 (Sera femboy OwO?)",
    "Inversion Saudi. Bombardeen Iran y palestina.",
    "Darent el verdadero traicionador.",
    "Yo siempre fui el verdadero lider de la traicion.",
    "Rakiten vs Darent (La batalla de negros)",
    "Me voi para la Lema. Voi buscash foltuna.",
    "Meow! -Un random que no descargo el GDPS-",
    "Se que sos un nene malo. Aja. Un nene malo. Si si.",
    "Dixonsaurio mi vida para siempre.",
    "No hay ceguera peor que la del low datail mode.",
    "Muerte a los 50 amigos y a toda su gente.",
    "VIVA EL IMPERIO PERUANO Y LIMA LA CAPITAL DEL MUNDO.",
    "La gente normal lee estos textos?",
    "Recuerda que si te dejan es porque el otro no es ciego.",
    "Salamandra fusion en 20026 :D! Falta poco...",
    "Voy a dejar que Deidara tome el control de mi mente.",
    "Everlasting X Yuu = Noche estrellada?",
    "Don Cheeto siempre fue canon y no lo sabia.",
    "Recuerda: 2 Lunas son mejor que una :3 -CrazyMoon-",
    "La Pluie no es real o si? Preguntale a Yuu.",
    "ID clave robada exitosamente >:0",
    "AY POTO ROTO!!! [La ID 2000 En corto]",
    "Esta ranchera es para el SR GIOVANI CUEVAS.",
    "Deidara solo es Deidara."
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

////////////////////////
/////MENÚ PRINCIPAL/////
////////////////////////

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

// Frase 1 (arriba)
auto label1 = CCLabelBMFont::create(splashTexts[splashIndex1].c_str(), "goldFont.fnt");
label1->setScale(0.40f);
label1->setPosition({ winSize.width / 2, 110.f });
label1->setZOrder(15);
label1->setID("splash-menu-1");
label1->runAction(CCRepeatForever::create(
    CCSequence::create(
        CCScaleTo::create(0.6f, 0.45f), // más pequeño
        CCScaleTo::create(0.6f, 0.40f),
        nullptr
    )
));
addChild(label1);

// Frase 2 (abajo, pegada)
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

#include <Geode/Geode.hpp>
#include <Geode/modify/EndLevelLayer.hpp>
#include <random>

using namespace geode::prelude;

#include <Geode/Geode.hpp>
#include <Geode/modify/EndLevelLayer.hpp>
#include <random>

using namespace geode::prelude;

/////////////////////////////////////////////////////////////////
// FRASES EXCLUSIVAS PARA LA PANTALLA FINAL (Nivel Completo)
/////////////////////////////////////////////////////////////////

std::vector<std::string> splashTextsEnd = {
    "Probando la paciencia del jugador.",
    "Otra vez aqui? No tienes nada mejor que hacer?",
    "Te has planteado salir a dar un paseo?",
    "Sabias que los gatos tienen cinco dedos en las patas delanteras?",
    "Por que no pruebas a contar hasta diez?",
    "Has considerado aprender a tocar un instrumento musical?",
    "Sabias que el corazon de una ballena azul es tan grande como un coche?",
    "Por que no intentas hacer un dibujo?"
};

std::string getRandomEndSplash() {
    static std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<size_t> dist(0, splashTextsEnd.size() - 1);
    return splashTextsEnd[dist(rng)];
}

/////////////////////////////////////////////////////////////////
// LÓGICA DE LA PANTALLA FINAL (Estilo Original)
/////////////////////////////////////////////////////////////////

class $modify(MyEndLevelLayer, EndLevelLayer) {
    void customSetup() {
        EndLevelLayer::customSetup();

        auto mainLayer = m_mainLayer;
        if (!mainLayer) return;

        // Buscamos el label de texto informativo (end-text)
        auto endTextLabel = typeinfo_cast<CCLabelBMFont*>(mainLayer->getChildByID("end-text"));
        
        if (endTextLabel) {
            // Cambiamos el texto por una de tus frases
            endTextLabel->setString(getRandomEndSplash().c_str());
            
            // Limitamos el ancho para que se encoja si es muy larga
            // El primer valor (300.f) es el ancho máximo antes de encogerse.
            endTextLabel->limitLabelWidth(300.f, 0.5f, 0.1f);
        }
    }
};