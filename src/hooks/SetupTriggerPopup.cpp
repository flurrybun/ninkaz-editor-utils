#include "SetupTriggerPopup.hpp"
#include "../additions/MixedInputPopup.hpp"
#include "../additions/Trigger.hpp"
#include "Geode/ui/Notification.hpp"

#include <Geode/Geode.hpp>
using namespace geode::prelude;

bool NewSetupTriggerPopup::init(EffectGameObject* obj, CCArray* objs, float f1, float f2, int i1) {
    if (!SetupTriggerPopup::init(obj, objs, f1, f2, i1)) return false;

#ifdef GEODE_IS_MOBILE
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
#endif

    return true;
}

void NewSetupTriggerPopup::updateDefaultTriggerValues() {
    SetupTriggerPopup::updateDefaultTriggerValues();
    if (typeinfo_cast<GJOptionsLayer*>(this)) return;
    
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

    log::info("input {} tag: {}", this, this->getTag());
    static_cast<NewSetupTriggerPopup*>(popup)->onMixedInput(this);

    return true;
}


#ifdef GEODE_IS_DESKTOP
void CCEGLViewTrigger::onGLFWMouseCallBack(GLFWwindow* window, int button, int action, int mods) {
    CCEGLView::onGLFWMouseCallBack(window, button, action, mods);
    if (button != GLFW_MOUSE_BUTTON_RIGHT) return;
    if (action != GLFW_RELEASE) return;

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