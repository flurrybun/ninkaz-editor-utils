#include <Geode/modify/HSVWidgetPopup.hpp>
#include <Geode/modify/ConfigureHSVWidget.hpp>
#include <Geode/modify/CustomizeObjectLayer.hpp>
#include <Geode/modify/HSVLiveOverlay.hpp>
#include "MultiEditContext.hpp"
#include "../../misc/SpriteColor.hpp"

#include <Geode/Geode.hpp>
using namespace geode::prelude;

enum class HSVType {
    Hue = 0,
    Saturation = 1,
    Value = 2
};

enum class HSVProperty {
    BaseHue = 0,
    BaseSaturation = 1,
    BaseValue = 2,
    DetailHue = 3,
    DetailSaturation = 4,
    DetailValue = 5
};

const float MIXED_VALUE = std::numeric_limits<float>::quiet_NaN();

// value == MIXED_VALUE always returns false!!
bool isMixed(float value) {
    return std::isnan(value);
}

HSVType hsvPropertyToType(HSVProperty property) {
    switch (property) {
        case HSVProperty::BaseHue:
        case HSVProperty::DetailHue:
            return HSVType::Hue;
        case HSVProperty::BaseSaturation:
        case HSVProperty::DetailSaturation:
            return HSVType::Saturation;
        case HSVProperty::BaseValue:
        case HSVProperty::DetailValue:
            return HSVType::Value;
    }
}

bool isDetail(HSVProperty property) {
    return property == HSVProperty::DetailHue ||
        property == HSVProperty::DetailSaturation ||
        property == HSVProperty::DetailValue;
}

void setHSVSliderValue(ConfigureHSVWidget* widget, HSVType hsvType, float value) {
    switch (hsvType) {
        case HSVType::Hue:
            MultiEditContext::setSliderValue(widget->m_hueSlider, value, -180, 180);

            break;
        case HSVType::Saturation:
            if (widget->m_hsv.absoluteSaturation) {
                MultiEditContext::setSliderValue(widget->m_saturationSlider, value, -1, 1);
            } else {
                MultiEditContext::setSliderValue(widget->m_saturationSlider, value, 0, 2);
            }

            break;
        case HSVType::Value:
            if (widget->m_hsv.absoluteBrightness) {
                MultiEditContext::setSliderValue(widget->m_brightnessSlider, value, -1, 1);
            } else {
                MultiEditContext::setSliderValue(widget->m_brightnessSlider, value, 0, 2);
            }

            break;
    }
}

class $modify(MEHSVWidgetPopup, HSVWidgetPopup) {
    struct Fields : MultiEditContext {
        ConfigureHSVWidget* widget;
        bool isBase;

        void init(HSVWidgetPopup* popup) {
            widget = popup->m_widget;
            registerSelf(popup, popup, popup->m_widget);

            m_buttonOffset = popup->m_widget->getPosition();
        }

        float getProperty(GameObject* object, int intProperty) override {
            HSVProperty property = static_cast<HSVProperty>(intProperty);
            HSVType type = hsvPropertyToType(static_cast<HSVProperty>(property));

            GJSpriteColor* color = object->getRelativeSpriteColor(isDetail(property) ? 2 : 1);
            if (!color) return 0;

            switch (type) {
                case HSVType::Hue: return color->m_hsv.h;
                case HSVType::Saturation: return color->m_hsv.s;
                case HSVType::Value: return color->m_hsv.v;
            }
            return 0;
        }

        void setProperty(GameObject* object, int intProperty, float value) override {
            HSVProperty property = static_cast<HSVProperty>(intProperty);
            HSVType type = hsvPropertyToType(static_cast<HSVProperty>(property));

            GJSpriteColor* color = object->getRelativeSpriteColor(isDetail(property) ? 2 : 1);
            if (!color) return;

            switch (type) {
                case HSVType::Hue:
                    while (value > 180) value -= 360;
                    while (value < -180) value += 360;
                    color->m_hsv.h = value;
                    break;
                case HSVType::Saturation:
                    color->m_hsv.s = value;
                    break;
                case HSVType::Value:
                    color->m_hsv.v = value;
                    break;
            }
        }

