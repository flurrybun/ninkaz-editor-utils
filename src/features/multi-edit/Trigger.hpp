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

    template<typename T>
    T* getPopup();
    SetupTriggerPopup* getTriggerPopup();
    CreateParticlePopup* getParticlePopup();

    CCParticleSystemQuad* getParticleForObject(GameObject*);

    float getProperty(GameObject*, short);
    void setProperty(GameObject*, short, float);
    bool hasProperty(GameObject*, short);
    std::string getEasingString(EasingType easing);
    short getPropertyDecimalPlaces(short);
    bool canPropertyBeNegative(short);
};