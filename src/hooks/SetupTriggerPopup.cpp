#include "SetupTriggerPopup.hpp"
#include "../additions/MixedInputPopup.hpp"
#include "../additions/Trigger.hpp"
#include "Geode/ui/Notification.hpp"

#include <Geode/modify/ColorSelectPopup.hpp>
#include <Geode/modify/SetupPulsePopup.hpp>
#include <Geode/modify/SetupOpacityPopup.hpp>
#include <Geode/modify/GJFollowCommandLayer.hpp>

#include <Geode/modify/CCKeyboardDispatcher.hpp>

#include <Geode/Geode.hpp>
using namespace geode::prelude;

bool isMobileControlsEnabled() {
#ifdef GEODE_IS_WINDOWS
    return Mod::get()->getSettingValue<bool>("show-mobile-controls");
#endif
    return true;
}

bool NewSetupTriggerPopup::isTriggerPopup() {
    // why does so much stuff inherit SetupTriggerPopup
    if (typeinfo_cast<SelectEventLayer*>(this)) return false;
    if (typeinfo_cast<CustomizeObjectSettingsPopup*>(this)) return false;
    if (typeinfo_cast<ColorSelectPopup*>(this)) return false;
    if (typeinfo_cast<SetupObjectOptions2Popup*>(this)) return false;
    if (typeinfo_cast<GJOptionsLayer*>(this)) return false;
    if (typeinfo_cast<UIOptionsLayer*>(this)) return false;
    if (typeinfo_cast<UIPOptionsLayer*>(this)) return false;
    if (typeinfo_cast<UISaveLoadLayer*>(this)) return false;
    return true;
}

bool NewSetupTriggerPopup::init(EffectGameObject* obj, CCArray* objs, float f1, float f2, int i1) {
    if (!SetupTriggerPopup::init(obj, objs, f1, f2, i1)) return false;
    if (!isMobileControlsEnabled()) return true;

    if (!isTriggerPopup()) return true;

    auto winSize = CCDirector::sharedDirector()->getWinSize();

    auto menu = CCMenu::create();
    menu->setID("trigger-menu"_spr);
    menu->setAnchorPoint({0, 0});
    menu->setPosition(winSize / 2 - m_buttonMenu->getPosition() + ccp(m_width / 2, -m_height / 2) + ccp(5, 0));
    menu->setLayout(ColumnLayout::create()
        ->setAxisAlignment(AxisAlignment::Start)
        ->setGap(5)
    );
    menu->setTouchPriority(-1000000);
    m_fields->m_sideMenu = menu;

    auto multiEditBtn = createMobileButton("multi-edit-btn.png"_spr, menu_selector(NewSetupTriggerPopup::toggleMixedMode));
    multiEditBtn->setID("multi-edit-btn"_spr);
    m_fields->m_mixedModeButton = multiEditBtn;

    auto hideBtn = createMobileButton("hide-btn.png"_spr, menu_selector(NewSetupTriggerPopup::toggleHideMode));
    hideBtn->setID("hide-btn"_spr);

    if (typeinfo_cast<SetupShaderEffectPopup*>(this)) {
        hideBtn->toggleWithCallback(true);
    }

    menu->addChild(multiEditBtn);
    menu->addChild(hideBtn);
    menu->updateLayout();

    m_buttonMenu->addChild(menu);

    return true;
}

CCMenuItemToggler* NewSetupTriggerPopup::createMobileButton(const char* sprName, SEL_MenuHandler selector) {
    auto onSprTop = CCSprite::createWithSpriteFrameName(sprName);
    auto onSpr = CCScale9Sprite::create("GJ_button_02.png");
    onSprTop->setScale(0.7);
    onSpr->setContentSize({30, 30});
    onSpr->addChildAtPosition(onSprTop, Anchor::Center);

    auto offSprTop = CCSprite::createWithSpriteFrameName(sprName);
    auto offSpr = CCScale9Sprite::create("GJ_button_04.png");
    offSprTop->setScale(0.7);
    offSpr->setContentSize({30, 30});
    offSpr->addChildAtPosition(offSprTop, Anchor::Center);

    auto btn = CCMenuItemToggler::create(offSpr, onSpr, this, selector);
    btn->toggle(false);

    return btn;
}

