#include <Geode/modify/SetupRotatePopup.hpp>
#include "multi-edit/MultiEditManager.hpp"

#include <Geode/Geode.hpp>
using namespace geode::prelude;

enum class RotateAction {
    Default = 0,
    Custom = 2,
    Disable = 1
};

class $modify(PRSetupRotatePopup, SetupRotatePopup) {
    struct Fields {
        CCSprite* m_previewObject;
        CCLayerColor* m_controlCover;

        float m_defaultRotationSpeed;
    };

    $override
    bool init(EnhancedGameObject* obj, CCArray* objs) {
        if (!SetupRotatePopup::init(obj, objs)) return false;

        // PREVIEW OBJECT BACKGROUND

        auto previewBG = CCScale9Sprite::create("square02_001.png", {0, 0, 80, 80});
        previewBG->setOpacity(50);
        previewBG->setPosition({205, 140});
        previewBG->setContentSize({80, 80});
        m_mainLayer->addChild(previewBG);

        // PREVIEW OBJECT

        auto previewObjString = fmt::format("1,{},2,0,3,0;", getObjectID(obj, objs));
        CCArray* previewObjs = CCArray::create();

        CCSprite* previewObj = EditorUI::get()->spriteFromObjectString(
            previewObjString, false, true, 1, previewObjs, nullptr, nullptr
        );

        if (auto obj = typeinfo_cast<EnhancedGameObject*>(previewObjs->firstObject())) {
            m_fields->m_defaultRotationSpeed = obj->m_rotationAngle;
            if (rand() % 2 == 0) m_fields->m_defaultRotationSpeed *= -1;

            // if (auto baseColor = obj->m_baseColor) {
            //     baseColor->m_colorID = 1011;
            // }

            // if (auto detailColor = obj->m_detailColor; detailColor && detailColor->m_defaultColorID == 1005) {
            //     detailColor->m_colorID = 1011;
            // }

            normalizeSpriteColor(obj->m_baseColor);
            normalizeSpriteColor(obj->m_detailColor);
        }

        LevelEditorLayer::get()->updateObjectColors(previewObjs);

        float scale = 0.7f;
        auto objSize = previewObj->getContentSize();

        if (previewObj->getContentWidth() > 60.f || previewObj->getContentHeight() > 60.f) {
            float scaleX = 60.f / previewObj->getContentWidth();
            float scaleY = 60.f / previewObj->getContentHeight();
            scale = std::min(scaleX, scaleY);
        }

        previewObj->setScale(scale);
        previewBG->addChildAtPosition(previewObj, Anchor::Center);
        m_fields->m_previewObject = previewObj;

        // MOVE CONTROLS TO THE RIGHT & REMOVE FROM GROUP

        CCPoint inputOffset = {50, 0};
        CCArray* rotationControls = static_cast<CCArray*>(m_groupContainers->objectAtIndex(2));

        for (const auto& node : CCArrayExt<CCNode>(rotationControls)) {
            node->setPosition(node->getPosition() + inputOffset);
            node->setVisible(true);
        }

        m_groupContainers->removeObjectAtIndex(2);

        // OVERLAY OVER CONTROLS WHEN ROTATION WHEN DISABLED

        auto controlCover = CCLayerColor::create(ccc4(153, 85, 51, 150), 170, 63);
        controlCover->setPosition({252, 111});
        controlCover->setZOrder(21);
        m_mainLayer->addChild(controlCover);
        m_fields->m_controlCover = controlCover;

        // SET INITIAL STATE

        RotateAction initialAction = getInitialAction();

        if (auto toggler = getToggler(initialAction)) {
            toggler->toggleWithCallback(true);
        }

        if (initialAction == RotateAction::Custom) {
            float rotationSpeed = getRotationSpeed();

            if (rotationSpeed != 0) {
                updateValueControls(97, rotationSpeed);
            }
        }

        schedule(schedule_selector(PRSetupRotatePopup::updatePreviewRotation));

        return true;
    }

