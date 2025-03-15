#pragma once

#include <Geode/Geode.hpp>
using namespace geode::prelude;

namespace Trigger {
    SetupTriggerPopup* getTriggerPopup();
    void setTriggerPopup(SetupTriggerPopup*);
    CreateParticlePopup* getParticlePopup();
    void setParticlePopup(CreateParticlePopup*);
    void resetPopups();

    bool isTriggerPopup(SetupTriggerPopup* popup);

    CCParticleSystemQuad* getParticleForObject(GameObject*);

    float getProperty(GameObject*, short);
    void setProperty(GameObject*, short, float);
    bool hasProperty(GameObject*, short);
    short getPropertyDecimalPlaces(GameObject*, short);
    bool canPropertyBeNegative(short);

    float getParticleValueByKey(CCParticleSystemQuad* particle, int key, bool isSet, float newValue = 0.0f);
    float getParticleValue(CCParticleSystemQuad* particle, int key);
    void setParticleValue(CCParticleSystemQuad* particle, int key, float value);
};
