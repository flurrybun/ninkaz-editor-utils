#include <Geode/modify/GameObject.hpp>
#include <Geode/modify/EnhancedGameObject.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>

#include <Geode/Geode.hpp>
using namespace geode::prelude;

class $modify(GameObject) {
    struct Fields {
        bool hasAddedGlow = false;
    };

    $override
    void addGlow(gd::string objectFrameName) {
        if (m_fields->hasAddedGlow) return;
        m_fields->hasAddedGlow = true;

        bool prevEE = m_editorEnabled;
        m_editorEnabled = false;

        GameObject::addGlow(objectFrameName);

        m_editorEnabled = prevEE;
    }

    $override
    static GameObject* createWithKey(int key) {
        GameObject* object = GameObject::createWithKey(key);
        if (!object) return object;

        if (object->m_editorEnabled) {
            object->addGlow(ObjectToolbox::sharedState()->intKeyToFrame(key));
        }

        return object;
    }

    $override
    void selectObject(ccColor3B color) {
        // temporarily unset variables which would prevent glow from being colored

        bool prevGULBG = m_glowUsesLighterBG;
        bool prevCCG = m_cantColorGlow;
        m_glowUsesLighterBG = false;
        m_cantColorGlow = false;

        GameObject::selectObject(color);

        m_glowUsesLighterBG = prevGULBG;
        m_cantColorGlow = prevCCG;
    }
};

class $modify(LevelEditorLayer) {
    $override
    void updateVisibility(float dt) {
        LevelEditorLayer::updateVisibility(dt);

        // some objects like orbs, ice spikes, and certain saws don't inherit their glow color from the object color
        // instead, they use a custom color set in PlayLayer::updateVisibility
        // the following is my best attempt at replicating that behavior (yes, i am very proud of this part)

        for (const auto& object : CCArrayExt<GameObject*>(m_objects)) {
            if (auto glowSprite = object->m_glowSprite) {
                glowSprite->setVisible(!object->m_hasNoGlow);
            }

            std::optional<ccColor3B> specialGlowColor = getSpecialGlowColor(object);

            // id 143 is for breakable blocks, which are a unique case
            if (object->m_objectID != 143 &&
                !object->m_glowUsesLighterBG &&
                !specialGlowColor.has_value()
            ) continue;
            if (object->m_isSelected) continue;

            ccColor3B glowColor = {};

            if (specialGlowColor.has_value()) {
                glowColor = specialGlowColor.value();
            } else if (object->m_glowCopiesLBG) {
                if (ColorActionSprite* lbgAction = m_effectManager->m_colorActionSpriteVector[1007]) {
                    glowColor = lbgAction->m_color;
                }
            } else {
                glowColor = m_lighterBGColor;
            }

            bool prevCCG = object->m_cantColorGlow;
            object->m_cantColorGlow = false;

            object->setGlowColor(glowColor);

            object->m_cantColorGlow = prevCCG;
        }
    }

    std::optional<ccColor3B> getSpecialGlowColor(GameObject* object) {
        switch (object->m_objectID) {
            case 36: // yellow orb
            case 35: // yellow pad
                return ccc3(255, 165, 0);
            case 84: // blue orb
            case 67: // blue pad
                return ccc3(0, 255, 255);
            case 141: // pink orb
            case 140: // pink pad
                return ccc3(255, 0, 255);
            case 1333: // red orb
            case 1332: // red pad
                return ccc3(255, 100, 100);
            case 1330: // black orb
            case 1594: // toggle orb
                return m_lighterBGColor;
            case 1022: // green orb
            case 1704: // green dash orb
                return ccc3(25, 255, 25);
            case 1751: // pink dash orb
                return ccc3(200, 0, 255);
            case 3004: // spider orb
            case 3005: // spider pad
                return ccc3(100, 0, 255);
            default:
                return std::nullopt;
        }
    }
};
