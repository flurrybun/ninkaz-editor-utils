#include "MultiEditContext.hpp"
#include "MixedInputPopup.hpp"
#include "../../misc/StringUtils.hpp"

#include <Geode/modify/CCTextInputNode.hpp>

void MultiEditContext::registerSelf(FLAlertLayer* self, CCMenu* buttonMenu) {
    m_self = self;
    m_alertLayer = self;
    m_inputParent = self->m_mainLayer;
    if (buttonMenu) m_buttonMenu = buttonMenu;
    else m_buttonMenu = self->m_buttonMenu;

    s_registry[self] = this;
}

void MultiEditContext::registerSelf(
    CCNode* self, FLAlertLayer* alertLayer, CCNode* inputParent, CCMenu* buttonMenu
) {
    m_self = self;
    m_alertLayer = alertLayer;
    m_inputParent = inputParent;
    if (buttonMenu) m_buttonMenu = buttonMenu;
    else m_buttonMenu = alertLayer->m_buttonMenu;

    s_registry[self] = this;
}

MultiEditContext::~MultiEditContext() {
    if (m_self) {
        s_registry.erase(m_self);
        m_self = nullptr;
    }
}

CCMenuItemSpriteExtra* MultiEditContext::createMixedButton(int property) {
    auto spr = CCLabelBMFont::create("Mixed", "bigFont.fnt");

    if (auto input = m_inputs[property]) {
        spr->limitLabelWidth(input->m_maxLabelWidth, input->getScale(), 0);
        spr->setScale(spr->getScale() * m_popupScale);
    }

    auto btn = CCMenuItemExt::createSpriteExtra(spr, [this](CCObject* sender) {
        onMixed(sender);
    });
    btn->setID("mixed-input-btn"_spr);
    btn->setUserObject("property"_spr, CCInteger::create(property));

    return btn;
}

void MultiEditContext::updateSideMenuButtons() {
    // would it be easier to just add the button to a menu with an axis layout? absolutely.
    // is this what i originally did? of course.
    // however, you see, after several hours i just could not get touch priority to work properly
    // using handleTouchPriority(m_alertLayer) breaks pulse and particle triggers
    // so whatever i'll just do it this way

    // CCSize popupSize = m_popupSize
    //     ? *m_popupSize
    //     : m_alertLayer->m_mainLayer->getChildByType<CCScale9Sprite>(0)->getContentSize();
    // CCPoint popupCenter = m_popupCenter
    //     ? *m_popupCenter
    //     : CCPoint(CCDirector::sharedDirector()->getWinSize() / 2);

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

        // button->setPosition(
        //     popupCenter
        //     + ccp(popupSize.width / 2, -popupSize.height / 2) * m_popupScale
        //     + button->getContentSize() / 2
        //     + ccp(5, 35 * i) * m_popupScale
        //     - m_buttonMenu->getPosition()
        // );
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
    bool shouldMakeButtonFromInput = !value && m_inputs.contains(property);
    bool shouldMakeInputFromButton = value && m_mixedButtons.contains(property);

    if (shouldMakeButtonFromInput) {
        makeMixed(property);
    } else if (shouldMakeInputFromButton) {
        removeMixed(property, *value);
    }

    toggleSlider(property, value.has_value());
    toggleButtons(property, value.has_value());
}

void MultiEditContext::makeMixed(int property) {
    if (m_inputs.find(property) == m_inputs.end()) return;
    if (m_mixedButtons.contains(property)) return;
    
    CCTextInputNode* input = m_inputs[property];

    CCMenuItemSpriteExtra* btn = createMixedButton(property);

    btn->setPosition((input->getPosition() * m_popupScale) - m_buttonMenu->getPosition() + m_buttonOffset);
    btn->setID("mixed-input-btn"_spr);
    btn->setTag(property);

    if (typeinfo_cast<CreateParticlePopup*>(m_alertLayer)) {
        btn->setVisible(input->getParent()->isVisible());
    } else {
        btn->setVisible(input->isVisible());
    }

    addToGroup(btn, input);
    addToPage(btn, input);

    m_mixedButtons[property] = btn;

    input->removeFromParent();
    m_buttonMenu->addChild(btn);

    toggleSlider(property, false);
    toggleButtons(property, false);
}

void MultiEditContext::removeMixed(int property, float newValue) {
    if (!m_inputs.contains(property)) return;
    if (!m_mixedButtons.contains(property)) return;

    CCTextInputNode* input = m_inputs[property];
    CCMenuItemSpriteExtra* btn = m_mixedButtons[property];

    m_mixedButtons.erase(property);

    m_inputParent->addChild(input);
    btn->removeFromParent();

    toggleSlider(property, true);
    toggleButtons(property, true);

    int decimalPlaces = getPropertyDecimalPlaces(property);
    setInputValue(input, newValue, decimalPlaces);
}

