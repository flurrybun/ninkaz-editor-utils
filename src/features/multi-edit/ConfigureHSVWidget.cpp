#include <Geode/modify/HSVWidgetPopup.hpp>
#include <Geode/modify/ConfigureHSVWidget.hpp>
#include <Geode/modify/CustomizeObjectLayer.hpp>
#include <Geode/modify/HSVLiveOverlay.hpp>
#include "MultiEditManager.hpp"

#include <Geode/Geode.hpp>
using namespace geode::prelude;

const float MIXED_VALUE = std::numeric_limits<float>::quiet_NaN();
const int HUE = 2000, SATURATION = 2001, VALUE = 2002;

// value == MIXED_VALUE always returns false!!
bool isMixed(float value) {
    return std::isnan(value);
}

void setSliderValue(ConfigureHSVWidget* widget, int hsvType, float value) {
    switch (hsvType) {
        case HUE:
            MultiEditManager::setSliderValue(widget->m_hueSlider, value, -180, 180);
            break;
        case SATURATION:
            if (widget->m_hsv.absoluteSaturation) MultiEditManager::setSliderValue(widget->m_saturationSlider, value, -1, 1);
            else MultiEditManager::setSliderValue(widget->m_saturationSlider, value, 0, 2);
            break;
        case VALUE:
            if (widget->m_hsv.absoluteBrightness) MultiEditManager::setSliderValue(widget->m_brightnessSlider, value, -1, 1);
            else MultiEditManager::setSliderValue(widget->m_brightnessSlider, value, 0, 2);
            break;
    }
}

class $modify(HSVWidgetPopup) {
    struct Fields {
        Ref<MultiEditManager> multiEditManager;
    };

    $override
    bool init(ccHSVValue hsv, HSVWidgetDelegate* delegate, gd::string title) {
        if (!HSVWidgetPopup::init(hsv, delegate, title)) return false;

        auto mem = MultiEditManager::create(this, EditorUI::get()->m_selectedObjects);
        m_fields->multiEditManager = mem;

        mem->setCallback([this, mem, hsv](int property, std::optional<float> value) {
            if (property > VALUE) property -= 3;

            switch (property) {
                case HUE:
                    m_widget->m_hsv.h = value.value_or(MIXED_VALUE);
                    break;
                case SATURATION:
                    m_widget->m_hsv.s = value.value_or(MIXED_VALUE);
                    break;
                case VALUE:
                    m_widget->m_hsv.v = value.value_or(MIXED_VALUE);
                    break;
            }

            if (value.has_value()) {
                setSliderValue(m_widget, property, value.value());
                m_widget->updateLabels();
            }
        });

        int baseOrDetailOffset = title == "Base HSV" ? 0 : 3;

        if (auto input = m_widget->getChildByType<CCTextInputNode*>(0)) {
            input->setMaxLabelWidth(30);
            mem->addInput(input, HUE + baseOrDetailOffset);
        }
        if (auto input = m_widget->getChildByType<CCTextInputNode*>(1)) {
            input->setMaxLabelWidth(30);
            mem->addInput(input, SATURATION + baseOrDetailOffset);
        }
        if (auto input = m_widget->getChildByType<CCTextInputNode*>(2)) {
            input->setMaxLabelWidth(30);
            mem->addInput(input, VALUE + baseOrDetailOffset);
        }

        mem->addInputBG(m_widget->getChildByType<CCScale9Sprite*>(1), HUE + baseOrDetailOffset);
        mem->addInputBG(m_widget->getChildByType<CCScale9Sprite*>(2), SATURATION + baseOrDetailOffset);
        mem->addInputBG(m_widget->getChildByType<CCScale9Sprite*>(3), VALUE + baseOrDetailOffset);

        mem->addSlider(m_widget->m_hueSlider, HUE + baseOrDetailOffset);
        mem->addSlider(m_widget->m_saturationSlider, SATURATION + baseOrDetailOffset);
        mem->addSlider(m_widget->m_brightnessSlider, VALUE + baseOrDetailOffset);

        CCMenu* buttonMenu = m_widget->getChildByType<CCMenu*>(0);
        mem->addButton(buttonMenu->getChildByType<CCMenuItemToggler*>(0), SATURATION + baseOrDetailOffset);
        mem->addButton(buttonMenu->getChildByType<CCMenuItemToggler*>(1), VALUE + baseOrDetailOffset);

        mem->setInputParentNode(m_widget);
        mem->setButtonOffset(m_widget->getPosition());

        // layer above widget
        m_buttonMenu->setZOrder(2);

        mem->setupSideMenu();
        mem->setupMixed();

        return true;
    }
};

