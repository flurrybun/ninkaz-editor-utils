#include <Geode/modify/GameObject.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>

#include <Geode/Geode.hpp>
using namespace geode::prelude;

class $modify(GameObject) {
    $override
    void addGlow(gd::string objectFrameName) {
        if (!LevelEditorLayer::get()) {
            GameObject::addGlow(objectFrameName);
            return;
        }

        if (GameManager::get()->m_performanceMode) return;

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

        ccColor3B lbgColor = {};
        if (ColorActionSprite* lbgAction = m_effectManager->m_colorActionSpriteVector[1007]) {
            lbgColor = lbgAction->m_color;
        }

        float screenRight = CCDirector::get()->getScreenRight();
        float playerX = screenRight * 0.5 - 75;

        for (const auto& object : CCArrayExt<GameObject*>(m_objects)) {
            updateCustomGlowColor(object);
            updateFadingBlock(object, playerX, screenRight, lbgColor);
        }
    }

    void updateCustomGlowColor(GameObject* object) {
        // some objects like orbs, ice spikes, and certain saws don't inherit their glow color from the object color
        // instead, they use a custom color updated per frame in PlayLayer::updateVisibility

        if (auto glowSprite = object->m_glowSprite) {
            glowSprite->setVisible(!object->m_hasNoGlow);
        }

        if (object->m_isSelected) return;

        std::optional<ccColor3B> specialGlowColor = getSpecialGlowColor(object);

        // id 143 is for breakable blocks, which are a special case
        if (object->m_objectID != 143 &&
            !object->m_glowUsesLighterBG &&
            !specialGlowColor.has_value()
        ) return;

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

    void updateFadingBlock(GameObject* object, float playerX, float screenRight, ccColor3B lbgColor) {
        if (!object->m_isFadingBlock || !m_previewMode) return;

        if (object->m_isSelected) {
            object->setOpacity(255);
            return;
        }

        // updateInvisibleBlock uses m_cameraPosition2 to get the camera's position,
        // which isn't updated properly in the editor

        CCPoint prevECP = m_gameState.m_cameraPosition2;
        m_gameState.m_cameraPosition2 = -m_objectLayer->getPosition() / m_objectLayer->getScale();

        // what the fuck are these params rob

        reinterpret_cast<PlayLayer*>(GJBaseGameLayer::get())->updateInvisibleBlock(
            object, playerX + 110, playerX, screenRight - (playerX + 110) - 90, playerX - 30, lbgColor
        );

        m_gameState.m_cameraPosition2 = prevECP;
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
