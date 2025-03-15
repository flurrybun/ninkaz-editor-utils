#include "MultiEditManager.hpp"
#include "MixedInputPopup.hpp"
#include "Trigger.hpp"
#include "../../misc/StringUtils.hpp"

#include <Geode/modify/CCTextInputNode.hpp>

MultiEditManager* MultiEditManager::s_instance = nullptr;

bool MultiEditManager::init(FLAlertLayer* popup, CCArray* gameObjects) {
    m_popup = popup;
    m_inputParentNode = popup->m_mainLayer;
    if (gameObjects) m_gameObjects.inner()->addObjectsFromArray(gameObjects);

    if (auto triggerPopup = typeinfo_cast<SetupTriggerPopup*>(popup)) {
        Trigger::setTriggerPopup(triggerPopup);
    } else if (auto particlePopup = typeinfo_cast<CreateParticlePopup*>(popup)) {
        Trigger::setParticlePopup(particlePopup);
    }

    return true;
}

MultiEditManager::~MultiEditManager() {
    s_instance = nullptr;
    Trigger::resetPopups();
}

CCMenuItemSpriteExtra* MultiEditManager::createMixedButton(int property) {
    auto spr = CCLabelBMFont::create("Mixed", "bigFont.fnt");

    if (auto input = m_inputs[property]) {
        spr->limitLabelWidth(input->m_maxLabelWidth, input->getScale(), 0);
    }

    auto btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(MultiEditManager::onMixed));
    btn->setID("mixed-input-btn"_spr);
    btn->setUserObject("property"_spr, CCInteger::create(property));

    return btn;
}

void MultiEditManager::setupSideMenu() {
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    CCSize popupSize = m_popup->m_mainLayer->getChildByType<CCScale9Sprite>(0)->getContentSize();

    auto menu = CCMenu::create();
    menu->setID("trigger-menu"_spr);
    menu->setAnchorPoint({0, 0});
    menu->setPosition(winSize / 2 + ccp(popupSize.width / 2, -popupSize.height / 2) + ccp(5, 0));
    menu->setLayout(ColumnLayout::create()
        ->setAxisAlignment(AxisAlignment::Start)
        ->setGap(5)
    );
    menu->getLayout()->ignoreInvisibleChildren(true);
    menu->setZOrder(1000);
    m_sideMenu = menu;

    bool showMultiEdit = m_gameObjects.size() > 1;

    auto multiEditBtn = createSideMenuButton("multi-edit-btn.png"_spr, this, menu_selector(MultiEditManager::onToggleMixed));
    multiEditBtn->setID("multi-edit-btn"_spr);
    multiEditBtn->setVisible(showMultiEdit);

    menu->addChild(multiEditBtn);
    menu->updateLayout();

    m_popup->m_mainLayer->addChild(menu);
    handleTouchPriority(m_popup);
}

void MultiEditManager::addSideMenuButton(CCMenuItem* button) {
    m_sideMenu->addChild(button);
    m_sideMenu->updateLayout();
}

void MultiEditManager::addInput(CCTextInputNode* input, int property) {
    input->setUserObject("property"_spr, CCInteger::create(property));
    m_inputs[property] = input;
}

void MultiEditManager::addSlider(Slider* slider, int property) {
    slider->setUserObject("property"_spr, CCInteger::create(property));
    m_sliders[property] = slider;
}

void MultiEditManager::addButton(CCMenuItem* button, int property) {
    button->setUserObject("property"_spr, CCInteger::create(property));
    m_buttons[property].push_back(button);
}

void MultiEditManager::addInputBG(CCScale9Sprite* bg, int property) {
    bg->setUserObject("property"_spr, CCInteger::create(property));
    m_inputBGs[property] = bg;
}

void MultiEditManager::addInputLabel(CCLabelBMFont* label, int property) {
    label->setUserObject("property"_spr, CCInteger::create(property));
    m_inputLabels[property] = label;
}

void MultiEditManager::setupMixed() {
    if (m_hasSetupMixed) return;
    m_hasSetupMixed = true;

    if (m_gameObjects.size() <= 1) return;

    for (auto const& [property, input] : m_inputs) {
        bool isMixed = false;
        float firstValue = Trigger::getProperty(m_gameObjects[0], property);

        for (auto object : m_gameObjects) {
            float value = Trigger::getProperty(object, property);

            if (value != firstValue) {
                isMixed = true;
                break;
            }
        }

        if (isMixed) makeMixed(property);
    }
}

