#include "MultiEditContext.hpp"
#include "MixedInputPopup.hpp"
#include "../../misc/StringUtils.hpp"

void MultiEditContext::registerSelf(FLAlertLayer* self, CCMenu* buttonMenu) {
    registerSelf(self, self, buttonMenu);
}

void MultiEditContext::registerSelf(CCNode* self, FLAlertLayer* alertLayer, CCMenu* buttonMenu) {
    m_self = self;
    m_alertLayer = alertLayer;
    if (buttonMenu) m_buttonMenu = buttonMenu;
    else m_buttonMenu = alertLayer->m_buttonMenu;

    s_registry[self] = this;

    m_mouseListener = MouseInputEvent().listen([this](MouseInputData& data) {
        if (
            data.button != MouseInputData::Button::Left ||
            CCScene::get()->getChildByIndex(-1) != m_alertLayer
        ) return ListenerResult::Propagate;

        if (data.action == MouseInputData::Action::Press) {
            // don't start a new press while one is still held down
            if (m_pressedInput) return ListenerResult::Propagate;

            for (const auto& [property, bg] : m_inputBGs) {
                if (!hoveringOverBG(bg)) continue;

                auto input = getInput(property);
                if (!input) break;
                if (!m_isMixedEnabled && !isMixed(property)) break;

                m_pressedInput = property;
                m_pressedInputButton = data.button;
                closeInput(input);
                depressInput(input, bg, true);

                return ListenerResult::Stop;
            }

            return ListenerResult::Propagate;
        }

        if (
            data.action != MouseInputData::Action::Release ||
            !m_pressedInput ||
            data.button != m_pressedInputButton
        ) return ListenerResult::Propagate;

        int property = *m_pressedInput;
        m_pressedInput.reset();

        auto input = getInput(property);
        auto bg = getInputBG(property);
        if (!input || !bg) return ListenerResult::Propagate;

        depressInput(input, bg, false);

        if (hoveringOverBG(bg)) {
            onMixed(input);
            return ListenerResult::Stop;
        }

        return ListenerResult::Propagate;
    });
}

MultiEditContext::~MultiEditContext() {
    if (m_self) {
        s_registry.erase(m_self);
        m_self = nullptr;
    }
}

void MultiEditContext::updateSideMenuButtons() {
    // would it be easier to just add the button to a menu with an axis layout? absolutely.
    // is this what i originally did? of course.
    // however, you see, after several hours i just could not get touch priority to work properly
    // using handleTouchPriority(m_alertLayer) breaks pulse and particle triggers
    // so whatever i'll just do it this way

    if (typeinfo_cast<HSVLiveOverlay*>(m_alertLayer)) {
        if (m_sideButtons.empty()) return;

        auto btn = m_sideButtons[0];

        btn->removeFromParent();
        m_buttonMenu->addChild(btn);
        btn->setPosition({ 152.6f, -86.4f });
        btn->setScale(0.5f);

        return;
    }

    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    CCSize popupSize = m_alertLayer->m_mainLayer->getChildByType<CCScale9Sprite>(0)->getContentSize();

    std::sort(m_sideButtons.begin(), m_sideButtons.end(), [](CCMenuItem* a, CCMenuItem* b) {
        return a->getTag() < b->getTag();
    });

    for (size_t i = 0; i < m_sideButtons.size(); i++) {
        auto button = m_sideButtons[i];

        button->removeFromParent();
        m_buttonMenu->addChild(button);

        button->setPosition(
            winSize / 2
            + ccp(popupSize.width / 2, -popupSize.height / 2)
            + button->getContentSize() / 2
            + ccp(5, 35 * i)
            - m_buttonMenu->getPosition()
        );
    }
}

void MultiEditContext::updateMixedUI(int property, std::optional<float> value) {
    bool shouldMakeInputMixed = !value && m_inputs.contains(property);
    bool shouldRestoreInput = value && m_mixedProperties.contains(property);

    if (shouldMakeInputMixed) {
        makeMixed(property);
    } else if (shouldRestoreInput) {
        removeMixed(property, *value);
    }

    toggleSlider(property, value.has_value());
    toggleButtons(property, value.has_value());
}

