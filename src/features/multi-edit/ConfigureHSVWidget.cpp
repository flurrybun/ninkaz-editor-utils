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

class $modify(MEConfigureHSVWidget, ConfigureHSVWidget) {
    $override
    bool init(ccHSVValue hsv, bool unused, bool addInputs) {
        if (!ConfigureHSVWidget::init(hsv, unused, true)) return false;

        for (auto [_, input] : CCDictionaryExt<int, CCTextInputNode*>(m_inputs)) {
            input->setUserObject("fix-text-input", CCBool::create(true));
            input->setLabelPlaceholderColor({ 150, 150, 150 });
        }

        CCMenu* buttonMenu = getChildByType<CCMenu*>(0);
        buttonMenu->getChildByType<CCMenuItemToggler*>(0)->setCascadeOpacityEnabled(true);
        buttonMenu->getChildByType<CCMenuItemToggler*>(1)->setCascadeOpacityEnabled(true);

        return true;
    }

    // ConfigureHSVWidget::getHSV is only used in the edit object menu and the hsv live overlay

    $override
    static ccHSVValue getHSV(GameObject* obj, CCArray* objs, int baseOrDetail) {
        CCArray* objects = objs ? objs : CCArray::create(obj);
        if (!objects || objects->count() == 0) return {0, 0, 0, false, false};

        ccHSVValue hsv = {0, 0, 0, true, true};
        bool first = true;

        for (auto object : CCArrayExt<GameObject*>(objects)) {
            GJSpriteColor* color = object->getRelativeSpriteColor(baseOrDetail);
            if (!color) continue;

            if (first) {
                hsv = color->m_hsv;
                first = false;
                continue;
            }

            if (color->m_hsv.h != hsv.h) hsv.h = MIXED_VALUE;
            if (color->m_hsv.s != hsv.s) hsv.s = MIXED_VALUE;
            if (color->m_hsv.v != hsv.v) hsv.v = MIXED_VALUE;

            if (color->m_hsv.absoluteSaturation == false) hsv.absoluteSaturation = false;
            if (color->m_hsv.absoluteBrightness == false) hsv.absoluteBrightness = false;
        }

        return hsv;
    }

    $override
    void onResetHSV(CCObject* sender) {
        ConfigureHSVWidget::onResetHSV(sender);

        if (auto ctx = MultiEditContext::getFromChild(this)) {
            ctx->onMixedInputApplied(static_cast<int>(HSVProperty::BaseHue), 0);
            ctx->onMixedInputApplied(static_cast<int>(HSVProperty::BaseSaturation), 1);
            ctx->onMixedInputApplied(static_cast<int>(HSVProperty::BaseValue), 1);
            ctx->onMixedInputApplied(static_cast<int>(HSVProperty::DetailHue), 0);
            ctx->onMixedInputApplied(static_cast<int>(HSVProperty::DetailSaturation), 1);
            ctx->onMixedInputApplied(static_cast<int>(HSVProperty::DetailValue), 1);
        }

        // this is a fix for a vanilla bug
        CCMenu* buttonMenu = getChildByType<CCMenu*>(0);
        buttonMenu->getChildByType<CCMenuItemToggler*>(0)->toggle(false);
        buttonMenu->getChildByType<CCMenuItemToggler*>(1)->toggle(false);
    }

    void setupMixed(bool isBase) {
        auto ctx = MultiEditContext::getFromChild(this);
        if (!ctx) return;

        int offset = isBase ? 0 : 3;
        int hue = static_cast<int>(HSVProperty::BaseHue) + offset;
        int saturation = static_cast<int>(HSVProperty::BaseSaturation) + offset;
        int value = static_cast<int>(HSVProperty::BaseValue) + offset;

        if (auto input = getChildByType<CCTextInputNode*>(0)) {
            input->setMaxLabelWidth(30);
            ctx->addInput(input, hue);
        }
        if (auto input = getChildByType<CCTextInputNode*>(1)) {
            input->setMaxLabelWidth(30);
            ctx->addInput(input, saturation);
        }
        if (auto input = getChildByType<CCTextInputNode*>(2)) {
            input->setMaxLabelWidth(30);
            ctx->addInput(input, value);
        }

        ctx->addInputBG(getChildByType<CCScale9Sprite*>(1), hue);
        ctx->addInputBG(getChildByType<CCScale9Sprite*>(2), saturation);
        ctx->addInputBG(getChildByType<CCScale9Sprite*>(3), value);

        ctx->addSlider(m_hueSlider, hue);
        ctx->addSlider(m_saturationSlider, saturation);
        ctx->addSlider(m_brightnessSlider, value);

        CCMenu* buttonMenu = getChildByType<CCMenu*>(0);
        ctx->addButton(buttonMenu->getChildByType<CCMenuItemToggler*>(0), saturation);
        ctx->addButton(buttonMenu->getChildByType<CCMenuItemToggler*>(1), value);
    }
};

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

        static_cast<MEConfigureHSVWidget*>(m_widget)->setupMixed(m_fields->isBase);

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
    struct Fields : MultiEditContext {
        ConfigureHSVWidget* widget;
        bool isBase;

        void init(HSVLiveOverlay* popup) {
            widget = popup->m_widget;
            registerSelf(popup, popup, popup->m_widget);

            m_buttonOffset = popup->m_widget->getPosition();
            m_popupScale = popup->m_widget->getScale();
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
    bool init(GameObject* obj, CCArray* objs) {
        if (!HSVLiveOverlay::init(obj, objs)) return false;

        m_fields->init(this);
        m_fields->isBase = m_activeTab == 1;

        static_cast<MEConfigureHSVWidget*>(m_widget)->setupMixed(m_fields->isBase);

        // layer above widget
        m_buttonMenu->setZOrder(2);

        m_fields->setupMixed();

        return true;
    }

    $override
    void hsvChanged(ConfigureHSVWidget* widget) {
        CCArrayExt<GameObject*> objects = m_objects ? m_objects : CCArray::createWithObject(m_object);
        ccHSVValue hsv = widget->m_hsv;

        for (auto object : objects) {
            auto color = object->getRelativeSpriteColor(m_activeTab);
            if (!color) continue;

            if (!isMixed(hsv.h)) color->m_hsv.h = hsv.h;
            if (!isMixed(hsv.s)) color->m_hsv.s = hsv.s;
            if (!isMixed(hsv.v)) color->m_hsv.v = hsv.v;

            color->m_hsv.absoluteSaturation = hsv.absoluteSaturation;
            color->m_hsv.absoluteBrightness = hsv.absoluteBrightness;
        }
    }
};
