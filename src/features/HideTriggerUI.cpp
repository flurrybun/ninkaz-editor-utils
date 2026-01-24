#include <Geode/modify/SetupTriggerPopup.hpp>
#include <Geode/modify/CreateParticlePopup.hpp>
#include "multi-edit/MultiEditContext.hpp"

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

        runOpacity(nodeInput->m_textLabel, isHidden);
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
    auto ctx = MultiEditContext::get(popup);
    int property = ctx->getPropertyID(slider).value_or(-99);

    CCArray* nodesToIgnore = CCArray::create();

    if (auto node = ctx->getSlider(property)) nodesToIgnore->addObject(node);
    if (auto node = ctx->getInput(property)) nodesToIgnore->addObject(node);
    if (auto node = ctx->getInputLabel(property)) nodesToIgnore->addObject(node);
    if (auto node = ctx->getInputBG(property)) nodesToIgnore->addObject(node);
    nodesToIgnore->addObject(popup->m_mainLayer->getChildByType<CCScale9Sprite*>(0));

    recursiveOpacity(popup, isHidden, nodesToIgnore);

    runOpacity(popup, isHidden);
    runOpacity(popup->m_mainLayer->getChildByType<CCScale9Sprite*>(0), isHidden);
}

bool isHideUIKeyPressed() {
    std::string keyString = Mod::get()->getSettingValue<std::string>("hide-ui-key");

    if (keyString == "CTRL/CMD") {
        return CCKeyboardDispatcher::get()->getControlKeyPressed()
            || CCKeyboardDispatcher::get()->getCommandKeyPressed();
    } else if (keyString == "SHIFT") {
        return CCKeyboardDispatcher::get()->getShiftKeyPressed();
    } else if (keyString == "ALT") {
        return CCKeyboardDispatcher::get()->getAltKeyPressed();
    }

    return false;
}

class $modify(HUISetupTriggerPopup, SetupTriggerPopup) {
    struct Fields {
        bool hideUIEnabled = false;
        bool isHidden = false;
        Slider* currentSlider;
    };

    $override
    bool init(EffectGameObject* trigger, CCArray* triggers, float width, float height, int unkEnum) {
        if (!SetupTriggerPopup::init(trigger, triggers, width, height, unkEnum)) return false;
        if (!MultiEditContext::isTriggerPopup(this)) return true;

        auto ctx = MultiEditContext::get(this);
        if (!ctx) return true;

        queueInMainThread([this, ctx]() {
            if (ctx->getSliders().size() > 0) {
                auto hideBtn = MultiEditContext::createSideMenuButton(
                    "hide-btn.png"_spr, [this](CCObject* sender) {
                        m_fields->hideUIEnabled = !m_fields->hideUIEnabled;
                    }
                );
                hideBtn->setID("hide-btn"_spr);
                hideBtn->setTag(1);

                if (typeinfo_cast<SetupShaderEffectPopup*>(this)) {
                    hideBtn->toggleWithCallback(true);
                }

                ctx->addSideMenuButton(hideBtn);
            }
        });

        #ifdef GEODE_IS_DESKTOP
        schedule(schedule_selector(HUISetupTriggerPopup::updateHideMode));
        #endif
        hideOrShowUI(false, this, nullptr);

        return true;
    }

    void updateHideMode(float dt) {
        bool hide = isHideUIKeyPressed();
        if (m_fields->currentSlider || hide == m_fields->isHidden) return;

        hideOrShowUI(hide, this, nullptr);
        m_fields->isHidden = hide;
    }

    $override
    void sliderBegan(Slider* slider) {
        m_fields->currentSlider = slider;
        if (!m_fields->hideUIEnabled) return;

        hideOrShowUI(true, this, slider);
    }

    $override
    void sliderEnded(Slider* slider) {
        m_fields->currentSlider = nullptr;
        if (!m_fields->hideUIEnabled) return;

        hideOrShowUI(false, this, slider);
    }
};

class $modify(HUICreateParticlePopup, CreateParticlePopup) {
    struct Fields : SliderDelegate {
        bool hideUIEnabled = false;
        bool isHidden = false;
        Slider* currentSlider;
        HUICreateParticlePopup* popup;

        void sliderBegan(Slider* slider) override {
            popup->CreateParticlePopup::sliderBegan(slider);

            currentSlider = slider;
            if (!hideUIEnabled) return;

            hideOrShowUI(true, popup, slider);
            popup->updateParticles();
        }

        void sliderEnded(Slider* slider) override {
            popup->CreateParticlePopup::sliderEnded(slider);

            currentSlider = nullptr;
            if (!hideUIEnabled) return;

            hideOrShowUI(false, popup, slider);
        }
    };

    $override
    bool init(ParticleGameObject* obj, CCArray* objs, gd::string str) {
        if (!CreateParticlePopup::init(obj, objs, str)) return false;

        m_fields->popup = this;

        auto ctx = MultiEditContext::get(this);
        if (!ctx) return true;

        auto hideBtn = MultiEditContext::createSideMenuButton(
            "hide-btn.png"_spr, [this](CCObject* sender) {
                m_fields->hideUIEnabled = !m_fields->hideUIEnabled;
            }
        );
        hideBtn->setID("hide-btn"_spr);
        hideBtn->setTag(1);

        ctx->addSideMenuButton(hideBtn);

        #ifdef GEODE_IS_DESKTOP
        schedule(schedule_selector(HUISetupTriggerPopup::updateHideMode));
        #endif
        hideOrShowUI(false, this, nullptr);

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

    void updateHideMode(float dt) {
        bool hide = isHideUIKeyPressed();
        if (m_fields->currentSlider || hide == m_fields->isHidden) return;

        hideOrShowUI(hide, this, nullptr);
        m_fields->isHidden = hide;

        updateParticles();
    }

    $override
    void createParticleSlider(
        gjParticleValue value, int page, bool centerLabel, CCPoint position, CCArray* displayNodes
    ) {
        CreateParticlePopup::createParticleSlider(value, page, centerLabel, position, displayNodes);

        auto slider = static_cast<Slider*>(
            static_cast<CCDictionary*>(
                m_sliderDicts->objectAtIndex(page)
            )->objectForKey(static_cast<int>(value))
        );
        if (slider) slider->m_delegate = m_fields.self();
    }

    $override
    void updateParticleValueForType(float p0, gjParticleValue p1, CCParticleSystemQuad* p2) {
        CreateParticlePopup::updateParticleValueForType(p0, p1, p2);

        if (m_fields->isHidden) updateParticles();
    }
};
