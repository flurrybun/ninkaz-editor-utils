#pragma once

#include <Geode/Geode.hpp>
using namespace geode::prelude;

enum GroupMenuType {
    TargetGroup, CenterGroup, Item
};

enum SliderMenuType {
    Duration, Opacity
};

struct Trigger {
    enum PropType {
        Color, Duration, Opacity, TargetGroup, CenterGroup, Easing, Item
    };

    static std::map<short, Trigger>& triggerProperties() {
        static std::map<short, Trigger> instance = {
            { 899, Trigger(true, true, false, false, false, false, false) }, //color
            { 901, Trigger(false, true, false, true, true, true, false) }, //move
            { 1006, Trigger(true, false, false, true, false, false, false) }, //pulse
            { 1007, Trigger(false, true, true, true, false, false, false) }, //alpha
            { 1268, Trigger(false, true, false, true, false, false, false) }, //spawn
            { 1346, Trigger(false, true, false, true, true, true, false) }, //rotate
            { 2067, Trigger(false, true, false, true, true, true, false) }, //scale
            { 1347, Trigger(false, true, false, true, true, false, false) }, //follow
            { 1814, Trigger(false, true, false, true, false, false, false) }, //follow player y
            { 1611, Trigger(false, false, false, true, false, false, true) }, //count
            { 1811, Trigger(false, false, false, true, false, false, true) }, //instant count
            { 1817, Trigger(false, false, false, false, false, false, true) } //pickup
        };
        return instance;
    }

    static std::string getEasingString(EasingType easing) {
        switch (easing) {
            case EasingType::None: return "None";
            case EasingType::EaseInOut: return "Ease In Out"; case EasingType::EaseIn: return "Ease In"; case EasingType::EaseOut: return "Ease Out";
            case EasingType::ElasticInOut: return "Elastic In Out"; case EasingType::ElasticIn: return "Elastic In"; case EasingType::ElasticOut: return "Elastic Out";
            case EasingType::BounceInOut: return "Bounce In Out"; case EasingType::BounceIn: return "Bounce In"; case EasingType::BounceOut: return "Bounce Out";
            case EasingType::ExponentialInOut: return "Exponential In Out"; case EasingType::ExponentialIn: return "Exponential In"; case EasingType::ExponentialOut: return "Exponential Out";
            case EasingType::SineInOut: return "Sine In Out"; case EasingType::SineIn: return "Sine In"; case EasingType::SineOut: return "Sine Out";
            case EasingType::BackInOut: return "Back In Out"; case EasingType::BackIn: return "Back In"; case EasingType::BackOut: return "Back Out";
        }
        return "";
    }
    
    EffectGameObject* object;
    bool hasColor;
    bool hasDuration;
    bool hasOpacity;
    bool hasTargetGroup;
    bool hasCenterGroup;
    bool hasEasing;
    bool hasItem;

    Trigger(
        bool hasColor,
        bool hasDuration,
        bool hasOpacity,
        bool hasTargetGroup,
        bool hasCenterGroup,
        bool hasEasing,
        bool hasItem
    ) : hasColor(hasColor), hasDuration(hasDuration), hasOpacity(hasOpacity),
        hasTargetGroup(hasTargetGroup), hasCenterGroup(hasCenterGroup), hasEasing(hasEasing),
        hasItem(hasItem) {};

    Trigger(EffectGameObject* object) {
        auto it = triggerProperties().find(object->m_objectID);
        
        if (it != triggerProperties().end()) *this = it->second;
        else *this = Trigger(false, false, false, true, false, false, false);

        this->object = object;
    };

    bool hasProperty(PropType type) const {
        switch (type) {
            case Color: return hasColor;
            case Duration: return hasDuration;
            case Opacity: return hasOpacity;
            case TargetGroup: return hasTargetGroup;
            case CenterGroup: return hasCenterGroup;
            case Easing: return hasEasing;
            case Item: return hasItem;
        }

        return false;
    };

    std::variant<std::monostate, int, float, EasingType> getProperty(PropType type) const {
        switch (type) {
            case Color: return object->m_targetColor;
            case Duration: return object->m_duration;
            case Opacity: return object->m_opacity;
            case TargetGroup: return object->m_targetGroupID;
            case CenterGroup: return object->m_centerGroupID;
            case Easing: return object->m_easingType;
            case Item: return object->m_itemID;
        }

        return std::monostate();
    };

    std::string getPropertyString(PropType type) const {
        auto prop = getProperty(type);
        if (std::holds_alternative<int>(prop)) return std::to_string(std::get<int>(prop));
        if (std::holds_alternative<float>(prop)) return std::to_string(std::get<float>(prop));
        if (std::holds_alternative<EasingType>(prop)) return Trigger::getEasingString(std::get<EasingType>(prop));
    }

    void setProperty(PropType type, std::variant<int, float, EasingType> value) {
        switch (type) {
            case Color: object->m_targetColor = std::get<int>(value); break;
            case Duration: object->m_duration = std::get<float>(value); break;
            case Opacity: object->m_opacity = std::get<float>(value); break;
            case TargetGroup: object->m_targetGroupID = std::get<int>(value); break;
            case CenterGroup: object->m_centerGroupID = std::get<int>(value); break;
            case Easing: object->m_easingType = std::get<EasingType>(value); break;
            case Item: object->m_itemID = std::get<int>(value); break;
        }
    };

    template<typename T>
    T getProperty(PropType type) const {
        return std::get<T>(getProperty(type));
    };
};