#pragma once

#include <Geode/Geode.hpp>
using namespace geode::prelude;

namespace Trigger {
    extern const short DURATION;
    extern const short OPACITY;
    extern const short TARGET_GROUP;
    extern const short CENTER_GROUP;
    extern const short EASING;
    extern const short ITEM;

    float getProperty(EffectGameObject* object, short property);
    void setProperty(EffectGameObject* object, short property, float newValue);
    bool hasProperty(EffectGameObject* object, short property);
    std::string getEasingString(EasingType easing);
    short getPropertyDecimalPlaces(short property);
    bool canPropertyBeNegative(short property);
    std::variant<float*, int*, EasingType*> getPropertyPointer(EffectGameObject* object, short property);
};