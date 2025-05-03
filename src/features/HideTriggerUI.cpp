#include <Geode/modify/SetupTriggerPopup.hpp>
#include <Geode/modify/CreateParticlePopup.hpp>
#include "multi-edit/MultiEditManager.hpp"
#include "multi-edit/Trigger.hpp"

#include <Geode/Geode.hpp>
using namespace geode::prelude;

void runOpacity(CCNode* node, bool isHidden) {
    if (!node) return;

    auto protocol = typeinfo_cast<CCRGBAProtocol*>(node);
    if (!protocol) return;

    GLubyte opacity = 255;

    if (!node->getUserObject("opacity"_spr)) {
        node->setUserObject("opacity"_spr, CCInteger::create(protocol->getOpacity()));
    }

    if (isHidden) {
        opacity = 0;
    } else {
        if (auto opacityObj = typeinfo_cast<CCInteger*>(node->getUserObject("opacity"_spr))) {
            opacity = opacityObj->getValue();
        }
    }

    if (opacity == protocol->getOpacity()) return;
    node->runAction(CCFadeTo::create(0.15, opacity));
}

void recursiveOpacity(CCNode* node, bool isHidden, CCArray* nodesToIgnore) {
    if (!node) return;
    if (nodesToIgnore->containsObject(node)) return;

    bool shouldRecurse = true;

    if (auto nodeSprite = typeinfo_cast<CCSprite*>(node)) {
        runOpacity(nodeSprite, isHidden);
    } else if (auto nodeSlider = typeinfo_cast<Slider*>(node)) {
        shouldRecurse = false;

        runOpacity(nodeSlider->m_groove, isHidden);
        runOpacity(nodeSlider->m_sliderBar, isHidden);
        runOpacity(nodeSlider->getThumb(), isHidden);
    } else if (auto nodeInput = typeinfo_cast<CCTextInputNode*>(node)) {
        shouldRecurse = false;

        runOpacity(nodeInput->m_placeholderLabel, isHidden);
    } else if (auto nodeBG = typeinfo_cast<CCScale9Sprite*>(node)) {
        shouldRecurse = false;

        runOpacity(nodeBG, isHidden);

        for (auto child : CCArrayExt<CCNode>(node->getChildren())) {
            if (child == nodeBG->_scale9Image) continue;
            recursiveOpacity(child, isHidden, nodesToIgnore);
        }
    } else if (auto nodePPL = typeinfo_cast<ParticlePreviewLayer*>(node)) {
        shouldRecurse = false;

        runOpacity(nodePPL, isHidden);
        nodePPL->m_particleSystem->setVisible(!isHidden);
    } else {
        if (!shouldRecurse) {
            runOpacity(node, isHidden);
        }
    }

    if (shouldRecurse && node->getChildrenCount() > 0) {
        for (auto child : CCArrayExt<CCNode>(node->getChildren())) {
            recursiveOpacity(child, isHidden, nodesToIgnore);
        }
    }
}

void hideOrShowUI(bool isHidden, FLAlertLayer* popup, Slider* slider) {
    GEODE_UNWRAP_OR_ELSE(mem, err, MultiEditManager::get()) return;
    int property = MultiEditManager::getProperty(slider).unwrapOr(-99);

    CCArray* nodesToIgnore = CCArray::create();

    if (auto node = mem->getSlider(property)) nodesToIgnore->addObject(node);
    if (auto node = mem->getInput(property)) nodesToIgnore->addObject(node);
    if (auto node = mem->getInputLabel(property)) nodesToIgnore->addObject(node);
    if (auto node = mem->getInputBG(property)) nodesToIgnore->addObject(node);
    nodesToIgnore->addObject(popup->m_mainLayer->getChildByType<CCScale9Sprite*>(0));

    recursiveOpacity(popup, isHidden, nodesToIgnore);

    runOpacity(popup, isHidden);
    runOpacity(popup->m_mainLayer->getChildByType<CCScale9Sprite*>(0), isHidden);
}

class $modify(HUISetupTriggerPopup, SetupTriggerPopup) {
    struct Fields {
        bool isHideMode = false;
        Slider* currentSlider = nullptr;
    };