void NewSetupTriggerPopup::updateDefaultTriggerValues() {
    SetupTriggerPopup::updateDefaultTriggerValues();
    if (!isTriggerPopup()) return;
    
    setupMultiEdit();
}

void NewSetupTriggerPopup::setupMultiEdit() {
    CCDictionaryExt<int, CCTextInputNode*> inputNodes = m_inputNodes;
    CCDictionaryExt<int, CCFloat*> triggerValues = m_triggerValues;
    CCArrayExt<CCArray*> groupContainers = m_groupContainers;
    CCArrayExt<CCArray*> pageContainers = m_pageContainers;

    CCArrayExt<CCTextInputNode*> inputNodeArray;
    std::vector<int> inputKeysToRemove;

    // shifting the main layer to the left to make space for the mobile button menu
    // it has to be done now bc doing it on init causes some elements to be placed incorrectly
    if (isMobileControlsEnabled() && CCDirector::get()->getWinSize().width < 520) m_mainLayer->setPositionX(m_mainLayer->getPositionX() - (35 / 2));

    for (auto const& [key, input] : inputNodes) {
        inputNodeArray.push_back(input);

        static_cast<CCTextInputNodeTrigger*>(input)->m_fields->m_isTriggerInput = true;
        if (!m_gameObjects || m_gameObjects->count() == 0) continue;

        if (!triggerValues.contains(key)) {
            // replace input with button if the value is mixed
            replaceInputWithButton(input, key);
            inputKeysToRemove.push_back(key);
        }
    }

    for (auto const& key : inputKeysToRemove) {
        m_inputNodes->removeObjectForKey(key);
    }

    getInputBGs(inputNodeArray);
}

void NewSetupTriggerPopup::setupOverrideMultiEdit(CCArrayExt<CCTextInputNode*> inputs) {
    m_fields->m_overrideInputs.inner()->addObjectsFromArray(inputs.inner());
    CCArrayExt<EffectGameObject*> triggers = m_gameObjects;

    if (isMobileControlsEnabled() && CCDirector::get()->getWinSize().width < 520) m_mainLayer->setPositionX(m_mainLayer->getPositionX() - (35 / 2));

    for (auto input : inputs) {
        auto overrideTag = static_cast<CCTextInputNodeTrigger*>(input)->m_fields->m_overrideTag;
        if (overrideTag == -1) continue;

        static_cast<CCTextInputNodeTrigger*>(input)->m_fields->m_isTriggerInput = true;
        m_inputNodes->setObject(input, overrideTag);

        if (!m_gameObjects || m_gameObjects->count() == 0) continue;

        bool isMixed = false;
        float firstValue = Trigger::getProperty(triggers[0], overrideTag);

        for (auto trigger : triggers) {
            auto value = Trigger::getProperty(trigger, overrideTag);

            if (value != firstValue) {
                isMixed = true;
                break;
            }
        }

        if (isMixed) replaceInputWithButton(input, overrideTag);
    }

    getInputBGs(inputs);
}

void NewSetupTriggerPopup::replaceInputWithButton(CCTextInputNode* input, int property) {
    CCArrayExt<CCArray*> groupContainers = m_groupContainers;
    CCArrayExt<CCArray*> pageContainers = m_pageContainers;
    int overrideTag = static_cast<CCTextInputNodeTrigger*>(input)->m_fields->m_overrideTag;

    // create "mixed" button

    auto spr = CCLabelBMFont::create("Mixed", "bigFont.fnt");
    spr->limitLabelWidth(input->m_maxLabelWidth, input->getScale(), 0);

    auto btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(NewSetupTriggerPopup::onMixedInput));
    btn->setPosition(input->getPosition() - m_buttonMenu->getPosition());
    btn->setID("mixed-input-button"_spr);
    if (overrideTag == -1) btn->setTag(input->getTag());
    else btn->setTag(overrideTag);

    // add button to the correct group and page containers

    auto groupIt = std::find_if(groupContainers.begin(), groupContainers.end(), [&](auto const& group) { return group->containsObject(input); });
    auto pageIt = std::find_if(pageContainers.begin(), pageContainers.end(), [&](auto const& page) { return page->containsObject(input); });
    auto pageContainer = pageIt != pageContainers.end() ? *pageIt : nullptr;
    auto groupContainer = groupIt != groupContainers.end() ? *groupIt : nullptr;

    bool isGroupVisible = !groupContainer || static_cast<CCNode*>(groupContainer->firstObject())->isVisible();
    bool isPageVisible = !pageContainer || static_cast<CCNode*>(pageContainer->firstObject())->isVisible();
    btn->setVisible(isGroupVisible && isPageVisible);

    if (groupContainer) {
        groupContainer->addObject(btn);
        groupContainer->removeObject(input);
    }
    if (pageContainer) {
        pageContainer->addObject(btn);
        pageContainer->removeObject(input);
    }

    // swap input with button

    m_fields->m_removedInputNodes[property] = input;
    input->removeFromParent();

    m_buttonMenu->addChild(btn);
    m_fields->m_mixedButtons[property] = btn;

    toggleArrowButtonsOfKey(property, false);
    toggleSliderOfKey(property, false);
}