class $modify(CustomizeObjectLayer) {
    void hsvPopupClosed(HSVWidgetPopup* hsvPopup, ccHSVValue newHSV) {
        if (!m_targetObjects || m_targetObjects->count() <= 1) {
            CustomizeObjectLayer::hsvPopupClosed(hsvPopup, newHSV);
            return;
        }

        for (auto object : CCArrayExt<GameObject*>(m_targetObjects)) {
            auto color = object->getRelativeSpriteColor(m_selectedMode);
            if (!color) continue;

            ccHSVValue& hsv = color->m_hsv;

            if (!isMixed(newHSV.h)) hsv.h = newHSV.h;
            if (!isMixed(newHSV.s)) {
                hsv.s = newHSV.s;
                hsv.absoluteSaturation = newHSV.absoluteSaturation;
            }
            if (!isMixed(newHSV.v)) {
                hsv.v = newHSV.v;
                hsv.absoluteBrightness = newHSV.absoluteBrightness;
            }
        }

        updateHSVButtons();
    }
};

class $modify(MEHSVLiveOverlay, HSVLiveOverlay) {
    struct Fields {
        std::map<int, CCMenuItemSpriteExtra*> unlinkButtons;
    };

    $override
    bool init(GameObject* obj, CCArray* objs) {
        if (!HSVLiveOverlay::init(obj, objs)) return false;
        setupMixedSliders();
        return true;
    }

    $override
    void onSelectTab(CCObject* sender) {
        HSVLiveOverlay::onSelectTab(sender);
        setupMixedSliders();
    }

    void setupMixedSliders() {
        int hsvTypes[] = {HUE, SATURATION, VALUE};
        Slider* sliders[] = {m_widget->m_hueSlider, m_widget->m_saturationSlider, m_widget->m_brightnessSlider};
        CCMenu* widgetButtonMenu = m_widget->getChildByType<CCMenu*>(0);

        for (int i = 0; i < 3; i++) {
            Slider* slider = sliders[i];
            int hsvType = hsvTypes[i];

            auto spr = CCSprite::createWithSpriteFrameName("GJ_resetBtn_001.png");
            auto btn = CCMenuItemSpriteExtra::create(
                spr, this, menu_selector(MEHSVLiveOverlay::onUnlink)
            );

            btn->setPosition(slider->getPosition() - widgetButtonMenu->getPosition());
            btn->setVisible(false);
            btn->setID("unlink-btn"_spr);
            btn->setTag(hsvType);

            widgetButtonMenu->addChild(btn);
            m_fields->unlinkButtons[hsvType] = btn;
        }

        if (isMixed(m_widget->m_hsv.h)) toggleSlider(HUE, false);
        if (isMixed(m_widget->m_hsv.s)) toggleSlider(SATURATION, false);
        if (isMixed(m_widget->m_hsv.v)) toggleSlider(VALUE, false);
    }

    void toggleSlider(int hsvType, bool isEnabled) {
        Slider* slider = nullptr;
        CCMenuItemToggler* toggle = nullptr;
        CCMenu* buttonMenu = m_widget->getChildByType<CCMenu*>(0);

        switch (hsvType) {
            case HUE:
                slider = m_widget->m_hueSlider;
                break;
            case SATURATION:
                slider = m_widget->m_saturationSlider;
                toggle = buttonMenu->getChildByType<CCMenuItemToggler*>(0);
                break;
            case VALUE:
                slider = m_widget->m_brightnessSlider;
                toggle = buttonMenu->getChildByType<CCMenuItemToggler*>(1);
                break;
        }

        if (isEnabled) {
            float resetValue = 0;
            if (hsvType == SATURATION && !m_widget->m_hsv.absoluteSaturation) resetValue = 1;
            else if (hsvType == VALUE && !m_widget->m_hsv.absoluteBrightness) resetValue = 1;

            if (hsvType == HUE) m_widget->m_hsv.h = resetValue;
            else if (hsvType == SATURATION) m_widget->m_hsv.s = resetValue;
            else if (hsvType == VALUE) m_widget->m_hsv.v = resetValue;

            setSliderValue(m_widget, hsvType, resetValue);
            m_widget->updateLabels();
            hsvChanged(m_widget);
        } else {
            slider->getThumb()->setPosition(9999, 0);
        }

        slider->m_groove->setOpacity(isEnabled ? 255 : 100);
        slider->getThumb()->setOpacity(isEnabled ? 255 : 0);
        slider->m_sliderBar->setOpacity(isEnabled ? 255 : 0);

        if (toggle) {
            toggle->setEnabled(isEnabled);
            toggle->setOpacity(isEnabled ? 255 : 100);
        }

        m_fields->unlinkButtons[hsvType]->setVisible(!isEnabled);
    }