    $override
    bool init(EffectGameObject* trigger, CCArray* triggers, float width, float height, int unkEnum) {
        if (!SetupTriggerPopup::init(trigger, triggers, width, height, unkEnum)) return false;
        if (!Trigger::isTriggerPopup(this)) return true;

        GEODE_UNWRAP_OR_ELSE(mem, err, MultiEditManager::get()) return true;

        auto hideBtn = MultiEditManager::createSideMenuButton("hide-btn.png"_spr, this, menu_selector(HUISetupTriggerPopup::toggleHideMode));
        hideBtn->setID("hide-btn"_spr);

        if (typeinfo_cast<SetupShaderEffectPopup*>(this)) {
            hideBtn->toggleWithCallback(true);
        }

        Loader::get()->queueInMainThread([mem, hideBtn]() {
            if (mem->getSliders().size() > 0) {
                mem->addSideMenuButton(hideBtn);
            }
        });

        hideOrShowUI(false, this, nullptr);

        return true;
    }

    void toggleHideMode(CCObject* sender) {
        m_fields->isHideMode = !m_fields->isHideMode;
    }

    $override
    void sliderBegan(Slider* slider) {
        m_fields->currentSlider = slider;
        if (!m_fields->isHideMode) return;

        hideOrShowUI(true, this, slider);
    }

    $override
    void sliderEnded(Slider* slider) {
        m_fields->currentSlider = nullptr;
        if (!m_fields->isHideMode) return;

        hideOrShowUI(false, this, slider);
    }
};

class $modify(HUICreateParticlePopup, CreateParticlePopup) {
    struct Fields {
        bool isHideMode = false;
        bool isHidden = false;
        Slider* currentSlider = nullptr;
    };

    $override
    bool init(ParticleGameObject* obj, CCArray* objs, gd::string str) {
        if (!CreateParticlePopup::init(obj, objs, str)) return false;

        GEODE_UNWRAP_OR_ELSE(mem, err, MultiEditManager::get()) return true;

        auto hideBtn = MultiEditManager::createSideMenuButton("hide-btn.png"_spr, this, menu_selector(HUICreateParticlePopup::toggleHideMode));
        hideBtn->setID("hide-btn"_spr);

        Loader::get()->queueInMainThread([mem, hideBtn]() {
            mem->addSideMenuButton(hideBtn);
        });

        return true;
    }

    void updateParticles() {
        if (m_targetObject) {
            gd::string str = GameToolbox::saveParticleToString(m_particle);
            m_targetObject->setParticleString(str);
            m_targetObject->updateParticle();

            return;
        }

        for (auto targetObject : CCArrayExt<ParticleGameObject>(m_targetObjects)) {
            gd::string str = GameToolbox::saveParticleToString(m_particle);
            targetObject->setParticleString(str);
            targetObject->updateParticle();
        }
    }

    void toggleHideMode(CCObject* sender) {
        m_fields->isHideMode = !m_fields->isHideMode;
    }

    $override
    void sliderBegan(Slider* slider) {
        CreateParticlePopup::sliderBegan(slider);

        m_fields->currentSlider = slider;
        if (!m_fields->isHideMode) return;

        hideOrShowUI(true, this, slider);
        m_fields->isHidden = true;
        updateParticles();
    }

    $override
    void sliderEnded(Slider* slider) {
        CreateParticlePopup::sliderEnded(slider);

        m_fields->currentSlider = nullptr;
        if (!m_fields->isHideMode) return;

        hideOrShowUI(false, this, slider);
        m_fields->isHidden = false;
    }

    $override
    void updateParticleValueForType(float p0, gjParticleValue p1, CCParticleSystemQuad* p2) {
        CreateParticlePopup::updateParticleValueForType(p0, p1, p2);

        if (m_fields->isHidden) updateParticles();
    }
};

#ifdef GEODE_IS_DESKTOP
#include <Geode/modify/CCKeyboardDispatcher.hpp>

class $modify(CCKeyboardDispatcher) {
    bool dispatchKeyboardMSG(enumKeyCodes key, bool isKeyDown, bool isKeyRepeat) {
        if (!CCKeyboardDispatcher::dispatchKeyboardMSG(key, isKeyDown, isKeyRepeat)) return false;
        if (key != KEY_LeftShift && key != KEY_RightShift) return true;
        if (isKeyRepeat) return true;

        if (auto popup = Trigger::getTriggerPopup()) {
            hideOrShowUI(isKeyDown, popup, static_cast<HUISetupTriggerPopup*>(popup)->m_fields->currentSlider);
        } else if (auto popup = Trigger::getParticlePopup()) {
            hideOrShowUI(isKeyDown, popup, static_cast<HUICreateParticlePopup*>(popup)->m_fields->currentSlider);

            static_cast<HUICreateParticlePopup*>(popup)->m_fields->isHidden = isKeyDown;
            if (isKeyDown) static_cast<HUICreateParticlePopup*>(popup)->updateParticles();
        }

        return true;
    }
};

#endif // GEODE_IS_DESKTOP
