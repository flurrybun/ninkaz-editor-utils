#pragma once

#include <Geode/modify/CreateParticlePopup.hpp>

#include <Geode/Geode.hpp>
using namespace geode::prelude;

float getParticleValueByKey(CCParticleSystemQuad* particle, int key, bool isSet, float newValue = 0.0f);
float getParticleValue(CCParticleSystemQuad* particle, int key);
void setParticleValue(CCParticleSystemQuad* particle, int key, float value);
std::optional<float> getParticleValue(CCArrayExt<CCParticleSystemQuad*> particles, int key);
void setParticleValue(CCArrayExt<CCParticleSystemQuad*> particles, int key, float value);

class $modify(MECreateParticlePopup, CreateParticlePopup) {
    struct Fields {
        CCDictionaryExt<int, CCTextInputNode*> removedInputNodes;
        CCDictionaryExt<int, CCMenuItemSpriteExtra*> mixedButtons;
    };
    
    $override bool init(ParticleGameObject* obj, cocos2d::CCArray* objs, gd::string str);
    void replaceInputWithButton(CCTextInputNode* input, int property, int page);
    void replaceButtonWithInput(CCMenuItemSpriteExtra* button, int property, float newValue, int page);
    void onMixedInput(CCObject* sender);
    
    void toggleSliderOfKey(int key, bool isEnabled);
    // void setInputValue(CCTextInputNode* input, float value);
};