void NewSetupTriggerPopup::replaceButtonWithInput(CCMenuItemSpriteExtra* button, int property, float newValue) {
    CCTextInputNode* input = m_fields->m_removedInputNodes[property];

    CCArrayExt<CCArray*> groupContainers = m_groupContainers;
    CCArrayExt<CCArray*> pageContainers = m_pageContainers;

    auto groupIt = std::find_if(groupContainers.begin(), groupContainers.end(), [&](auto const& group) { return group->containsObject(button); });
    auto pageIt = std::find_if(pageContainers.begin(), pageContainers.end(), [&](auto const& page) { return page->containsObject(button); });
    auto pageContainer = pageIt != pageContainers.end() ? *pageIt : nullptr;
    auto groupContainer = groupIt != groupContainers.end() ? *groupIt : nullptr;

    if (groupContainer) {
        groupContainer->addObject(input);
        groupContainer->removeObject(button);
    }
    if (pageContainer) {
        pageContainer->addObject(input);
        pageContainer->removeObject(button);
    }

    button->setVisible(input->isVisible());

    m_mainLayer->addChild(input);
    m_inputNodes->setObject(input, property);

    m_fields->m_mixedButtons.inner()->removeObjectForKey(property);
    m_fields->m_removedInputNodes.inner()->removeObjectForKey(property);
    button->removeFromParent();

    setInputValue(input, newValue);

    toggleArrowButtonsOfKey(property, true);
    toggleSliderOfKey(property, true);
}

void NewSetupTriggerPopup::toggleSliderOfKey(int key, bool isEnabled) {
    auto slider = typeinfo_cast<Slider*>(m_valueControls->objectForKey(key));
    if (!slider) return;

    slider->setTouchEnabled(isEnabled);

    slider->m_groove->setOpacity(isEnabled ? 255 : 100);
    slider->getThumb()->setOpacity(isEnabled ? 255 : 0);
    slider->m_sliderBar->setOpacity(isEnabled ? 255 : 0);
}

void NewSetupTriggerPopup::toggleArrowButtonsOfKey(int key, bool isEnabled) {
    CCArrayExt<CCNode*> children = m_buttonMenu->getChildren();

    for (auto child : children) {
        auto button = typeinfo_cast<CCMenuItemSpriteExtra*>(child);
        if (!button || button->getID() == "mixed-input-button"_spr) continue;

        if (button->getTag() != key && static_cast<TriggerItemSprite*>(button)->m_fields->m_overrideTag != key) continue;

        button->setEnabled(isEnabled);
        button->setOpacity(isEnabled ? 255 : 100);
    }
}

void NewSetupTriggerPopup::getInputBGs(CCArrayExt<CCTextInputNode*> inputs) {
    CCArrayExt<CCNode*> children = m_mainLayer->getChildren();
    
    for (auto input : inputs) {
        auto position = input->getPosition();

        for (auto child : children) {
            auto bg = typeinfo_cast<CCScale9Sprite*>(child);
            if (!bg) continue;

            if (bg->getPosition() == position) {
                int tag = input->getTag();
                int overrideTag = static_cast<CCTextInputNodeTrigger*>(input)->m_fields->m_overrideTag;
                if (overrideTag != -1) tag = overrideTag;
                
                m_fields->m_inputBGs[tag] = bg;
                break;
            }
        }
    }
}

