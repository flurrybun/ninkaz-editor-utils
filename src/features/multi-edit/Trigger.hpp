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

    SetupTriggerPopup* getTriggerPopup();

    float getProperty(EffectGameObject*, short);
    void setProperty(EffectGameObject*, short, float);
    bool hasProperty(EffectGameObject*, short);
    std::string getEasingString(EasingType easing);
    short getPropertyDecimalPlaces(short);
    bool canPropertyBeNegative(short);
};