        bool hasProperty(GameObject* object, int intProperty) override {
            HSVProperty property = static_cast<HSVProperty>(intProperty);

            if (isDetail(property)) {
                return nk::getDetailSpriteColor(object) != nullptr;
            } else {
                return nk::getBaseSpriteColor(object) != nullptr;
            }
        }

        int getPropertyDecimalPlaces(int intProperty) override {
            HSVProperty property = static_cast<HSVProperty>(intProperty);
            HSVType type = hsvPropertyToType(static_cast<HSVProperty>(property));

            switch (type) {
                case HSVType::Hue:
                    return 0;
                case HSVType::Saturation:
                case HSVType::Value:
                    return 2;
            }
        }

        PropertyBounds getPropertyBounds(int intProperty) override {
            return PropertyBounds::negInfToInf();
        }

        void onMixedInputApplied(int intProperty, std::optional<float> value) override {
            HSVProperty property = static_cast<HSVProperty>(intProperty);
            HSVType type = hsvPropertyToType(static_cast<HSVProperty>(property));

            switch (type) {
                case HSVType::Hue:
                    widget->m_hsv.h = value.value_or(MIXED_VALUE);
                    break;
                case HSVType::Saturation:
                    widget->m_hsv.s = value.value_or(MIXED_VALUE);
                    break;
                case HSVType::Value:
                    widget->m_hsv.v = value.value_or(MIXED_VALUE);
                    break;
            }

            updateMixedUI(intProperty, value);

            if (value) {
                setHSVSliderValue(widget, type, *value);
                widget->updateLabels();
            }
        }

        CCArray* getObjectArray() override {
            CCArray* objects = CCArray::create();
            
            for (auto object : CCArrayExt<GameObject*>(EditorUI::get()->m_selectedObjects)) {
                GJSpriteColor* color = isBase ? nk::getBaseSpriteColor(object) : nk::getDetailSpriteColor(object);

                if (color) {
                    objects->addObject(object);
                }
            }
            
            return objects;
        }
    };

