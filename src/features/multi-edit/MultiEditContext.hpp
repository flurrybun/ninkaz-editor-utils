#pragma once

#include <Geode/Geode.hpp>
#include <Geode/ui/Toggler.hpp>
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
    CCNode* m_self = nullptr;

protected:
    ListenerHandle m_mouseListener;
    CCArrayExt<GameObject*> m_gameObjects;
    FLAlertLayer* m_alertLayer = nullptr;
    Ref<CCNode> m_sideContainer = nullptr;

    constexpr static float SIDE_BUTTON_WIDTH = 30.f;

    bool m_isMixedEnabled = false;
    bool m_hasSetupMixed = false;

    std::map<int, Ref<CCTextInputNode>> m_inputs;
    std::map<int, Ref<Slider>> m_sliders;
    std::map<int, std::vector<Ref<CCMenuItem>>> m_buttons;
    std::map<int, Ref<CCScale9Sprite>> m_inputBGs;
    std::map<int, std::vector<Ref<CCNode>>> m_extraNodes;
    std::set<int> m_mixedProperties;

    std::optional<int> m_pressedInput;
    MouseInputData::Button m_pressedInputButton = MouseInputData::Button::Left;
    float m_pressedLabelScale = 1.f;
    float m_pressedBGScale = 1.f;

    /**
     * Register this context in the global registry.
     */
    void registerSelf(FLAlertLayer* self);
    void registerSelf(CCNode* self, FLAlertLayer* alertLayer);

    /**
     * Sets up the side menu (m_sideContainer), which side buttons are added to.
     */
    virtual void setupSideMenu();

    void createSideContainer(CCPoint position, CCSize size);

    void updateMixedUI(int property, std::optional<float> value);
    void makeMixed(int property);
    void removeMixed(int property, float newValue);
    void onToggleMixed(bool enabled);
    void toggleSlider(int property, bool isEnabled);
    void toggleButtons(int property, bool isEnabled);
    void reassignNode(CCNode* node, int newProperty);
    void depressInput(CCTextInputNode* input, CCScale9Sprite* bg, bool isPressed);
    void closeAllInputs();

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

    void addSideMenuButton(geode::Button* button);
    void addInput(CCTextInputNode* input, int property);
    void addSlider(Slider* slider, int property);
    void addButton(CCMenuItem* button, int property);
    void addInputBG(CCScale9Sprite* bg, int property);
    void addExtraNode(CCNode* node, int property);

    void reassignProperty(IDNameInput* input, int newProperty);

    CCArrayExt<GameObject*>& getGameObjects() { return m_gameObjects; }
    std::map<int, Ref<Slider>>& getSliders() { return m_sliders; }

    CCTextInputNode* getInput(int property);
    Slider* getSlider(int property);
    CCScale9Sprite* getInputBG(int property);
    std::vector<Ref<CCNode>> getExtraNodesForProperty(int property);
    bool isMixed(int property);

    static std::optional<int> getPropertyID(CCNode* node);
    static void setInputValue(CCTextInputNode* input, float value, int decimalPlaces);
    static void setSliderValue(Slider* slider, float value, float min, float max);
    static Toggler* createSideMenuButton(ZStringView sprName, Toggler::ToggleCallback callback);
    static bool isTriggerPopup(SetupTriggerPopup* popup);
    static void closeInput(CCTextInputNode* input);

    static MultiEditContext* get(CCNode* popup);
    static MultiEditContext* getFromChild(CCNode* child);
};

/**
 * An event fired right before MultiEditContext::setupMixed runs.
 */
class SetupMixedEvent : public Event<SetupMixedEvent, bool(MultiEditContext* ctx)> {
public:
    using Event::Event;
};

bool hoveringOverBG(CCScale9Sprite* bg);
CCNode* getTopLevelParent(CCNode* node);
