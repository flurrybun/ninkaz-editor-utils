#pragma once

#include <Geode/Geode.hpp>
using namespace geode::prelude;

struct PropertyBounds {
    float min;
    float max;

    PropertyBounds(float min, float max) : min(min), max(max) {}

    static PropertyBounds toInf(float min) {
        return PropertyBounds(min, std::numeric_limits<float>::max());
    }
    static PropertyBounds negInfTo(float max) {
        return PropertyBounds(std::numeric_limits<float>::lowest(), max);
    }
    static PropertyBounds negInfToInf() {
        return PropertyBounds(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::max());
    }
};

/**
 * Abstract base class for implementing multi-edit functionality.
 * 
 * Inherit this class in your hook's Fields and implement the abstract methods. Call registerSelf() on init
 * and setupMixed() once all the inputs/sliders/buttons have been registered.
 * 
 * It can also be used in other cases since it acts like a database of all the inputs, sliders, buttons, etc.
 * in all trigger popups.
 */
class MultiEditContext {
private:
    static inline std::unordered_map<CCNode*, MultiEditContext*> s_registry;
    CCNode* m_self;
    FLAlertLayer* m_alertLayer;
    CCNode* m_inputParent;

protected:
    std::vector<CCMenuItem*> m_sideButtons;
    CCArrayExt<GameObject*> m_gameObjects;
    Ref<Notification> m_mixedNotification;
    bool m_isMixedEnabled = false;
    bool m_hasSetupMixed = false;

    std::map<int, Ref<CCTextInputNode>> m_inputs;
    std::map<int, Slider*> m_sliders;
    std::map<int, std::vector<CCMenuItem*>> m_buttons;
    std::map<int, CCScale9Sprite*> m_inputBGs;
    std::map<int, CCLabelBMFont*> m_inputLabels;
    std::map<int, CCMenuItemSpriteExtra*> m_mixedButtons;

    CCPoint m_buttonOffset = {0, 0};

    /**
     * CCArrays of CCArrays of CCNodes.
     * Some popups manage toggling groups of nodes via toggling the visibility of all nodes in a CCArray.
     * You can add these arrays here so nodes added have their visibility updated for free.
     * MultiEditContext does not manage the memory of these arrays.
     */
    CCArray* m_groups;
    CCArray* m_pages;

    /**
     * Register this context in the global registry.
     */
    void registerSelf(FLAlertLayer* self);
    void registerSelf(CCNode* self, FLAlertLayer* alertLayer, CCNode* inputParent);

    CCMenuItemSpriteExtra* createMixedButton(int property);
    void updateSideMenuButtons();

    void updateMixedUI(int property, std::optional<float> value);
    void makeMixed(int property);
    void removeMixed(int property, float newValue);
    void onToggleMixed(CCObject* sender);
    void toggleSlider(int property, bool isEnabled);
    void toggleButtons(int property, bool isEnabled);

public:
    virtual ~MultiEditContext();
    
    /**
     * Get the value of a property for a specific game object.
     * @param object The game object
     * @param property The property ID
     * @return The property value
     */
    virtual float getProperty(GameObject* object, int property) = 0;
    
    /**
     * Set the value of a property for a specific game object.
     * @param object The game object
     * @param property The property ID
     * @param value The new value to set
     */
    virtual void setProperty(GameObject* object, int property, float value) = 0;
    
    /**
     * Check if a game object supports a given property.
     * @param object The game object to check
     * @param property The property ID
     * @return If the property is valid for this object
     */
    virtual bool hasProperty(GameObject* object, int property) = 0;

    /**
     * Get the number of decimal places to display for a property.
     * @param property The property ID
     * @return Number of decimal places
     */
    virtual int getPropertyDecimalPlaces(int property) = 0;

    /**
     * Get the valid bounds for a property.
     * @param property The property ID
     * @return A PropertyBounds struct with a min and max
     */
    virtual PropertyBounds getPropertyBounds(int property) = 0;

    /**
     * Called when the MixedInputPopup applies changes, to handle updating the UI.
     * @param property The property that was modified
     * @param value The new value, or nullopt if values are still mixed
     */
    virtual void onMixedInputApplied(int property, std::optional<float> value) = 0;

    /**
     * Get the array of game objects to multi-edit.
     * @return The array of GameObjects, or nullptr if not applicable
     */
    virtual CCArray* getObjectArray() = 0;

    void setupMixed();
    void onMixed(CCObject* sender);

    void addSideMenuButton(CCMenuItem* button);
    void addInput(CCTextInputNode* input, int property);
    void addSlider(Slider* slider, int property);
    void addButton(CCMenuItem* button, int property);
    void addInputBG(CCScale9Sprite* bg, int property);
    void addInputLabel(CCLabelBMFont* label, int property);

    bool isMixedEnabled() { return m_isMixedEnabled; }
    CCArrayExt<GameObject*>& getGameObjects() { return m_gameObjects; }
    std::map<int, Ref<CCTextInputNode>>& getInputs() { return m_inputs; }
    std::map<int, Slider*>& getSliders() { return m_sliders; }
    std::map<int, std::vector<CCMenuItem*>>& getButtons() { return m_buttons; }
    std::map<int, CCScale9Sprite*>& getInputBGs() { return m_inputBGs; }
    std::map<int, CCLabelBMFont*>& getInputLabels() { return m_inputLabels; }
    std::map<int, CCMenuItemSpriteExtra*>& getMixedButtons() { return m_mixedButtons; }

    CCTextInputNode* getInput(int property);
    Slider* getSlider(int property);
    std::vector<CCMenuItem*> getButtonsForProperty(int property);
    CCScale9Sprite* getInputBG(int property);
    CCLabelBMFont* getInputLabel(int property);
    CCMenuItemSpriteExtra* getMixedButton(int property);

    void setButtonOffset(CCPoint offset) { m_buttonOffset = offset; }
    void setGroups(CCArray* groups) { m_groups = groups; }
    void setPages(CCArray* pages) { m_pages = pages; }

    static std::optional<int> getPropertyID(CCNode* node);
    static void setInputValue(CCTextInputNode* input, float value, int decimalPlaces);
    static void setSliderValue(Slider* slider, float value, float min, float max);
    static CCMenuItemToggler* createSideMenuButton(
        const char* sprName, std::function<void (CCMenuItemToggler*)> callback
    );
    static bool isTriggerPopup(SetupTriggerPopup* popup);

    static MultiEditContext* get(CCNode* popup);
    static bool hasContext(CCNode* popup);
};
