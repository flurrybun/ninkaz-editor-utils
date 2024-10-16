#pragma once

#include <Geode/modify/SetupTriggerPopup.hpp>
#include <Geode/modify/CCTextInputNode.hpp>

#include <Geode/Geode.hpp>
using namespace geode::prelude;

class $modify(NewSetupTriggerPopup, SetupTriggerPopup) {
    struct Fields {
        CCDictionaryExt<int, CCTextInputNode*> m_removedInputNodes;
        CCDictionaryExt<int, CCMenuItemSpriteExtra*> m_mixedButtons;
    };

    virtual void updateDefaultTriggerValues();
    void replaceInputWithButton(CCTextInputNode*, int);
    void replaceButtonWithInput(CCMenuItemSpriteExtra*, int, float);
    void setInputValue(CCTextInputNode*, float);
    void onMixedInput(CCObject*);
    // void tempLogVals();
};

class $modify(CCTextInputNodeTrigger, CCTextInputNode) {
    struct Fields {
        bool m_isTriggerInput = false;
        short m_tapCount = 0;
        CCAction* m_action;
    };

    bool ccTouchBegan(CCTouch*, CCEvent*);
    void onTripleTouch();
    void onTripleTouchTimeout();
};