void NewSetupTriggerPopup::setInputValue(CCTextInputNode* input, float value) {
    auto factor = std::pow(10, input->m_decimalPlaces);
    auto newValue = std::floor(value * factor) / factor;

    std::ostringstream out;
    out << std::fixed << std::setprecision(input->m_decimalPlaces) << newValue;
    auto str = out.str();

    input->setString(str);
}

CCTextInputNode* NewSetupTriggerPopup::getInputOfKey(int key) {
    for (auto input : m_fields->m_overrideInputs) {
        if (static_cast<CCTextInputNodeTrigger*>(input)->m_fields->m_overrideTag == key) return input;
    }

    auto input = typeinfo_cast<CCTextInputNode*>(m_inputNodes->objectForKey(key));
    return input;
}

void NewSetupTriggerPopup::onMixedInput(CCObject* sender) {
    int property = static_cast<int>(sender->getTag());

    if (auto input = typeinfo_cast<CCTextInputNode*>(sender)) {
        int overrideTag = static_cast<CCTextInputNodeTrigger*>(input)->m_fields->m_overrideTag;
        if (overrideTag != -1) property = overrideTag;
    }

    auto callback = [sender, property, this](std::optional<float> value) {
        auto shouldReplaceInputWithButton = typeinfo_cast<CCTextInputNode*>(sender) && !value.has_value();
        auto shouldReplaceButtonWithInput = typeinfo_cast<CCMenuItemSpriteExtra*>(sender) && value.has_value();
        auto shouldChangeInput = typeinfo_cast<CCTextInputNode*>(sender) && value.has_value();

        if (shouldReplaceInputWithButton)
            replaceInputWithButton(static_cast<CCTextInputNode*>(sender), property);
        else if (shouldReplaceButtonWithInput)
            replaceButtonWithInput(static_cast<CCMenuItemSpriteExtra*>(sender), property, value.value());
        else if (shouldChangeInput)
            setInputValue(static_cast<CCTextInputNode*>(sender), value.value());
    };

    if (!m_gameObjects || m_gameObjects->count() == 0) {
        Notification::create("Only one trigger selected", NotificationIcon::Error, 2)->show();
        return;
    }

    auto input = getInputOfKey(property);
    InputValueType valueType = input ? input->m_valueType : InputValueType::Float;

    auto alert = MixedInputPopup::create(m_gameObjects, property, valueType, callback);
    alert->m_noElasticity = true;
    alert->show();
}

void NewSetupTriggerPopup::toggleMixedMode(CCObject* sender) {
    m_fields->m_isMixedMode = !m_fields->m_isMixedMode;

    GLubyte opacity = m_fields->m_isMixedMode ? 140 : 70;

    for (auto [key, bg] : m_fields->m_inputBGs) {
        bg->setOpacity(opacity);
        bg->runAction(CCEaseInOut::create(CCFadeTo::create(0.8, 100), 2));
    }
}

void NewSetupTriggerPopup::toggleHideMode(CCObject* sender) {
    m_fields->m_isHideMode = !m_fields->m_isHideMode;
}

void NewSetupTriggerPopup::sliderBegan(Slider* slider) {
    m_fields->m_currentSlider = slider;
    if (!m_fields->m_isHideMode) return;
    hideOrShowUI(true);
}

void NewSetupTriggerPopup::sliderEnded(Slider* slider) {
    m_fields->m_currentSlider = nullptr;
    if (!m_fields->m_isHideMode) return;
    hideOrShowUI(false);
}

