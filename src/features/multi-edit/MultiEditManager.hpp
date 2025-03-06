#pragma once

#include <Geode/Geode.hpp>
using namespace geode::prelude;

class MultiEditManager : public CCObject {
private:

    static MultiEditManager* s_instance;

    FLAlertLayer* m_popup;
    CCMenu* m_sideMenu;
    CCArrayExt<GameObject*> m_gameObjects;
    Ref<Notification> m_mixedNotification;
    bool m_isMixedEnabled = false;
    bool m_hasSetupMixed = false;

    std::map<int, Ref<CCTextInputNode>> m_inputs;
    std::map<int, Slider*> m_sliders;
    std::map<int, std::vector<CCMenuItemSpriteExtra*>> m_buttons;
    std::map<int, CCScale9Sprite*> m_inputBGs;
    std::map<int, CCLabelBMFont*> m_inputLabels;
    std::map<int, CCMenuItemSpriteExtra*> m_mixedButtons;

    std::optional<CCArrayExt<CCArray*>> m_groups;
    std::optional<CCArrayExt<CCArray*>> m_pages;
    std::optional<std::function<void(int, std::optional<float>)>> m_customCallback;

    bool init(FLAlertLayer* popup, CCArray* gameObjects);
    ~MultiEditManager();

    CCMenuItemSpriteExtra* createMixedButton(int property);

public:

    void setupSideMenu();
    void addSideMenuButton(CCMenuItem* button);
    CCArray* getSideMenuButtons() { return m_sideMenu->getChildren(); }

    void addInput(CCTextInputNode* input, int property);
    void addSlider(Slider* slider, int property);
    void addButton(CCMenuItemSpriteExtra* button, int property);
    void addInputBG(CCScale9Sprite* bg, int property);
    void addInputLabel(CCLabelBMFont* label, int property);

    bool isMixedEnabled() { return m_isMixedEnabled; }
    std::map<int, Ref<CCTextInputNode>>& getInputs() { return m_inputs; }
    std::map<int, Slider*>& getSliders() { return m_sliders; }
    std::map<int, std::vector<CCMenuItemSpriteExtra*>>& getButtons() { return m_buttons; }
    std::map<int, CCScale9Sprite*>& getInputBGs() { return m_inputBGs; }
    std::map<int, CCLabelBMFont*>& getInputLabels() { return m_inputLabels; }
    std::map<int, CCMenuItemSpriteExtra*>& getMixedButtons() { return m_mixedButtons; }

    void setGroups(CCArrayExt<CCArray*> groups) { m_groups.emplace(groups); }
    void setPages(CCArrayExt<CCArray*> pages) { m_pages.emplace(pages); }
    void setCallback(std::function<void(int property, std::optional<float> value)> callback) { m_customCallback = callback; }

    void setupMixed();
    void makeMixed(int property);
    void removeMixed(int property, float newValue);
    void onMixed(CCObject* sender);
    void onToggleMixed(CCObject* sender);

    void toggleSlider(int property, bool isEnabled);
    void toggleButtons(int property, bool isEnabled);

    static Result<int> getProperty(CCNode* node);
    static void setInputValue(CCTextInputNode* input, float value);
    static void setSliderValue(Slider* slider, float value, float min, float max);
    static CCMenuItemToggler* createSideMenuButton(const char* sprName, CCObject* handler, SEL_MenuHandler selector);

    static MultiEditManager* create(FLAlertLayer* popup, CCArray* gameObjects);
    static Result<MultiEditManager*> get();
};
