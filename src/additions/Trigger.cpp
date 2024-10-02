#include "Trigger.hpp"

#include <Geode/Geode.hpp>
using namespace geode::prelude;

namespace Trigger {
    const short DURATION = 10;
    const short OPACITY = 35;
    const short TARGET_GROUP = 51;
    const short CENTER_GROUP = 71;
    const short EASING = 30;
    const short ITEM = 80;
}

float Trigger::getProperty(EffectGameObject* object, short property) {
    auto value = getPropertyPointer(object, property);

    if (auto ptr = std::get_if<float*>(&value)) return **ptr;
    else if (auto ptr = std::get_if<int*>(&value)) return static_cast<float>(**ptr);
    else if (auto ptr = std::get_if<EasingType*>(&value)) return static_cast<float>(**ptr);

    return 0;
}

void Trigger::setProperty(EffectGameObject* object, short property, float newValue) {
    auto value = getPropertyPointer(object, property);

    if (auto ptr = std::get_if<float*>(&value)) **ptr = newValue;
    else if (auto ptr = std::get_if<int*>(&value)) **ptr = static_cast<int>(newValue);
    else if (auto ptr = std::get_if<EasingType*>(&value)) **ptr = static_cast<EasingType>(newValue);
}

bool Trigger::hasProperty(EffectGameObject* object, short property) {
    auto in = [property](const std::vector<short>& vec) {
        return std::find(vec.begin(), vec.end(), property) != vec.end();
    };

    switch (object->m_objectID) {
        case 899: return in({10});
        case 901: return in({10, 51, 71, 30, /**/ 28, 29});
        case 1007: return in({10, 35, 51});
        case 1006: return in({51});
        case 1268: return in({51});
        case 1346: return in({51, 71, 30});
        case 2067: return in({51, 71, 30});
        case 1347: return in({51, 71});
        case 1814: return in({10, 51});
        case 1611: return in({51, 80});
        case 1811: return in({51, 80});
        case 1817: return in({51, 80});
    }

    return false;
}

std::string Trigger::getEasingString(EasingType easing) {
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

short Trigger::getPropertyDecimalPlaces(short property) {
    int twoDecimalPlaces[] = {10, 35, 75, 84, 143, 144};

    if (std::find(std::begin(twoDecimalPlaces), std::end(twoDecimalPlaces), property)
        != std::end(twoDecimalPlaces)) return 2;
    return 0;
}

bool Trigger::canPropertyBeNegative(short property) {
    short negativeProperties[] = {28, 29, 143, 144};

    return std::find(std::begin(negativeProperties), std::end(negativeProperties), property)
        != std::end(negativeProperties);
}

std::variant<float*, int*, EasingType*> Trigger::getPropertyPointer(EffectGameObject* object, short property) {
    switch (property) {
        case 10: return &object->m_duration;
        case 35: return &object->m_opacity;
        case 51: return &object->m_targetGroupID;
        case 71: return &object->m_centerGroupID;
        case 75: return &object->m_shakeStrength;
        case 84: return &object->m_shakeInterval;
        case 28: return &object->m_moveOffsetX;
        case 29: return &object->m_moveOffsetY;
        case 30: return &object->m_easingType;
        case 85: return &object->m_easingRate;
        case 143: return &object->m_moveModX;
        case 144: return &object->m_moveModY;
        case 80: return &object->m_itemID;
        default: throw std::invalid_argument("Invalid property");
    }
}