void NewSetupTriggerPopup::hideOrShowUI(bool isHidden) {
    auto slider = m_fields->m_currentSlider;
    int sliderTag = slider ? slider->getTag() : -99;
    
    auto runOpacity = [isHidden](CCNode* node, GLubyte defaultOpacity = 255) {
        node->runAction(CCFadeTo::create(0.15, isHidden ? 0 : defaultOpacity));
    };

    runOpacity(this, 150);

    CCArrayExt<CCNode*> nodes;
    nodes.inner()->addObjectsFromArray(m_mainLayer->getChildren());
    nodes.inner()->addObjectsFromArray(m_buttonMenu->getChildren());
    nodes.inner()->addObjectsFromArray(m_fields->m_sideMenu->getChildren());

    nodes.inner()->removeObject(m_buttonMenu);
    nodes.inner()->removeObject(m_fields->m_inputBGs[sliderTag]);
    nodes.inner()->removeObject(m_inputLabels->objectForKey(sliderTag));

    // using a traditional for loop so we can iterate over the array while modifying it

    for (size_t i = 0; i < nodes.size(); i++) {
        auto node = nodes[i];

        auto tag = node->getTag();
        if (auto input = typeinfo_cast<CCTextInputNode*>(node)) {
            auto overrideTag = static_cast<CCTextInputNodeTrigger*>(input)->m_fields->m_overrideTag;
            if (overrideTag != -1) tag = overrideTag;
        } else if (auto button = typeinfo_cast<CCMenuItemSpriteExtra*>(node)) {
            auto overrideTag = static_cast<TriggerItemSprite*>(button)->m_fields->m_overrideTag;
            if (overrideTag != -1) tag = overrideTag;
        }

        if (tag == sliderTag) continue;

        // im sorry this is a total mess

        if (auto nodeSlider = typeinfo_cast<Slider*>(node)) {
            runOpacity(nodeSlider->m_groove);
            runOpacity(nodeSlider->m_sliderBar);
            runOpacity(nodeSlider->getThumb());
        } else if (auto nodeInput = typeinfo_cast<CCTextInputNode*>(node)) {
            runOpacity(nodeInput->m_placeholderLabel);
        } else if (auto nodeBG = typeinfo_cast<CCScale9Sprite*>(node); nodeBG && nodeBG->getTag() != 1) {
            runOpacity(nodeBG, 100);
        } else if (auto nodeBtn = typeinfo_cast<CCMenuItemSpriteExtra*>(node)) {
            auto spr = nodeBtn->getNormalImage();

            if (nodeBtn == m_easingRateButton) {
                runOpacity(spr, 100);
                runOpacity(spr->getChildByType<CCLabelBMFont>(0));
            } else if (auto btnSpr = typeinfo_cast<ButtonSprite*>(spr)) {
                if (auto spr = btnSpr->m_label) runOpacity(spr);
                if (auto spr = btnSpr->m_BGSprite) runOpacity(spr);
                if (auto spr = btnSpr->m_subSprite) runOpacity(spr);
                if (auto spr = btnSpr->m_subBGSprite) runOpacity(spr);
            } else if (auto subSpr = spr->getChildByType<CCSprite>(0)) {
                runOpacity(spr);
                runOpacity(subSpr);
            } else {
                runOpacity(spr);
            }
        } else if (auto nodeToggler = typeinfo_cast<CCMenuItemToggler*>(node)) {
            nodes.push_back(nodeToggler->m_onButton);
            nodes.push_back(nodeToggler->m_offButton);
        } else {
            runOpacity(node);
        }
    }
}

// some 2.0 triggers were likely made before rob had a good system for making trigger ui,
// so they don't use the usual functions for creating inputs, updating values, etc.
// meaning we have to deal with each on a case by case basis

// m_overrideTag is used in these cases when the input's tag doesn't match the property it represents

void setOverrideInputs(CCLayer* mainLayer, CCArrayExt<CCTextInputNode*>& inputs, std::map<int, int> tagOverrides) {
    CCArrayExt<CCNode*> children = mainLayer->getChildren();

    for (auto child : children) {
        if (auto input = typeinfo_cast<CCTextInputNode*>(child)) {
            inputs.push_back(input);
            auto& overrideTag = static_cast<CCTextInputNodeTrigger*>(input)->m_fields->m_overrideTag;

            if (tagOverrides.find(input->getTag()) != tagOverrides.end()) {
                overrideTag = tagOverrides[input->getTag()];
            }

            if (overrideTag == 50 || overrideTag == 51 || overrideTag == 71 || overrideTag == 23) {
                input->m_valueType = InputValueType::Uint;
                input->setMaxLabelWidth(40);
            } else {
                input->m_valueType = InputValueType::Float;
                input->setMaxLabelWidth(50);
            }
        }
    }
}