void MultiEditManager::makeMixed(int property) {
    if (m_inputs.find(property) == m_inputs.end()) return;
    CCTextInputNode* input = m_inputs[property];

    CCMenuItemSpriteExtra* btn = createMixedButton(property);

    btn->setPosition(input->getPosition() - m_popup->m_buttonMenu->getPosition() + m_buttonOffset);
    btn->setID("mixed-input-btn"_spr);
    btn->setTag(property);

    if (typeinfo_cast<CreateParticlePopup*>(m_popup)) {
        btn->setVisible(input->getParent()->isVisible());
    } else {
        btn->setVisible(input->isVisible());
    }

    if (m_groups.has_value()) {
        auto& groups = m_groups.value();

        for (auto group : groups) {
            if (!group->containsObject(input)) continue;

            group->addObject(btn);
            break;
        }
    }

    if (m_pages.has_value()) {
        auto& pages = m_pages.value();

        for (size_t i = 0; i < pages.size(); i++) {
            if (auto particlePopup = typeinfo_cast<CreateParticlePopup*>(m_popup)) {
                CCDictionary* inputDict = static_cast<CCDictionary*>(particlePopup->m_inputDicts->objectAtIndex(i + 1));
                if (!inputDict) continue;

                auto input = inputDict->objectForKey(property);
                if (!input || !input->isEqual(input)) continue;
            } else {
                if (!pages[i]->containsObject(input)) continue;
            }

            pages[i]->addObject(btn);
            break;
        }
    }

    m_mixedButtons[property] = btn;

    input->removeFromParent();
    m_popup->m_buttonMenu->addChild(btn);

    toggleSlider(property, false);
    toggleButtons(property, false);
}

void MultiEditManager::removeMixed(int property, float newValue) {
    if (!m_inputs.contains(property)) return;
    if (!m_mixedButtons.contains(property)) return;

    CCTextInputNode* input = m_inputs[property];
    CCMenuItemSpriteExtra* btn = m_mixedButtons[property];

    m_mixedButtons.erase(property);

    m_inputParentNode->addChild(input);
    btn->removeFromParent();

    toggleSlider(property, true);
    toggleButtons(property, true);

    setInputValue(input, newValue);
}

void MultiEditManager::onMixed(CCObject* sender) {
    if (m_gameObjects.size() <= 1) {
        if (!m_mixedNotification) {
            m_mixedNotification = Notification::create("Only one object selected", NotificationIcon::Error, 2);
        }

        m_mixedNotification->show();
        return;
    }

    GEODE_UNWRAP_OR_ELSE(property, err, getProperty(static_cast<CCNode*>(sender))) return;

    auto callback = [sender, property, this](std::optional<float> value) {
        bool shouldReplaceInputWithButton = typeinfo_cast<CCTextInputNode*>(sender) && !value.has_value();
        bool shouldReplaceButtonWithInput = typeinfo_cast<CCMenuItemSpriteExtra*>(sender) && value.has_value();

        if (shouldReplaceInputWithButton) {
            makeMixed(property);
        } else if (shouldReplaceButtonWithInput) {
            removeMixed(property, value.value());
        }

        toggleSlider(property, value.has_value());
        toggleButtons(property, value.has_value());

        if (m_customCallback.has_value()) {
            m_customCallback.value()(property, value);
        }
    };

    auto popup = MixedInputPopup::create(m_gameObjects, property, callback);
    popup->m_noElasticity = true;
    popup->show();
}

void MultiEditManager::onToggleMixed(CCObject* sender) {
    m_isMixedEnabled = !m_isMixedEnabled;
    GLubyte opacity = m_isMixedEnabled ? 140 : 70;

    const int ACTION_TAG = 1234;

    for (auto const& [property, bg] : m_inputBGs) {
        if (!bg) continue;
        bg->stopActionByTag(ACTION_TAG);
        bg->setOpacity(opacity);

        CCAction* action;

        if (m_isMixedEnabled) {
            action = CCRepeatForever::create(
                CCSequence::create(
                    CCFadeTo::create(0.8, 80),
                    CCFadeTo::create(0.8, 120),
                    nullptr
                )
            );
        } else {
            action = CCFadeTo::create(0.8, 100);
        }

        action->setTag(ACTION_TAG);
        bg->runAction(action);
    }
}

