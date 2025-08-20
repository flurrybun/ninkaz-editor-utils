#include <Geode/modify/GameObject.hpp>
#include <Geode/modify/EffectGameObject.hpp>

#include <Geode/Geode.hpp>
using namespace geode::prelude;

class $modify(GameObject) {
    $override
    void customSetup() {
        GameObject::customSetup();

        if (m_isTrigger) {
            m_activateInEditor = true;
        }
    }
};