void MultiEditContext::makeMixed(int property) {
    if (m_mixedProperties.contains(property)) return;

    CCTextInputNode* input = getInput(property);
    if (!input) return;

    m_mixedProperties.insert(property);

    input->setTouchEnabled(false);

    // do this to prevent any text change callbacks from firing
    input->m_textLabel->setString("Mixed");
    input->m_textLabel->setColor(input->m_textColor);

    toggleSlider(property, false);
    toggleButtons(property, false);
}

void MultiEditContext::removeMixed(int property, float newValue) {
    if (!m_mixedProperties.contains(property)) return;

    CCTextInputNode* input = getInput(property);
    if (!input) return;

    m_mixedProperties.erase(property);

    input->setTouchEnabled(true);

    toggleSlider(property, true);
    toggleButtons(property, true);

    int decimalPlaces = getPropertyDecimalPlaces(property);
    setInputValue(input, newValue, decimalPlaces);
}

void MultiEditContext::onToggleMixed(CCObject* sender) {
    m_isMixedEnabled = !m_isMixedEnabled;
    GLubyte opacity = m_isMixedEnabled ? 140 : 70;

    const int ACTION_TAG = 1234;

    closeAllInputs();

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

void MultiEditContext::toggleSlider(int property, bool isEnabled) {
    if (m_sliders.find(property) == m_sliders.end()) return;
    Slider* slider = m_sliders[property];

    slider->getThumb()->setPosition(9999, 0);

    slider->m_groove->setOpacity(isEnabled ? 255 : 100);
    slider->getThumb()->setOpacity(isEnabled ? 255 : 0);
    slider->m_sliderBar->setOpacity(isEnabled ? 255 : 0);
}

void MultiEditContext::toggleButtons(int property, bool isEnabled) {
    if (m_buttons.find(property) == m_buttons.end()) return;
    auto& buttons = m_buttons[property];

    for (auto button : buttons) {
        button->setEnabled(isEnabled);
        button->setOpacity(isEnabled ? 255 : 100);
    }
}

void MultiEditContext::setupMixed() {
    if (m_hasSetupMixed) return;
    m_hasSetupMixed = true;

    SetupMixedEvent().send(this);

    if (auto objArray = getObjectArray()) {
        m_gameObjects.inner()->removeAllObjects();
        m_gameObjects.inner()->addObjectsFromArray(objArray);
    }

    if (m_gameObjects.size() <= 1) return;

    auto multiEditBtn = createSideMenuButton("multi-edit-btn.png"_spr, [this](CCObject* sender) {
        onToggleMixed(sender);
    });
    multiEditBtn->setID("multi-edit-btn"_spr);

    addSideMenuButton(multiEditBtn);

    for (const auto& [property, input] : m_inputs) {
        bool isMixed = false;
        float firstValue = getProperty(m_gameObjects[0], property);

        for (auto object : m_gameObjects) {
            float value = getProperty(object, property);

            if (value != firstValue) {
                isMixed = true;
                break;
            }
        }

        if (isMixed) makeMixed(property);
    }
}

void MultiEditContext::onMixed(CCObject* sender) {
    if (m_gameObjects.size() <= 1) {
        if (!m_mixedNotification) {
            m_mixedNotification = Notification::create("Only one object selected", NotificationIcon::Error, 2);
        }

        m_mixedNotification->show();
        return;
    }

    if (CCScene::get()->getChildByType<me::MixedInputPopup>(0)) return;

    std::optional<int> property = getPropertyID(static_cast<CCNode*>(sender));
    if (!property) return;

    closeAllInputs();

    auto popup = me::MixedInputPopup::create(this, *property);
    popup->m_noElasticity = true;
    popup->show();
}

void MultiEditContext::addSideMenuButton(CCMenuItem* button) {
    m_sideButtons.push_back(button);
    updateSideMenuButtons();
}

void MultiEditContext::addInput(CCTextInputNode* input, int property) {
    if (!input) return;
    input->setUserObject("property"_spr, CCInteger::create(property));
    m_inputs[property] = input;
}

void MultiEditContext::addSlider(Slider* slider, int property) {
    if (!slider) return;
    slider->setUserObject("property"_spr, CCInteger::create(property));
    m_sliders[property] = slider;
}

void MultiEditContext::addButton(CCMenuItem* button, int property) {
    if (!button) return;
    button->setUserObject("property"_spr, CCInteger::create(property));
    m_buttons[property].push_back(button);
}

void MultiEditContext::addInputBG(CCScale9Sprite* bg, int property) {
    if (!bg) return;
    bg->setUserObject("property"_spr, CCInteger::create(property));
    m_inputBGs[property] = bg;
}

void MultiEditContext::addExtraNode(CCNode* node, int property) {
    if (!node) return;
    node->setUserObject("property"_spr, CCInteger::create(property));
    m_extraNodes[property].push_back(node);
}

CCTextInputNode* MultiEditContext::getInput(int property) {
    if (m_inputs.find(property) == m_inputs.end()) return nullptr;
    return m_inputs[property];
}

Slider* MultiEditContext::getSlider(int property) {
    if (m_sliders.find(property) == m_sliders.end()) return nullptr;
    return m_sliders[property];
}

CCScale9Sprite* MultiEditContext::getInputBG(int property) {
    if (m_inputBGs.find(property) == m_inputBGs.end()) return nullptr;
    return m_inputBGs[property];
}

std::vector<Ref<CCNode>> MultiEditContext::getExtraNodesForProperty(int property) {
    if (m_extraNodes.find(property) == m_extraNodes.end()) return {};
    return m_extraNodes[property];
}

bool MultiEditContext::isMixed(int property) {
    return m_mixedProperties.contains(property);
}

template <class T>
static void removeFromPropertyMap(std::map<int, std::vector<Ref<T>>>& map, int property, T* node) {
    auto it = map.find(property);
    if (it == map.end()) return;

    auto& nodes = it->second;
    nodes.erase(std::remove(nodes.begin(), nodes.end(), Ref<T>(node)), nodes.end());
    if (nodes.empty()) map.erase(it);
}

void MultiEditContext::reassignProperty(IDNameInput* input, int newProperty) {
    if (!input) return;

    reassignNode(input->m_textInput, newProperty);
    reassignNode(input->m_groupName, newProperty);
    reassignNode(input->m_groupNameShadow, newProperty);
    reassignNode(input->m_label, newProperty);
    reassignNode(input->m_searchButton, newProperty);
    reassignNode(input->m_nextButton, newProperty);
    reassignNode(input->m_leftButton, newProperty);
    reassignNode(input->m_rightButton, newProperty);
    reassignNode(input->m_background, newProperty);
}

void MultiEditContext::reassignNode(CCNode* node, int newProperty) {
    if (!node) return;

    auto oldProperty = getPropertyID(node);
    if (!oldProperty || *oldProperty == newProperty) return;

    node->setUserObject("property"_spr, CCInteger::create(newProperty));

    if (auto input = typeinfo_cast<CCTextInputNode*>(node)) {
        m_inputs.erase(*oldProperty);
        m_inputs[newProperty] = input;
    } else if (auto slider = typeinfo_cast<Slider*>(node)) {
        m_sliders.erase(*oldProperty);
        m_sliders[newProperty] = slider;
    } else if (auto bg = typeinfo_cast<CCScale9Sprite*>(node)) {
        m_inputBGs.erase(*oldProperty);
        m_inputBGs[newProperty] = bg;
    } else if (auto button = typeinfo_cast<CCMenuItem*>(node)) {
        removeFromPropertyMap(m_buttons, *oldProperty, button);
        m_buttons[newProperty].push_back(button);
    } else {
        removeFromPropertyMap(m_extraNodes, *oldProperty, node);
        m_extraNodes[newProperty].push_back(node);
    }
}

void MultiEditContext::depressInput(CCTextInputNode* input, CCScale9Sprite* bg, bool isPressed) {
    constexpr int ACTION_TAG = hash("ninkaz loves you <3");
    constexpr float SCALE_FACTOR = 1.26f;
    const float DURATION = isPressed ? 0.3f : 0.4f;
    
    auto label = input->m_textLabel;
    bool wasAnimating = label->getActionByTag(ACTION_TAG);

    label->stopActionByTag(ACTION_TAG);
    bg->stopActionByTag(ACTION_TAG);

    if (isPressed && !wasAnimating) {
        m_pressedLabelScale = label->getScale();
        m_pressedBGScale = bg->getScale();
    }

    float scale = isPressed ? m_pressedLabelScale * SCALE_FACTOR : m_pressedLabelScale;
    auto action = CCEaseBounceOut::create(CCScaleTo::create(DURATION, scale));
    action->setTag(ACTION_TAG);
    label->runAction(action);

    scale = isPressed ? m_pressedBGScale * SCALE_FACTOR : m_pressedBGScale;
    action = CCEaseBounceOut::create(CCScaleTo::create(DURATION, scale));
    action->setTag(ACTION_TAG);
    bg->runAction(action);
}

void MultiEditContext::closeAllInputs() {
    for (const auto& [_, input] : m_inputs) {
        closeInput(input);
    }
}

std::optional<int> MultiEditContext::getPropertyID(CCNode* node) {
    if (!node) return std::nullopt;
    auto ccInt = typeinfo_cast<CCInteger*>(node->getUserObject("property"_spr));

    if (!ccInt) return std::nullopt;
    return ccInt->getValue();
}

void MultiEditContext::setInputValue(CCTextInputNode* input, float value, int decimalPlaces) {
    float factor = std::pow(10, decimalPlaces);
    float newValue = std::floor(value * factor) / factor;

    std::string str = nk::toString(newValue, decimalPlaces, false);
    input->setString(str);
}

void MultiEditContext::setSliderValue(Slider* slider, float value, float min, float max) {
    float newValue = (value - min) / (max - min);
    slider->setValue(std::clamp(newValue, 0.f, 1.f));
}

CCMenuItemToggler* MultiEditContext::createSideMenuButton(
    const char* sprName, geode::Function<void (CCMenuItemToggler*)> callback
) {
    auto onSprTop = CCSprite::createWithSpriteFrameName(sprName);
    auto onSpr = CCScale9Sprite::create("GJ_button_02.png");
    onSprTop->setScale(0.7f);
    onSpr->setContentSize({30, 30});
    onSpr->addChildAtPosition(onSprTop, Anchor::Center);

    auto offSprTop = CCSprite::createWithSpriteFrameName(sprName);
    auto offSpr = CCScale9Sprite::create("GJ_button_04.png");
    offSprTop->setScale(0.7f);
    offSpr->setContentSize({30, 30});
    offSpr->addChildAtPosition(offSprTop, Anchor::Center);

    auto btn = CCMenuItemExt::createToggler(onSpr, offSpr, std::move(callback));
    btn->toggle(false);

    return btn;
}

bool MultiEditContext::isTriggerPopup(SetupTriggerPopup* popup) {
    // why does so much stuff inherit SetupTriggerPopup
    if (typeinfo_cast<SelectEventLayer*>(popup)) return false;
    if (typeinfo_cast<CustomizeObjectSettingsPopup*>(popup)) return false;
    if (typeinfo_cast<ColorSelectPopup*>(popup)) {
        return popup->m_gameObject || popup->m_gameObjects;
    }
    if (typeinfo_cast<EditGameObjectPopup*>(popup)) return false;
    if (typeinfo_cast<GJOptionsLayer*>(popup)) return false;
    if (typeinfo_cast<UIOptionsLayer*>(popup)) return false;
    if (typeinfo_cast<UIPOptionsLayer*>(popup)) return false;
    if (typeinfo_cast<UISaveLoadLayer*>(popup)) return false;
    if (typeinfo_cast<EasingSelectPopup*>(popup)) return false;
    return true;
}

void MultiEditContext::closeInput(CCTextInputNode* input) {
    // taken from SetupTriggerPopup::closeInputNodes
    input->m_delegate = nullptr;
    input->onClickTrackNode(false);
}

MultiEditContext* MultiEditContext::get(CCNode* popup) {
    if (!popup) return nullptr;

    auto it = s_registry.find(popup);
    return it != s_registry.end() ? it->second : nullptr;
}

MultiEditContext* MultiEditContext::getFromChild(CCNode* child) {
    while (child) {
        auto it = s_registry.find(child);
        if (it != s_registry.end()) return it->second;

        child = child->getParent();
    }

    return nullptr;
}

/* enter mixed input when clicking on an input when mixed mode is enabled */

bool hoveringOverBG(CCScale9Sprite* bg) {
    if (!nodeIsVisible(bg)) return false;

    auto touchPos = getMousePos();
    auto blPos = bg->convertToWorldSpace({0, 0});
    auto trPos = bg->convertToWorldSpace(bg->getContentSize());

    return (
        touchPos.x >= blPos.x && touchPos.x <= trPos.x &&
        touchPos.y >= blPos.y && touchPos.y <= trPos.y
    );
}
