#include <Geode/modify/SetupTriggerPopup.hpp>
#include <Geode/modify/CCKeyboardDispatcher.hpp>
#include "multi-edit/MultiEditManager.hpp"
#include "multi-edit/Trigger.hpp"

#include <Geode/Geode.hpp>
using namespace geode::prelude;

class $modify(HUISetupTriggerPopup, SetupTriggerPopup) {
    struct Fields {
        MultiEditManager* multiEditManager;
        bool isHideMode = false;
        Slider* currentSlider = nullptr;
    };

    $override
    bool init(EffectGameObject* trigger, CCArray* triggers, float width, float height, int unkEnum) {
        if (!SetupTriggerPopup::init(trigger, triggers, width, height, unkEnum)) return false;
        if (!Trigger::isTriggerPopup(this)) return true;

        GEODE_UNWRAP_OR_ELSE(mem, err, MultiEditManager::get()) return true;
        m_fields->multiEditManager = mem;

        auto hideBtn = MultiEditManager::createSideMenuButton("hide-btn.png"_spr, this, menu_selector(HUISetupTriggerPopup::toggleHideMode));
        hideBtn->setID("hide-btn"_spr);

        if (typeinfo_cast<SetupShaderEffectPopup*>(this)) {
            hideBtn->toggleWithCallback(true);
        }

        mem->addSideMenuButton(hideBtn);

        return true;
    }

    void toggleHideMode(CCObject* sender) {
        m_fields->isHideMode = !m_fields->isHideMode;
    }

    $override
    void sliderBegan(Slider* slider) {
        m_fields->currentSlider = slider;
        if (!m_fields->isHideMode) return;
        hideOrShowUI(true);
    }

    $override
    void sliderEnded(Slider* slider) {
        m_fields->currentSlider = nullptr;
        if (!m_fields->isHideMode) return;
        hideOrShowUI(false);
    }

    void hideOrShowUI(bool isHidden) {
        Slider* slider = m_fields->currentSlider;
        int sliderProperty = MultiEditManager::getProperty(slider).unwrapOr(-99);
    
        auto& inputBGs = m_fields->multiEditManager->getInputBGs();
        auto& inputLabels = m_fields->multiEditManager->getInputLabels();
        
        auto runOpacity = [isHidden](CCNode* node, GLubyte defaultOpacity = 255) {
            node->runAction(CCFadeTo::create(0.15, isHidden ? 0 : defaultOpacity));
        };

        runOpacity(this, 150);

        CCArray* nodes = CCArray::create();

        nodes->addObjectsFromArray(this->getChildren());
        nodes->addObjectsFromArray(m_mainLayer->getChildren());
        nodes->addObjectsFromArray(m_buttonMenu->getChildren());
        nodes->addObjectsFromArray(m_fields->multiEditManager->getSideMenuButtons());
        
        nodes->removeObject(m_mainLayer);
        nodes->removeObject(m_buttonMenu);
        if (inputBGs.contains(sliderProperty)) nodes->removeObject(inputBGs[sliderProperty]);
        if (inputLabels.contains(sliderProperty)) nodes->removeObject(inputLabels[sliderProperty]);

        // using a traditional for loop so we can iterate over the array while modifying it

        for (size_t i = 0; i < nodes->count(); i++) {
            CCNode* node = typeinfo_cast<CCNode*>(nodes->objectAtIndex(i));
            if (!node) continue;

            if (GEODE_UNWRAP_EITHER(property, err, MultiEditManager::getProperty(node))) {
                if (property == sliderProperty) continue;
            } else {
                if (node->getTag() == sliderProperty) continue;
            }

            // im sorry this is a total mess

            if (auto nodeSlider = typeinfo_cast<Slider*>(node)) {
                if (nodeSlider->getThumb()->getPositionX() != 9999) {
                    runOpacity(nodeSlider->m_groove);
                    runOpacity(nodeSlider->m_sliderBar);
                    runOpacity(nodeSlider->getThumb());
                } else {
                    runOpacity(nodeSlider->m_groove, 100);
                }
            } else if (auto nodeInput = typeinfo_cast<CCTextInputNode*>(node)) {
                runOpacity(nodeInput->m_placeholderLabel);
            } else if (auto nodeBG = typeinfo_cast<CCScale9Sprite*>(node); nodeBG && nodeBG->getTag() != 1) {
                runOpacity(nodeBG, 100);
            } else if (auto nodeBtn = typeinfo_cast<CCMenuItemSpriteExtra*>(node)) {
                auto spr = nodeBtn->getNormalImage();

                if (nodeBtn == m_easingRateButton) {
                    runOpacity(spr, 100);
                    runOpacity(spr->getChildByType<CCLabelBMFont>(0));
                } else if (auto btnSpr = typeinfo_cast<ButtonSprite*>(spr)) {
                    if (auto spr = btnSpr->m_label) runOpacity(spr);
                    if (auto spr = btnSpr->m_BGSprite) runOpacity(spr);
                    if (auto spr = btnSpr->m_subSprite) runOpacity(spr);
                    if (auto spr = btnSpr->m_subBGSprite) runOpacity(spr);
                } else if (auto subSpr = spr->getChildByType<CCSprite>(0)) {
                    runOpacity(spr);
                    runOpacity(subSpr);
                } else {
                    runOpacity(spr);
                }
            } else if (auto nodeToggler = typeinfo_cast<CCMenuItemToggler*>(node)) {
                nodes->addObject(nodeToggler->m_onButton);
                nodes->addObject(nodeToggler->m_offButton);
            } else {
                runOpacity(node);
            }
        }
    }
};

class $modify(CCKeyboardDispatcher) {
    bool dispatchKeyboardMSG(enumKeyCodes key, bool isKeyDown, bool isKeyRepeat) {
        if (!CCKeyboardDispatcher::dispatchKeyboardMSG(key, isKeyDown, isKeyRepeat)) return false;
        if (key != KEY_LeftShift && key != KEY_RightShift) return true;
        if (isKeyRepeat) return true;

        auto popup = Trigger::getTriggerPopup();
        if (!popup) return true;
        if (!Trigger::isTriggerPopup(popup)) return true;

        static_cast<HUISetupTriggerPopup*>(popup)->hideOrShowUI(isKeyDown);

        return true;
    }
};