void setOverrideArrowButtons(CCMenu* buttonMenu, std::map<int, std::vector<CCPoint>>& buttonPositions) {
    CCArrayExt<CCNode*> buttons = buttonMenu->getChildren();

    for (auto button : buttons) {
        auto btn = typeinfo_cast<CCMenuItemSpriteExtra*>(button);
        if (!btn) continue;

        CCPoint pos = btn->getPosition();

        for (auto const& [tag, positions] : buttonPositions) {
            if (std::find(positions.begin(), positions.end(), pos) != positions.end()) {
                static_cast<TriggerItemSprite*>(btn)->m_fields->m_overrideTag = tag;
                break;
            }
        }
    }
}

class $modify(ColorSelectPopup) {
    bool init(EffectGameObject* obj, CCArray* objs, ColorAction* action) {
        if (!ColorSelectPopup::init(obj, objs, action)) return false;

        // color action is only nullptr for color triggers
        if (action) return true;

        CCArrayExt<CCTextInputNode*> inputs;
        std::map<int, int> inputOverrides = {
            {5, 10}, {3, 23}
        };
        std::map<int, std::vector<CCPoint>> arrowOverrides = {
            {23, {{100, 36}, {200, 36}}}
        };

        setOverrideInputs(m_mainLayer, inputs, inputOverrides);
        setOverrideArrowButtons(m_buttonMenu, arrowOverrides);

        m_valueControls->setObject(m_mainLayer->getChildByType<Slider>(0), 10);

        static_cast<NewSetupTriggerPopup*>(static_cast<SetupTriggerPopup*>(this))->setupOverrideMultiEdit(inputs);

        return true;
    }
};

class $modify(SetupPulsePopup) {
    bool init(EffectGameObject* obj, CCArray* objs) {
        if (!SetupPulsePopup::init(obj, objs)) return false;

        CCArrayExt<CCTextInputNode*> inputs;
        std::map<int, int> inputOverrides = {
            {4, 51}, {5, 50}, {8, 45}, {9, 46}, {10, 47}
        };
        std::map<int, std::vector<CCPoint>> arrowOverrides = {
            {51, {{95, 6}, {195, 6}}},
            {50, {{92, 177}, {192, 177}}}
        };

        setOverrideInputs(m_mainLayer, inputs, inputOverrides);
        setOverrideArrowButtons(m_buttonMenu, arrowOverrides);

        m_valueControls->setObject(m_mainLayer->getChildByType<Slider>(0), 45);
        m_valueControls->setObject(m_mainLayer->getChildByType<Slider>(1), 46);
        m_valueControls->setObject(m_mainLayer->getChildByType<Slider>(2), 47);

        static_cast<NewSetupTriggerPopup*>(static_cast<SetupTriggerPopup*>(this))->setupOverrideMultiEdit(inputs);

        return true;
    }
};

class $modify(SetupOpacityPopup) {
    bool init(EffectGameObject* obj, CCArray* objs) {
        if (!SetupOpacityPopup::init(obj, objs)) return false;

        CCArrayExt<CCTextInputNode*> inputs;
        std::map<int, int> inputOverrides = {
            {3, 51}, {4, 10}
        };
        std::map<int, std::vector<CCPoint>> arrowOverrides = {
            {51, {{-50, 186}, {50, 186}}}
        };

        setOverrideInputs(m_mainLayer, inputs, inputOverrides);
        setOverrideArrowButtons(m_buttonMenu, arrowOverrides);

        m_valueControls->setObject(m_mainLayer->getChildByType<Slider>(0), 10);
        // m_valueControls->setObject(m_mainLayer->getChildByType<Slider>(1), 35);

        static_cast<NewSetupTriggerPopup*>(static_cast<SetupTriggerPopup*>(this))->setupOverrideMultiEdit(inputs);

        return true;
    }
};

class $modify(GJFollowCommandLayer) {
    bool init(EffectGameObject* obj, CCArray* objs) {
        if (!GJFollowCommandLayer::init(obj, objs)) return false;

        CCArrayExt<CCTextInputNode*> inputs;
        std::map<int, int> inputOverrides = {
            {0, 72}, {1, 73}, {2, 51}, {3, 10}, {4, 71}
        };
        std::map<int, std::vector<CCPoint>> arrowOverrides = {
            {51, {{80, 101}, {80, 21}}},
            {71, {{160, 101}, {160, 21}}}
        };

        setOverrideInputs(m_mainLayer, inputs, inputOverrides);
        setOverrideArrowButtons(m_buttonMenu, arrowOverrides);

        m_valueControls->setObject(m_mainLayer->getChildByType<Slider>(0), 10);
        m_valueControls->setObject(m_mainLayer->getChildByType<Slider>(1), 72);
        m_valueControls->setObject(m_mainLayer->getChildByType<Slider>(2), 73);

        static_cast<NewSetupTriggerPopup*>(static_cast<SetupTriggerPopup*>(this))->setupOverrideMultiEdit(inputs);

        return true;
    }
};


