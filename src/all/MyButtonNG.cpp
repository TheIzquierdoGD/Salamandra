#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>

using namespace geode::prelude;

class $modify(NewgroundsRequest, MenuLayer) {
    bool init() {
        if (!MenuLayer::init())
            return false;

        auto bottomMenu = this->getChildByID("bottom-menu");
        if (!bottomMenu)
            return true;

        auto ngBtn = typeinfo_cast<CCMenuItemSpriteExtra*>(
            bottomMenu->getChildByID("newgrounds-button")
        );

        if (ngBtn) {
            // Forma compatible con Geode 5.x
            ngBtn->m_pListener = this;
            ngBtn->m_pfnSelector = menu_selector(NewgroundsRequest::onRequestForm);
        }

        return true;
    }

    void onRequestForm(CCObject*) {
        geode::createQuickPopup(
            "Request Formulary",
            "Would you like to become a verified player? :D",
            "Cancel",
            "Open",
            [](auto, bool accepted) {
                if (accepted) {
                    geode::utils::web::openLinkInBrowser(
                        "https://docs.google.com/forms/d/e/1FAIpQLSfdltkrpTdTrJWFwBoZQM8blkyjW0blGnLRE6rnXLbICLEpog/viewform"
                    );
                }
            }
        );
    }
};