void MultiEditManager::toggleSlider(int property, bool isEnabled) {
    if (m_sliders.find(property) == m_sliders.end()) return;
    Slider* slider = m_sliders[property];

    slider->getThumb()->setPosition(9999, 0);

    slider->m_groove->setOpacity(isEnabled ? 255 : 100);
    slider->getThumb()->setOpacity(isEnabled ? 255 : 0);
    slider->m_sliderBar->setOpacity(isEnabled ? 255 : 0);
}

void MultiEditManager::toggleButtons(int property, bool isEnabled) {
    if (m_buttons.find(property) == m_buttons.end()) return;
    auto& buttons = m_buttons[property];

    for (auto button : buttons) {
        button->setEnabled(isEnabled);
        button->setOpacity(isEnabled ? 255 : 100);
    }
}

/* public static functions */

Result<int> MultiEditManager::getProperty(CCNode* node) {
    if (!node) return Err("Node is nullptr");
    auto ccInt = typeinfo_cast<CCInteger*>(node->getUserObject("property"_spr));

    if (!ccInt) return Err("Node doesn't have property");
    return Ok(ccInt->getValue());
}

void MultiEditManager::setInputValue(CCTextInputNode* input, float value) {
    int decimalPlaces = input->m_decimalPlaces;

    if (GEODE_UNWRAP_IF_OK(property, getProperty(input))) {
        decimalPlaces = Trigger::getPropertyDecimalPlaces(nullptr, property);
    }

    float factor = std::pow(10, decimalPlaces);
    float newValue = std::floor(value * factor) / factor;

    std::string str = nk::toString(newValue, decimalPlaces, false);
    input->setString(str);
}

void MultiEditManager::setSliderValue(Slider* slider, float value, float min, float max) {
    float newValue = (value - min) / (max - min);
    slider->setValue(std::clamp(newValue, 0.f, 1.f));
}

CCMenuItemToggler* MultiEditManager::createSideMenuButton(const char* sprName, CCObject* handler, SEL_MenuHandler selector) {
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

    auto btn = CCMenuItemToggler::create(offSpr, onSpr, handler, selector);
    btn->toggle(false);

    return btn;
}

MultiEditManager* MultiEditManager::create(FLAlertLayer* popup, CCArray* gameObjects) {
    auto ret = new MultiEditManager();
    if (ret && ret->init(popup, gameObjects)) {
        s_instance = ret;
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

Result<MultiEditManager*> MultiEditManager::get() {
    if (!s_instance) return Err("No instance found");
    return Ok(s_instance);
}

/* enter mixed input when clicking on an input when mixed mode is enabled */

class $modify(CCTextInputNode) {
    bool ccTouchBegan(CCTouch* touch, CCEvent* event) {
        GEODE_UNWRAP_OR_ELSE(property, err, MultiEditManager::getProperty(this)) return CCTextInputNode::ccTouchBegan(touch, event);
        GEODE_UNWRAP_OR_ELSE(mem, err, MultiEditManager::get()) return CCTextInputNode::ccTouchBegan(touch, event);

        if (!mem->isMixedEnabled()) return CCTextInputNode::ccTouchBegan(touch, event);

        auto bounds = m_textField->boundingBox();
        auto touchLocation = m_textField->convertToNodeSpace(touch->getLocation()) - m_textField->getContentSize() / 2;
        if (!bounds.containsPoint(touchLocation)) return true;

        mem->onMixed(this);

        return true;
    }
};

/* right click on input to enter mixed input mode (windows only) */

#ifdef GEODE_IS_WINDOWS
#include <Geode/modify/CCEGLView.hpp>

class $modify(CCEGLViewTrigger, CCEGLView) {
    $override void onGLFWMouseCallBack(GLFWwindow* window, int button, int action, int mods) {
        CCEGLView::onGLFWMouseCallBack(window, button, action, mods);

        if (button != GLFW_MOUSE_BUTTON_RIGHT) return;
        if (action != GLFW_RELEASE) return;

        GEODE_UNWRAP_OR_ELSE(mem, err, MultiEditManager::get()) return;

        CCPoint mousePosition = getMousePos();
        auto& inputNodes = mem->getInputs();

        for (auto const& [key, input] : inputNodes) {
            if (!input->isVisible()) continue;

            auto parent = input->getParent();
            if (parent && !parent->isVisible()) continue;

            CCPoint inputPosition = input->m_textField->convertToWorldSpace({0, 0});
            CCSize inputSize = input->m_textField->getContentSize();
            CCRect inputRect = CCRect(inputPosition, inputSize);

            if (inputRect.containsPoint(mousePosition)) {
                mem->onMixed(input);
                return;
            }
        }
    }
};
#endif