    void onUnlink(CCObject* sender) {
        int hsvType = sender->getTag();
        toggleSlider(hsvType, true);
    }

    $override
    void hsvChanged(ConfigureHSVWidget* widget) {
        if (!m_objects || m_objects->count() <= 1) {
            HSVLiveOverlay::hsvChanged(widget);
            return;
        }

        ccHSVValue& newHSV = widget->m_hsv;

        for (auto object : CCArrayExt<GameObject*>(m_objects)) {
            auto color = object->getRelativeSpriteColor(m_activeTab);
            if (!color) continue;

            ccHSVValue& hsv = color->m_hsv;

            if (!isMixed(newHSV.h)) hsv.h = newHSV.h;
            if (!isMixed(newHSV.s)) {
                hsv.s = newHSV.s;
                hsv.absoluteSaturation = newHSV.absoluteSaturation;
            }
            if (!isMixed(newHSV.v)) {
                hsv.v = newHSV.v;
                hsv.absoluteBrightness = newHSV.absoluteBrightness;
            }
        }
    }
};

class $modify(ConfigureHSVWidget) {
    $override
    bool init(ccHSVValue hsv, bool unused, bool addInputs) {
        if (!ConfigureHSVWidget::init(hsv, unused, addInputs)) return false;

        CCMenu* buttonMenu = getChildByType<CCMenu*>(0);
        buttonMenu->getChildByType<CCMenuItemToggler*>(0)->setCascadeOpacityEnabled(true);
        buttonMenu->getChildByType<CCMenuItemToggler*>(1)->setCascadeOpacityEnabled(true);

        return true;
    }

    $override
    void updateLabels() {
        ConfigureHSVWidget::updateLabels();
        if (m_addInputs) return;

        if (isMixed(m_hsv.h)) m_hueLabel->setString("Hue: Mixed");
        if (isMixed(m_hsv.s)) m_saturationLabel->setString("Saturation: Mixed");
        if (isMixed(m_hsv.v)) m_brightnessLabel->setString("Brightness: Mixed");
    }

    // ConfigureHSVWidget::getHSV is only used in the edit object menu and the hsv live overlay

    $override
    static ccHSVValue getHSV(GameObject* obj, CCArray* objs, int baseOrDetail) {
        if (obj) return ConfigureHSVWidget::getHSV(obj, objs, baseOrDetail);

        CCArrayExt<GameObject*> objects = objs;

        ccHSVValue multiHSV;

        if (auto obj = typeinfo_cast<GameObject*>(objects[0])) {
            GJSpriteColor* color = obj->getRelativeSpriteColor(baseOrDetail);
            if (color) multiHSV = color->m_hsv;
        }

        multiHSV.absoluteSaturation = true;
        multiHSV.absoluteBrightness = true;

        for (auto object : objects) {
            auto hsv = ConfigureHSVWidget::getHSV(object, nullptr, baseOrDetail);

            if (hsv.h != multiHSV.h) multiHSV.h = MIXED_VALUE;
            if (hsv.s != multiHSV.s) multiHSV.s = MIXED_VALUE;
            if (hsv.v != multiHSV.v) multiHSV.v = MIXED_VALUE;

            if (hsv.absoluteSaturation == false) multiHSV.absoluteSaturation = false;
            if (hsv.absoluteBrightness == false) multiHSV.absoluteBrightness = false;
        }

        return multiHSV;
    }

    $override
    void onResetHSV(CCObject* sender) {
        ConfigureHSVWidget::onResetHSV(sender);

        HSVLiveOverlay* hsvOverlay = EditorUI::get()->m_hsvOverlay;
        if (hsvOverlay) {
            static_cast<MEHSVLiveOverlay*>(hsvOverlay)->toggleSlider(HUE, true);
            static_cast<MEHSVLiveOverlay*>(hsvOverlay)->toggleSlider(SATURATION, true);
            static_cast<MEHSVLiveOverlay*>(hsvOverlay)->toggleSlider(VALUE, true);
        } else if (m_addInputs) {
            GEODE_UNWRAP_OR_ELSE(mem, err, MultiEditManager::get()) return;
            mem->removeMixed(HUE, 0);
            mem->removeMixed(HUE + 3, 0);
            mem->removeMixed(SATURATION, 1);
            mem->removeMixed(SATURATION + 3, 1);
            mem->removeMixed(VALUE, 1);
            mem->removeMixed(VALUE + 3, 1);
        }

        // this is a fix for a vanilla bug
        CCMenu* buttonMenu = getChildByType<CCMenu*>(0);
        buttonMenu->getChildByType<CCMenuItemToggler*>(0)->toggle(false);
        buttonMenu->getChildByType<CCMenuItemToggler*>(1)->toggle(false);
    }
};