void MultiEditContext::onToggleMixed(CCObject* sender) {
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

    if (CCScene::get()->getChildByType<MixedInputPopup>(0)) return;

    std::optional<int> property = getPropertyID(static_cast<CCNode*>(sender));
    if (!property) return;

    auto popup = MixedInputPopup::create(this, *property);
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

void MultiEditContext::addInputLabel(CCLabelBMFont* label, int property) {
    if (!label) return;
    label->setUserObject("property"_spr, CCInteger::create(property));
    m_inputLabels[property] = label;
}

CCTextInputNode* MultiEditContext::getInput(int property) {
    if (m_inputs.find(property) == m_inputs.end()) return nullptr;
    return m_inputs[property];
}

Slider* MultiEditContext::getSlider(int property) {
    if (m_sliders.find(property) == m_sliders.end()) return nullptr;
    return m_sliders[property];
}

std::vector<CCMenuItem*> MultiEditContext::getButtonsForProperty(int property) {
    if (m_buttons.find(property) == m_buttons.end()) return {};
    return m_buttons[property];
}

CCScale9Sprite* MultiEditContext::getInputBG(int property) {
    if (m_inputBGs.find(property) == m_inputBGs.end()) return nullptr;
    return m_inputBGs[property];
}

CCLabelBMFont* MultiEditContext::getInputLabel(int property) {
    if (m_inputLabels.find(property) == m_inputLabels.end()) return nullptr;
    return m_inputLabels[property];
}

CCMenuItemSpriteExtra* MultiEditContext::getMixedButton(int property) {
    if (m_mixedButtons.find(property) == m_mixedButtons.end()) return nullptr;
    return m_mixedButtons[property];
}

void MultiEditContext::addToGroup(CCNode* node, CCNode* nodeInGroup) {
    if (!m_groups) return;

    for (size_t i = 0; i < m_groups->count(); i++) {
        CCArray* group = static_cast<CCArray*>(m_groups->objectAtIndex(i));

        if (group->containsObject(nodeInGroup)) {
            group->addObject(node);
            break;
        }
    }
}

void MultiEditContext::addToPage(CCNode* node, CCNode* nodeInPage) {
    if (!m_pages) return;

    if (auto particlePopup = typeinfo_cast<CreateParticlePopup*>(m_alertLayer)) {
        for (size_t i = 0; i < particlePopup->m_inputDicts->count(); i++) {
            CCDictionary* inputDict = static_cast<CCDictionary*>(particlePopup->m_inputDicts->objectAtIndex(i + 1));
            if (!inputDict) continue;

            int property = MultiEditContext::getPropertyID(nodeInPage).value_or(-1);
            if (property == -1) continue;

            auto dictInput = inputDict->objectForKey(property);
            if (!dictInput || dictInput != nodeInPage) continue;

            static_cast<CCArray*>(m_pages->objectAtIndex(i))->addObject(node);
            break;
        }

        return;
    }

    for (size_t i = 0; i < m_pages->count(); i++) {
        CCArray* page = static_cast<CCArray*>(m_pages->objectAtIndex(i));

        if (page->containsObject(nodeInPage)) {
            page->addObject(node);
            break;
        }
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
    const char* sprName, std::function<void (CCMenuItemToggler*)> callback
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

    auto btn = CCMenuItemExt::createToggler(onSpr, offSpr, callback);
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
    return true;
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

bool MultiEditContext::hasContext(CCNode* popup) {
    return get(popup) != nullptr;
}

/* enter mixed input when clicking on an input when mixed mode is enabled */

bool mousePosOverBG(CCScale9Sprite* bg) {
    if (!nodeIsVisible(bg)) return false;

    auto touchPos = getMousePos();
    auto blPos = bg->convertToWorldSpace({0, 0});
    auto trPos = bg->convertToWorldSpace(bg->getContentSize());

    return (
        touchPos.x >= blPos.x && touchPos.x <= trPos.x &&
        touchPos.y >= blPos.y && touchPos.y <= trPos.y
    );
}

class $modify(CCTextInputNode) {
    static void onModify(auto& self) {
        // run before geode's input node fix
        (void)self.setHookPriority("CCTextInputNode::ccTouchBegan", Priority::EarlyPost);
    }

    $override
    bool ccTouchBegan(CCTouch* touch, CCEvent* event) {
        std::optional<int> property = MultiEditContext::getPropertyID(this);
        if (!property) return CCTextInputNode::ccTouchBegan(touch, event);

        auto ctx = MultiEditContext::getFromChild(this);
        if (!ctx) return CCTextInputNode::ccTouchBegan(touch, event);

        if (!ctx->isMixedEnabled()) return CCTextInputNode::ccTouchBegan(touch, event);

        auto bg = ctx->getInputBG(*property);
        if (!bg) return CCTextInputNode::ccTouchBegan(touch, event);

        if (!mousePosOverBG(bg)) return CCTextInputNode::ccTouchBegan(touch, event);

        ctx->onMixed(this);

        return true;
    }
};

/* right click on input to enter mixed input mode */

void onRightClick() {
    auto popup = CCScene::get()->getChildByType<FLAlertLayer>(-1);
    if (!popup) return;

    auto ctx = MultiEditContext::get(popup);
    if (!ctx) return;

    for (auto const& [key, bg] : ctx->getInputBGs()) {
        if (!mousePosOverBG(bg)) continue;

        auto input = ctx->getInput(key);
        if (!input) return;

        ctx->onMixed(input);
        return;
    }
}

#if defined(GEODE_IS_WINDOWS)
#include <Geode/modify/CCEGLView.hpp>

class $modify(CCEGLViewTrigger, CCEGLView) {
    $override void onGLFWMouseCallBack(GLFWwindow* window, int button, int action, int mods) {
        CCEGLView::onGLFWMouseCallBack(window, button, action, mods);

        if (button != GLFW_MOUSE_BUTTON_RIGHT) return;
        if (action != GLFW_RELEASE) return;

        onRightClick();
    }
};
#elif defined(GEODE_IS_MACOS)
#include <objc/message.h>

void rightMouseUpHook(void* self, SEL sel, void* event) {
    queueInMainThread([] {
        onRightClick();
    });
    reinterpret_cast<void(*)(void*, SEL, void*)>(objc_msgSend)(self, sel, event);
}

$execute {
    if (auto hook = ObjcHook::create("EAGLView", "rightMouseUp:", &rightMouseUpHook, tulip::hook::HookMetadata())) {
        (void)Mod::get()->claimHook(hook.unwrap());
    }
}
#endif
