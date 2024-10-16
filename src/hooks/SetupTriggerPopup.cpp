#include "SetupTriggerPopup.hpp"
#include "../additions/MixedInputPopup.hpp"

#include <Geode/Geode.hpp>
using namespace geode::prelude;

void NewSetupTriggerPopup::updateDefaultTriggerValues() {
    SetupTriggerPopup::updateDefaultTriggerValues();
    
    CCDictionaryExt<int, CCTextInputNode*> inputNodes = m_inputNodes;
    CCDictionaryExt<int, CCFloat*> triggerValues = m_triggerValues;
    CCArrayExt<CCArray*> groupContainers = m_groupContainers;
    CCArrayExt<CCArray*> pageContainers = m_pageContainers;

    std::vector<int> inputKeysToRemove;

    // tempLogVals();

    for (auto const& [key, input] : inputNodes) {
        if (!triggerValues.contains(key)) {
            // replace input with button if the value is mixed
            replaceInputWithButton(input, key);
            inputKeysToRemove.push_back(key);
        } else {
            // allow double clicking input
            static_cast<CCTextInputNodeTrigger*>(input)->m_fields->m_isTriggerInput = true;
        }
    }

    for (auto const& key : inputKeysToRemove) {
        m_inputNodes->removeObjectForKey(key);
    }
}

void NewSetupTriggerPopup::replaceInputWithButton(CCTextInputNode* input, int property) {
    CCArrayExt<CCArray*> groupContainers = m_groupContainers;
    CCArrayExt<CCArray*> pageContainers = m_pageContainers;

    auto spr = CCLabelBMFont::create("Mixed", "bigFont.fnt");
    spr->limitLabelWidth(input->m_maxLabelWidth, input->getScale(), 0);

    auto btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(NewSetupTriggerPopup::onMixedInput));
    btn->setPosition(input->getPosition() - m_buttonMenu->getPosition());
    btn->setTag(input->getTag());

    auto groupIt = std::find_if(groupContainers.begin(), groupContainers.end(), [&](auto const& group) { return group->containsObject(input); });
    auto pageIt = std::find_if(pageContainers.begin(), pageContainers.end(), [&](auto const& page) { return page->containsObject(input); });
    auto pageContainer = pageIt != pageContainers.end() ? *pageIt : nullptr;
    auto groupContainer = groupIt != groupContainers.end() ? *groupIt : nullptr;

    auto isGroupVisible = !groupContainer || static_cast<CCNode*>(groupContainer->firstObject())->isVisible();
    auto isPageVisible = !pageContainer || static_cast<CCNode*>(pageContainer->firstObject())->isVisible();
    btn->setVisible(isGroupVisible && isPageVisible);
    
    if (groupContainer) {
        groupContainer->addObject(btn);
        groupContainer->removeObject(input);
    }
    if (pageContainer) {
        pageContainer->addObject(btn);
        pageContainer->removeObject(input);
    }

    m_fields->m_removedInputNodes[property] = input;
    input->removeFromParent();

    m_buttonMenu->addChild(btn);
    m_fields->m_mixedButtons[property] = btn;
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
    static_cast<CCTextInputNodeTrigger*>(input)->m_fields->m_isTriggerInput = true;

    m_fields->m_mixedButtons.inner()->removeObjectForKey(property);
    m_fields->m_removedInputNodes.inner()->removeObjectForKey(property);
    button->removeFromParent();

    setInputValue(input, newValue);
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
    auto property = static_cast<int>(sender->getTag());

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

    if (auto input = typeinfo_cast<CCTextInputNode*>(sender)) textInputClosed(input);

    auto alert = MixedInputPopup::create(m_gameObjects, property, callback);

    alert->m_noElasticity = true;
    alert->show();
}

// open mixed input popup when triple clicking input

bool CCTextInputNodeTrigger::ccTouchBegan(CCTouch* touch, CCEvent* event) {
    if (!CCTextInputNode::ccTouchBegan(touch, event)) return false;
    if (!m_fields->m_isTriggerInput) return true;

    auto& action = m_fields->m_action;

    if (action) {
        stopAction(action);
        m_fields->m_action = nullptr;
        m_fields->m_tapCount++;

        if (m_fields->m_tapCount == 3) {
            m_fields->m_tapCount = 0;
            onTripleTouch();
        } else {
            action = runAction(CCSequence::create(
                CCDelayTime::create(0.4),
                CCCallFunc::create(this, callfunc_selector(CCTextInputNodeTrigger::onTripleTouchTimeout)),
                nullptr
            ));
        }
    } else {
        m_fields->m_tapCount = 1;
        action = runAction(CCSequence::create(
            CCDelayTime::create(0.4),
            CCCallFunc::create(this, callfunc_selector(CCTextInputNodeTrigger::onTripleTouchTimeout)),
            nullptr
        ));
    }

    return true;
}

void CCTextInputNodeTrigger::onTripleTouch() {
    auto triggerPopup = typeinfo_cast<SetupTriggerPopup*>(getParent()->getParent());

    if (triggerPopup) static_cast<NewSetupTriggerPopup*>(triggerPopup)->onMixedInput(this);
}

void CCTextInputNodeTrigger::onTripleTouchTimeout() {
    m_fields->m_action = nullptr;
    m_fields->m_tapCount = 0;
}

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