    $override
    bool init(ccHSVValue hsv, HSVWidgetDelegate* delegate, gd::string title) {
        if (!HSVWidgetPopup::init(hsv, delegate, title)) return false;
        
        m_fields->init(this);
        m_fields->isBase = title == "Base HSV";

        int offset = m_fields->isBase ? 0 : 3;
        int hue = static_cast<int>(HSVProperty::BaseHue) + offset;
        int saturation = static_cast<int>(HSVProperty::BaseSaturation) + offset;
        int value = static_cast<int>(HSVProperty::BaseValue) + offset;

        if (auto input = m_widget->getChildByType<CCTextInputNode*>(0)) {
            input->setMaxLabelWidth(30);
            m_fields->addInput(input, hue);
        }
        if (auto input = m_widget->getChildByType<CCTextInputNode*>(1)) {
            input->setMaxLabelWidth(30);
            m_fields->addInput(input, saturation);
        }
        if (auto input = m_widget->getChildByType<CCTextInputNode*>(2)) {
            input->setMaxLabelWidth(30);
            m_fields->addInput(input, value);
        }

        m_fields->addInputBG(m_widget->getChildByType<CCScale9Sprite*>(1), hue);
        m_fields->addInputBG(m_widget->getChildByType<CCScale9Sprite*>(2), saturation);
        m_fields->addInputBG(m_widget->getChildByType<CCScale9Sprite*>(3), value);

        m_fields->addSlider(m_widget->m_hueSlider, hue);
        m_fields->addSlider(m_widget->m_saturationSlider, saturation);
        m_fields->addSlider(m_widget->m_brightnessSlider, value);

        CCMenu* buttonMenu = m_widget->getChildByType<CCMenu*>(0);
        m_fields->addButton(buttonMenu->getChildByType<CCMenuItemToggler*>(0), saturation);
        m_fields->addButton(buttonMenu->getChildByType<CCMenuItemToggler*>(1), value);

        // layer above widget
        m_buttonMenu->setZOrder(2);

        m_fields->setupMixed();

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
        std::array<HSVType, 3> hsvTypes = {
            HSVType::Hue, HSVType::Saturation, HSVType::Value
        };
        std::array<Slider*, 3> sliders = {
            m_widget->m_hueSlider, m_widget->m_saturationSlider, m_widget->m_brightnessSlider
        };

        CCMenu* widgetButtonMenu = m_widget->getChildByType<CCMenu*>(0);

        for (int i = 0; i < 3; i++) {
            Slider* slider = sliders[i];
            int hsvType = static_cast<int>(hsvTypes[i]);

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

        if (isMixed(m_widget->m_hsv.h)) toggleSlider(HSVType::Hue, false);
        if (isMixed(m_widget->m_hsv.s)) toggleSlider(HSVType::Saturation, false);
        if (isMixed(m_widget->m_hsv.v)) toggleSlider(HSVType::Value, false);
    }

    void toggleSlider(HSVType hsvType, bool isEnabled) {
        Slider* slider = nullptr;
        CCMenuItemToggler* toggle = nullptr;
        CCMenu* buttonMenu = m_widget->getChildByType<CCMenu*>(0);

        switch (hsvType) {
            case HSVType::Hue:
                slider = m_widget->m_hueSlider;
                break;
            case HSVType::Saturation:
                slider = m_widget->m_saturationSlider;
                toggle = buttonMenu->getChildByType<CCMenuItemToggler*>(0);
                break;
            case HSVType::Value:
                slider = m_widget->m_brightnessSlider;
                toggle = buttonMenu->getChildByType<CCMenuItemToggler*>(1);
                break;
        }

        if (isEnabled) {
            float resetValue = 0;
            if (hsvType == HSVType::Saturation && !m_widget->m_hsv.absoluteSaturation) resetValue = 1;
            else if (hsvType == HSVType::Value && !m_widget->m_hsv.absoluteBrightness) resetValue = 1;

            if (hsvType == HSVType::Hue) m_widget->m_hsv.h = resetValue;
            else if (hsvType == HSVType::Saturation) m_widget->m_hsv.s = resetValue;
            else if (hsvType == HSVType::Value) m_widget->m_hsv.v = resetValue;

            setHSVSliderValue(m_widget, hsvType, resetValue);
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

        m_fields->unlinkButtons[static_cast<int>(hsvType)]->setVisible(!isEnabled);
    }

    void onUnlink(CCObject* sender) {
        toggleSlider(static_cast<HSVType>(sender->getTag()), true);
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
        ccHSVValue multiHSV = ConfigureHSVWidget::getHSV(objects[0], nullptr, baseOrDetail);

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
            static_cast<MEHSVLiveOverlay*>(hsvOverlay)->toggleSlider(HSVType::Hue, true);
            static_cast<MEHSVLiveOverlay*>(hsvOverlay)->toggleSlider(HSVType::Saturation, true);
            static_cast<MEHSVLiveOverlay*>(hsvOverlay)->toggleSlider(HSVType::Value, true);
        } else if (m_addInputs) {
            if (auto ctx = MultiEditContext::get(getParent()->getParent())) {
                ctx->onMixedInputApplied(static_cast<int>(HSVType::Hue), 0);
                ctx->onMixedInputApplied(static_cast<int>(HSVType::Saturation), 1);
                ctx->onMixedInputApplied(static_cast<int>(HSVType::Value), 1);
            }
        }

        // this is a fix for a vanilla bug
        CCMenu* buttonMenu = getChildByType<CCMenu*>(0);
        buttonMenu->getChildByType<CCMenuItemToggler*>(0)->toggle(false);
        buttonMenu->getChildByType<CCMenuItemToggler*>(1)->toggle(false);
    }
};
