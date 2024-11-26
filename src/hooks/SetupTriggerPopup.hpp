#pragma once

#include <Geode/modify/SetupTriggerPopup.hpp>
#include <Geode/modify/CCTextInputNode.hpp>
#include <Geode/modify/CCMenuItemSpriteExtra.hpp>

#include <Geode/Geode.hpp>
using namespace geode::prelude;

class $modify(NewSetupTriggerPopup, SetupTriggerPopup) {
    struct Fields {
        CCDictionaryExt<int, CCTextInputNode*> m_removedInputNodes;
        CCDictionaryExt<int, CCMenuItemSpriteExtra*> m_mixedButtons;
        bool m_isMixedMode = false;
        CCMenuItemToggler* m_mixedModeButton = nullptr;
        CCArrayExt<CCTextInputNode*> m_overrideInputs;
        CCArrayExt<CCScale9Sprite*> m_inputBGs;
    };

    $override bool init(EffectGameObject*, CCArray*, float, float, int);
    $override void updateDefaultTriggerValues();
    void setupMultiEdit();
    void setupOverrideMultiEdit(CCArrayExt<CCTextInputNode*>);
    void replaceInputWithButton(CCTextInputNode*, int);
    void replaceButtonWithInput(CCMenuItemSpriteExtra*, int, float);
    void toggleSliderOfKey(int, bool);
    void toggleArrowButtonsOfKey(int, bool);
    void getInputBGs(CCArrayExt<CCTextInputNode*>);
    void setInputValue(CCTextInputNode*, float);
    CCTextInputNode* getInputOfKey(int);

    void onMixedInput(CCObject*);
    void toggleMixedMode(CCObject*);
    // void tempLogVals();
};

class $modify(CCTextInputNodeTrigger, CCTextInputNode) {
    struct Fields {
        int m_overrideTag = -1;
        bool m_isTriggerInput = false;
    };

    $override bool ccTouchBegan(CCTouch*, CCEvent*);
};

class $modify(TriggerItemSprite, CCMenuItemSpriteExtra) {
    struct Fields {
        int m_overrideTag = -1;
    };
};

#ifdef GEODE_IS_DESKTOP

#include <Geode/modify/CCEGLView.hpp>
class $modify(CCEGLViewTrigger, CCEGLView) {
    $override void onGLFWMouseCallBack(GLFWwindow*, int, int, int);
};

#endif
