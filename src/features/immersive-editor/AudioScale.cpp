#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>

#include <Geode/Geode.hpp>
using namespace geode::prelude;

class $modify(LevelEditorLayer) {
    struct Fields {
        int prevAudioTrack = -1;
        int prevSongID = -1;
    };

    $override
    bool init(GJGameLevel* p0, bool p1) {
        if (!LevelEditorLayer::init(p0, p1)) return false;

        FMODAudioEngine::get()->enableMetering();

        return true;
    }

    $override
    void levelSettingsUpdated() {
        LevelEditorLayer::levelSettingsUpdated();

        bool hasAudioChanged = m_fields->prevAudioTrack != m_level->m_audioTrack ||
            m_fields->prevSongID != m_level->m_songID;

        m_fields->prevAudioTrack = m_level->m_audioTrack;
        m_fields->prevSongID = m_level->m_songID;

        if (!hasAudioChanged) return;

        if (m_audioEffectsLayer) {
            m_audioEffectsLayer->removeFromParent();
            m_audioEffectsLayer = nullptr;
        }

        if (m_level->m_songID == 0) {
            m_audioEffectsLayer = AudioEffectsLayer::create(LevelTools::getAudioString(m_level->m_audioTrack));
            m_objectLayer->addChild(m_audioEffectsLayer);
        }
    }

    $override
    void updateVisibility(float dt) {
        LevelEditorLayer::updateVisibility(dt);

        if (m_playbackMode == PlaybackMode::Not && !m_editorUI->m_isPlayingMusic) {
            for (const auto& object : CCArrayExt<GameObject*>(m_objects)) {
                object->setRScale(1.0f);
            }

            return;
        }

        if (m_audioEffectsLayer) {
            GameManager::get()->m_playLayer = reinterpret_cast<PlayLayer*>(GJBaseGameLayer::get());
            m_audioEffectsLayer->audioStep(dt);
            GameManager::get()->m_playLayer = nullptr;
        }

        float audioScale = m_audioEffectsLayer
            ? m_audioEffectsLayer->m_audioScale
            : FMODAudioEngine::get()->getMeteringValue();

        for (const auto& object : CCArrayExt<GameObject*>(m_objects)) {
            if (!object->m_isAudioScale || object->m_hasNoAudioScale) continue;

            // orbs have their own audio scale logic in RingObject::setRScale, which only runs when
            // m_editorEnabled is false? otherwise it's the same as GameObject::setRScale. weird

            object->m_editorEnabled = false;

            if (object->m_customAudioScale) {
                float min = object->m_minAudioScale;
                float max = object->m_maxAudioScale;

                object->setRScale(min + audioScale * (max - min));
            } else {
                object->setRScale(audioScale);
            }

            object->m_editorEnabled = true;
        }
    }
};

class $modify(GJBaseGameLayer) {
    // i can't hook ~LevelEditorLayer() but this works just fine

    $override
    void destructor() {
        GJBaseGameLayer::~GJBaseGameLayer();
        FMODAudioEngine::get()->disableMetering();
    }
};
