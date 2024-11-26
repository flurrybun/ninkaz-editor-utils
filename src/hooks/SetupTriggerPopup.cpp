#include "SetupTriggerPopup.hpp"
#include "../additions/MixedInputPopup.hpp"
#include "../additions/Trigger.hpp"
#include "Geode/ui/Notification.hpp"

#include <Geode/modify/ColorSelectPopup.hpp>
#include <Geode/modify/SetupPulsePopup.hpp>
#include <Geode/modify/SetupOpacityPopup.hpp>
#include <Geode/modify/GJFollowCommandLayer.hpp>

#include <Geode/Geode.hpp>
using namespace geode::prelude;

bool NewSetupTriggerPopup::init(EffectGameObject* obj, CCArray* objs, float f1, float f2, int i1) {
    if (!SetupTriggerPopup::init(obj, objs, f1, f2, i1)) return false;

#ifdef GEODE_IS_DESKTOP
    if (Mod::get()->getSettingValue<std::string>("select-mixed-input") == "Right Click") return true;
#endif

    // for some reason, some editor-related settings menus extend SetupTriggerPopup
    if (typeinfo_cast<GJOptionsLayer*>(this)) return true;
    if (!m_gameObjects || m_gameObjects->count() == 0) return true;

    auto winSize = CCDirector::sharedDirector()->getWinSize();

    auto onIcon = CCSprite::createWithSpriteFrameName("GJ_completesIcon_001.png");
    onIcon->setScale(0.8);
    auto onSpr = IconButtonSprite::create("GJ_button_01.png", onIcon, "Mixed", "bigFont.fnt");

    auto offIcon = CCSprite::createWithSpriteFrameName("GJ_deleteIcon_001.png");
    offIcon->setScale(0.8);
    auto offSpr = IconButtonSprite::create("GJ_button_06.png", offIcon, "Mixed", "bigFont.fnt");

    auto btn = CCMenuItemToggler::create(offSpr, onSpr, this, menu_selector(NewSetupTriggerPopup::toggleMixedMode));
    btn->setID("mixed-button"_spr);
    btn->toggle(false);
    m_fields->m_mixedModeButton = btn;

    auto menu = CCMenu::create();
    menu->setID("trigger-menu"_spr);
    menu->setAnchorPoint({1, 0});
    menu->setPosition(ccp(winSize.width - 5, 5));
    menu->setScale(0.5);
    menu->setLayout(ColumnLayout::create()
        ->setAxisAlignment(AxisAlignment::Start)
        ->setGap(5)
    );
    menu->setTouchPriority(-504);
    menu->addChild(btn);
    menu->updateLayout();

    m_mainLayer->addChild(menu);

    return true;
}

void NewSetupTriggerPopup::updateDefaultTriggerValues() {
    SetupTriggerPopup::updateDefaultTriggerValues();
    if (typeinfo_cast<GJOptionsLayer*>(this)) return;
    
    setupMultiEdit();
}

void NewSetupTriggerPopup::setupMultiEdit() {
    CCDictionaryExt<int, CCTextInputNode*> inputNodes = m_inputNodes;
    CCDictionaryExt<int, CCFloat*> triggerValues = m_triggerValues;
    CCArrayExt<CCArray*> groupContainers = m_groupContainers;
    CCArrayExt<CCArray*> pageContainers = m_pageContainers;

    std::vector<int> inputKeysToRemove;

    for (auto const& [key, input] : inputNodes) {
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
}

void NewSetupTriggerPopup::setupOverrideMultiEdit(CCArrayExt<CCTextInputNode*> inputs) {
    CCArrayExt<EffectGameObject*> triggers = m_gameObjects;

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

void NewSetupTriggerPopup::setInputValue(CCTextInputNode* input, float value) {
    auto factor = std::pow(10, input->m_decimalPlaces);
    auto newValue = std::floor(value * factor) / factor;

    std::ostringstream out;
    out << std::fixed << std::setprecision(input->m_decimalPlaces) << newValue;
    auto str = out.str();

    input->setString(str);
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

    // if (auto input = typeinfo_cast<CCTextInputNode*>(sender)) textInputClosed(input);

    if (!m_gameObjects || m_gameObjects->count() == 0) {
        Notification::create("Only one trigger selected", NotificationIcon::Error, 2)->show();
        return;
    }

    auto alert = MixedInputPopup::create(m_gameObjects, property, callback);
    alert->m_noElasticity = true;
    alert->show();
}

void NewSetupTriggerPopup::toggleMixedMode(CCObject* sender) {
    m_fields->m_isMixedMode = !m_fields->m_isMixedMode;
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

            if (overrideTag == 50 || overrideTag == 51 || overrideTag == 71 || overrideTag == 23) input->setMaxLabelWidth(40);
            else input->setMaxLabelWidth(50);
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


#ifdef GEODE_IS_DESKTOP
void CCEGLViewTrigger::onGLFWMouseCallBack(GLFWwindow* window, int button, int action, int mods) {
    CCEGLView::onGLFWMouseCallBack(window, button, action, mods);
    if (button != GLFW_MOUSE_BUTTON_RIGHT) return;
    if (action != GLFW_RELEASE) return;

    if (Mod::get()->getSettingValue<std::string>("select-mixed-input") == "Toggle Button") return;

    auto popup = Trigger::getTriggerPopup();
    if (!popup) return;
    if (typeinfo_cast<GJOptionsLayer*>(popup)) return;

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