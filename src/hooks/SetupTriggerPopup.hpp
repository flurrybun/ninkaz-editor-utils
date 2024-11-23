#pragma once

#include <Geode/modify/SetupTriggerPopup.hpp>
#include <Geode/modify/CCTextInputNode.hpp>

#include <Geode/Geode.hpp>
using namespace geode::prelude;

class $modify(NewSetupTriggerPopup, SetupTriggerPopup) {
    struct Fields {
        CCDictionaryExt<int, CCTextInputNode*> m_removedInputNodes;
        CCDictionaryExt<int, CCMenuItemSpriteExtra*> m_mixedButtons;
        bool m_isMixedMode = false;
        CCMenuItemToggler* m_mixedModeButton = nullptr;
    };

#ifdef GEODE_IS_DESKTOP
    $override bool init(EffectGameObject*, CCArray*, float, float, int);
#endif
    $override virtual void updateDefaultTriggerValues();
    void replaceInputWithButton(CCTextInputNode*, int);
    void replaceButtonWithInput(CCMenuItemSpriteExtra*, int, float);
    void setInputValue(CCTextInputNode*, float);

    void onMixedInput(CCObject*);
    void toggleMixedMode(CCObject*);
    // void tempLogVals();
};

class $modify(CCTextInputNodeTrigger, CCTextInputNode) {
    struct Fields {
        bool m_isTriggerInput = false;
    };

    $override bool ccTouchBegan(CCTouch*, CCEvent*);
};

#ifdef GEODE_IS_DESKTOP

#include <Geode/modify/CCEGLView.hpp>
class $modify(CCEGLViewTrigger, CCEGLView) {
    $override void onGLFWMouseCallBack(GLFWwindow*, int, int, int);
};

#endif
