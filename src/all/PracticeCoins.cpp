//Coins In Practice Mode
#include <Geode/Geode.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>

using namespace geode::prelude;

class $modify(GJBaseGameLayer) {
    void collisionCheckObjects(PlayerObject* p0, gd::vector<GameObject*>* p1, int p2, float p3) {
        if (m_isPracticeMode && p0 && p1) {
            for (size_t i = 0; i < p1->size(); i++) {
                auto obj = p1->at(i);

                if (p0->getObjectRect().intersectsRect(obj->getObjectRect())) {
                    if (obj->m_objectType == GameObjectType::SecretCoin || obj->m_objectType == GameObjectType::UserCoin) {
                        if (obj->getOpacity() != 0)
                            destroyObject(obj);
                    }
                }
            }
        }

        // Llamar a la función original
        GJBaseGameLayer::collisionCheckObjects(p0, p1, p2, p3);
    }
};