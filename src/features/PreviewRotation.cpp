#include <Geode/modify/SetupRotatePopup.hpp>
#include "multi-edit/MultiEditContext.hpp"
#include "../misc/SpriteColor.hpp"

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
        CCTextInputNode* m_input;
        Slider* m_slider;

        float m_defaultRotationSpeed;
    };

    $override
    bool init(EnhancedGameObject* obj, CCArray* objs) {
        if (!SetupRotatePopup::init(obj, objs)) return false;

        CCSize winCenter = CCDirector::get()->getWinSize() / 2;

        // PREVIEW OBJECT BACKGROUND

        auto previewBG = CCScale9Sprite::create("square02_001.png", {0, 0, 80, 80});
        previewBG->setOpacity(50);
        previewBG->setPosition(winCenter + ccp(-80, -20));
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

            nk::normalizeSpriteColor(obj->m_baseColor);
            nk::normalizeSpriteColor(obj->m_detailColor);
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

            if (auto input = typeinfo_cast<CCTextInputNode*>(node)) {
                m_fields->m_input = input;
            } else if (auto slider = typeinfo_cast<Slider*>(node)) {
                m_fields->m_slider = slider;
            }
        }

        m_groupContainers->removeObjectAtIndex(2);

        // OVERLAY OVER CONTROLS WHEN ROTATION WHEN DISABLED

        auto controlCover = CCLayerColor::create(ccc4(153, 85, 51, 150), 170, 63);
        controlCover->setPosition(winCenter + ccp(-32, -49));
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

        bool enableInput = getToggler(RotateAction::Custom)->isToggled();

        m_fields->m_controlCover->setVisible(!enableInput);
        m_fields->m_input->setTouchEnabled(enableInput);
        m_fields->m_slider->m_touchLogic->setEnabled(enableInput);
        m_fields->m_slider->m_enabled = enableInput;

        // unfocus input
        m_fields->m_input->onClickTrackNode(false);

        if (!sender || sender->getTag() == static_cast<int>(RotateAction::Custom)) return;

        auto ctx = MultiEditContext::get(this);

        if (ctx && ctx->getMixedButton(97)) {
            ctx->onMixedInputApplied(97, 0);
        }
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

    CCMenuItemToggler* getToggler(RotateAction action) {
        auto obj = m_customValueToggles->objectForKey(static_cast<int>(action));
        return typeinfo_cast<CCMenuItemToggler*>(obj);
    }
};
