#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/EffectGameObject.hpp>

#include <Geode/Geode.hpp>
using namespace geode::prelude;

class $modify(SGLevelEditorLayer, LevelEditorLayer) {
    struct Fields {
        bool bgEffectEnabled = true;
    };

    $override
    bool init(GJGameLevel* p0, bool p1) {
        if (!LevelEditorLayer::init(p0, p1)) return false;

        m_glitterParticles = CCParticleSystemQuad::create("glitterEffect.plist", false);

        m_objectLayer->addChild(m_glitterParticles);
        m_glitterParticles->setVisible(false);
        m_glitterParticles->stopSystem();

        ccColor4F color = to4F(to4B(m_player1->m_playerColor1));
        m_glitterParticles->setStartColor(color);
        m_glitterParticles->setEndColor(color);

        m_glitterParticles->setPositionType(kCCPositionTypeRelative);

        return true;
    }

    $override
    void postUpdate(float dt) {
        LevelEditorLayer::postUpdate(dt);

        CCSize winSize = CCDirector::get()->getWinSize();
        CCPoint cameraCenter = winSize * 0.5f / m_gameState.m_cameraZoom;

        m_glitterParticles->setPosition(cameraCenter + m_gameState.m_cameraPosition);
    }

    $override
    void onPlaytest() {
        LevelEditorLayer::onPlaytest();

        m_fields->bgEffectEnabled = true;
        m_glitterParticles->setVisible(true);
    }

    $override
    void onStopPlaytest() {
        LevelEditorLayer::onStopPlaytest();

        m_glitterParticles->stopSystem();
        m_glitterParticles->setVisible(false);
    }

    void setGlitterVisible(bool visible) {
        if (m_fields->bgEffectEnabled && visible) {
            m_glitterParticles->resumeSystem();
        } else {
            m_glitterParticles->stopSystem();
        }
    }

    void setBGEffectEnabled(bool enabled) {
        m_fields->bgEffectEnabled = enabled;

        bool isFlying = m_gameState.m_isDualMode
            ? m_player1->isFlying() || m_player2->isFlying()
            : m_player1->isFlying();

        if (isFlying) setGlitterVisible(enabled);
    }
};

class $modify(PlayerObject) {
    $override
    void updatePlayerArt() {
        // this function gets called redundantly but i do not care
        // the gamemode switching logic in PlayerObject is so fucked

        PlayerObject::updatePlayerArt();

        if (!LevelEditorLayer::get() || GameManager::get()->m_performanceMode) return;
        auto modLEL = static_cast<SGLevelEditorLayer*>(LevelEditorLayer::get());

        modLEL->setGlitterVisible(isFlying());
    }
};

class $modify(EffectGameObject) {
    $override
    void triggerObject(GJBaseGameLayer* p0, int p1, gd::vector<int> const* p2) {
        EffectGameObject::triggerObject(p0, p1, p2);

        if (!LevelEditorLayer::get() || GameManager::get()->m_performanceMode) return;
        auto modLEL = static_cast<SGLevelEditorLayer*>(LevelEditorLayer::get());

        switch (m_objectID) {
            case 1818: // bg effect on
                modLEL->setBGEffectEnabled(true);
                break;
            case 1819: // bg effect off
                modLEL->setBGEffectEnabled(false);
                break;
        }
    }
};
