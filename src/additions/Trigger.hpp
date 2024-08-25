#pragma once

#include <Geode/Geode.hpp>
using namespace geode::prelude;

struct Trigger {
    static constexpr short DURATION = 10;
    static constexpr short OPACITY = 35;
    static constexpr short TARGET_GROUP = 51;
    static constexpr short CENTER_GROUP = 71;
    static constexpr short EASING = 30;
    static constexpr short ITEM = 80;

    static float getProperty(EffectGameObject* object, short property) {
        switch (property) {
            case 10: return object->m_duration;
            case 35: return object->m_opacity;
            case 51: return object->m_targetGroupID;
            case 71: return object->m_centerGroupID;
            case 30: return static_cast<float>(object->m_easingType);
            case 80: return object->m_itemID;
        }

        return 0;
    }

    static void setProperty(EffectGameObject* object, short property, float value) {
        switch (property) {
            case 10: object->m_duration = value; break;
            case 35: object->m_opacity = value; break;
            case 51: object->m_targetGroupID = value; break;
            case 71: object->m_centerGroupID = value; break;
            case 30: object->m_easingType = static_cast<EasingType>(value); break;
            case 80: object->m_itemID = value; break;
        }
    }

    static bool hasProperty(EffectGameObject* object, short property) {
        auto id = object->m_objectID;

        switch (property) {
            case 10: return id == 899 || id == 901 || id == 1007 || id == 1814;
            case 35: return id == 1007;
            case 51: return id == 901 || id == 1006 || id == 1007 || id == 1268 || id == 1346 || id == 2067 ||
                id == 1347 || id == 1814 || id == 1611 || id == 1811 || id == 1817;
            case 71: return id == 901 || id == 1346 || id == 2067 || id == 1347;
            case 30: return id == 901 || id == 1346 || id == 2067;
            case 80: return id == 1611 || id == 1811 || id == 1817;
        }

        return false;
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

    static bool isPropertyFloat(short property) {
        return property == DURATION || property == OPACITY;
    }

    static bool canPropertyBeNegative(short property) {
        return false;
    }
};