    $override
    void onCustomToggleTriggerValue(CCObject* sender) {
        // SetupRotatePopup::onCustomToggleTriggerValue(sender);

        if (sender->getTag() == static_cast<int>(RotateAction::Default)) {
            valueChanged(97, 0);
            valueChanged(98, 0);
        } else if (sender->getTag() == static_cast<int>(RotateAction::Disable)) {
            valueChanged(97, 0);
            valueChanged(98, 1);
        } else if (sender->getTag() == static_cast<int>(RotateAction::Custom)) {
            float rotationSpeed = getRotationSpeed();
            if (rotationSpeed != 0) valueChanged(97, rotationSpeed);

            valueChanged(98, 0);
        }

        for (const auto& [key, toggler] : CCDictionaryExt<int, CCMenuItemToggler*>(m_customValueToggles)) {
            toggler->toggle(key == sender->getTag());
        }

        m_fields->m_controlCover->setVisible(
            !getToggler(RotateAction::Custom)->isToggled()
        );

        if (!sender || sender->getTag() == static_cast<int>(RotateAction::Custom)) return;
        GEODE_UNWRAP_OR_ELSE(mem, err, MultiEditManager::get()) return;
        if (!mem->getMixedButtons()[97]) return;

        mem->removeMixed(97, 0);
    }

    RotateAction getInitialAction() {
        if (m_gameObject) {
            if (m_gameObject->m_disableRotation) return RotateAction::Disable;
            if (m_gameObject->m_rotationSpeed == 0) return RotateAction::Default;
            return RotateAction::Custom;
        }

        if (!m_gameObjects) return RotateAction::Default;

        CCArrayExt<EnhancedGameObject> objs = m_gameObjects;
        bool allCustomSpeed = true;
        bool allDisabledRotation = true;

        for (const auto& obj : objs) {
            if (obj->m_rotationSpeed == 0) allCustomSpeed = false;
            if (!obj->m_disableRotation) allDisabledRotation = false;
        }

        if (allDisabledRotation) return RotateAction::Disable;
        if (allCustomSpeed) return RotateAction::Custom;

        return RotateAction::Default;
    }

    float getRotationSpeed() {
        if (m_gameObject) return m_gameObject->m_rotationSpeed;
        if (!m_gameObjects) return 0;

        CCArrayExt<EnhancedGameObject> objs = m_gameObjects;
        float rotationSpeed = objs[0]->m_rotationSpeed;

        for (const auto& obj : objs) {
            if (obj->m_rotationSpeed != rotationSpeed) return 0;
        }

        return rotationSpeed;
    }

    bool getDisableRotation() {
        if (m_gameObject) return m_gameObject->m_disableRotation;
        if (!m_gameObjects) return false;

        CCArrayExt<EnhancedGameObject> objs = m_gameObjects;
        bool disableRotation = objs[0]->m_disableRotation;

        for (const auto& obj : objs) {
            if (obj->m_disableRotation != disableRotation) return false;
        }

        return disableRotation;
    }

    void updatePreviewRotation(float dt) {
        auto obj = m_fields->m_previewObject;

        float rotationSpeed = getRotationSpeed();
        bool disableRotation = getDisableRotation();

        if (disableRotation) {
            obj->setRotation(0);
        } else {
            if (rotationSpeed == 0) rotationSpeed = m_fields->m_defaultRotationSpeed;

            float newRotation = obj->getRotation() + rotationSpeed * dt;
            obj->setRotation(newRotation);
        }
    }

    int getObjectID(EnhancedGameObject* obj, CCArray* objs) {
        if (obj) return obj->m_objectID;
        if (!objs) return 1705; // default saw

        std::unordered_map<int, int> idCounts;
        int mostCommonID = 1705;
        int maxCount = 0;

        for (const auto& obj : CCArrayExt<EnhancedGameObject>(objs)) {
            int id = obj->m_objectID;
            idCounts[id]++;

            if (idCounts[id] > maxCount) {
                maxCount = idCounts[id];
                mostCommonID = id;
            }
        }
    
        return mostCommonID;
    }

    void normalizeSpriteColor(GJSpriteColor* color) {
        if (!color) return;

        switch (color->m_defaultColorID) {
            case 1005: // P1
            case 1006: // P2
            case 1007: // LBG
                color->m_colorID = 1011; // set to white
        }
    }

    CCMenuItemToggler* getToggler(RotateAction action) {
        auto obj = m_customValueToggles->objectForKey(static_cast<int>(action));
        return typeinfo_cast<CCMenuItemToggler*>(obj);
    }
};
