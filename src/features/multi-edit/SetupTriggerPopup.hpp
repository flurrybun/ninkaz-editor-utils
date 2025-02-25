#pragma once

#include <Geode/modify/SetupTriggerPopup.hpp>
#include <Geode/modify/CCTextInputNode.hpp>
#include <Geode/modify/CCMenuItemSpriteExtra.hpp>

#include <Geode/Geode.hpp>
using namespace geode::prelude;

class $modify(MESetupTriggerPopup, SetupTriggerPopup) {
    struct Fields {
        CCDictionaryExt<int, CCTextInputNode*> m_removedInputNodes;
        CCDictionaryExt<int, CCMenuItemSpriteExtra*> m_mixedButtons;
        CCArrayExt<CCTextInputNode*> m_overrideInputs;
        CCDictionaryExt<int, CCScale9Sprite*> m_inputBGs;
        CCMenu* m_sideMenu = nullptr;

        bool m_isMixedMode = false;
        Ref<Notification> m_mixedNotification;

        bool m_isHideMode = false;
        Slider* m_currentSlider = nullptr;
    };

    bool isTriggerPopup();

    $override bool init(EffectGameObject*, CCArray*, float, float, int);
    CCMenuItemToggler* createMobileButton(const char*, SEL_MenuHandler);

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
    void toggleHideMode(CCObject*);
    // void tempLogVals();

    $override virtual void sliderBegan(Slider*);
    $override virtual void sliderEnded(Slider*);
    void hideOrShowUI(bool);
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

#ifdef GEODE_IS_WINDOWS
#include <Geode/modify/CCEGLView.hpp>
class $modify(CCEGLViewTrigger, CCEGLView) {
    $override void onGLFWMouseCallBack(GLFWwindow*, int, int, int);
};
#endif