bool CCTextInputNodeTrigger::ccTouchBegan(CCTouch* touch, CCEvent* event) {
    if (!m_fields->m_isTriggerInput) return CCTextInputNode::ccTouchBegan(touch, event);

    auto parent = getParent();
    if (!parent) return CCTextInputNode::ccTouchBegan(touch, event);

    auto popup = typeinfo_cast<SetupTriggerPopup*>(parent->getParent());
    if (!popup) return CCTextInputNode::ccTouchBegan(touch, event);

    auto isMixedMode = static_cast<NewSetupTriggerPopup*>(popup)->m_fields->m_isMixedMode;
    if (!isMixedMode) return CCTextInputNode::ccTouchBegan(touch, event);

    auto bounds = m_textField->boundingBox();
    auto touchLocation = m_textField->convertToNodeSpace(touch->getLocation()) - m_textField->getContentSize() / 2;
    if (!bounds.containsPoint(touchLocation)) return true;

    static_cast<NewSetupTriggerPopup*>(popup)->onMixedInput(this);

    return true;
}


class $modify(CCKeyboardDispatcher) {
    bool dispatchKeyboardMSG(enumKeyCodes key, bool isKeyDown, bool isKeyRepeat) {
        if (!CCKeyboardDispatcher::dispatchKeyboardMSG(key, isKeyDown, isKeyRepeat)) return false;
        if (key != KEY_LeftShift && key != KEY_RightShift) return true;
        if (isKeyRepeat) return true;

        auto popup = Trigger::getTriggerPopup();
        if (!popup) return true;
        if (!static_cast<NewSetupTriggerPopup*>(popup)->isTriggerPopup()) return true;

        static_cast<NewSetupTriggerPopup*>(popup)->hideOrShowUI(isKeyDown);

        return true;
    }
};


#ifdef GEODE_IS_WINDOWS
void CCEGLViewTrigger::onGLFWMouseCallBack(GLFWwindow* window, int button, int action, int mods) {
    CCEGLView::onGLFWMouseCallBack(window, button, action, mods);
    if (button != GLFW_MOUSE_BUTTON_RIGHT) return;
    if (action != GLFW_RELEASE) return;

    auto popup = Trigger::getTriggerPopup();
    if (!popup) return;
    if (!static_cast<NewSetupTriggerPopup*>(popup)->isTriggerPopup()) return;

    CCDictionaryExt<int, CCTextInputNode*> inputNodes = popup->m_inputNodes;
    auto mousePosition = getMousePos();

    for (auto const& [key, input] : inputNodes) {
        if (!input->isVisible()) continue;
        
        auto inputPosition = input->m_textField->convertToWorldSpace({0, 0});
        auto inputSize = input->m_textField->getContentSize();
        auto inputRect = CCRect(inputPosition, inputSize);

        if (inputRect.containsPoint(mousePosition)) {
            static_cast<NewSetupTriggerPopup*>(popup)->onMixedInput(input);
            return;
        }
    }
}
#endif

// temp function to determine what properties each trigger uses
// used for Trigger::hasProperty

// void NewSetupTriggerPopup::tempLogVals() {
//     CCDictionaryExt<int, CCTextInputNode*> inputNodes = m_inputNodes;
//     std::ostringstream os;

//     auto objectID = m_gameObject ? m_gameObject->m_objectID : static_cast<GameObject*>(m_gameObjects->firstObject())->m_objectID;

//     os << "case " << objectID << ": return in({";

//     int count = 0;
//     int size = inputNodes.size();
//     for (auto const& [key, input] : inputNodes) {
//         os << key;
//         if (++count < size) {
//             os << ", ";
//         }
//     }

//     os << "});";

//     log::info("{}", os.str());
//     // auto old = clipboard::read();
//     // clipboard::write(old + "\n" + os.str());
// }