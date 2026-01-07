#pragma once

#include <Geode/Geode.hpp>
using namespace geode::prelude;

class PasteStatePopup : public Popup<> {
protected:
    enum Preset { Default, All, None, Custom };
    const std::array<const char*, 4> PRESET_NAMES = {"Default", "All", "None", "Custom"};

    enum Property {
        GroupID, ZLayer, ZOrder, EditorLayer, Extra, ObjectID,
        PositionX, PositionY, RotationX, RotationY, ScaleX, ScaleY
    };
    const std::array<const char*, 12> PROPERTY_NAMES = {
        "Group IDs", "Z Layer", "Z Order", "Editor Layers", "Extras", "Object ID",
        "Position X", "Position Y", "Rotation X", "Rotation Y", "Scale X", "Scale Y"
    };

    static PasteStatePopup* s_instance;
    std::array<bool, 12> m_properties;
    std::array<CCMenuItemToggler*, 12> m_togglers;
    std::array<CCMenuItemSpriteExtra*, 4> m_presetButtons;

    bool setup() override;
    void onClose(CCObject* sender) override;

    void addQuickPasteButton();
    CCMenu* createToggler(const char* name, Property property);
    
    void onToggler(CCObject* sender);
    void onPreset(CCObject* sender);
    void setHighlightedPreset(Preset preset);
    void onCancel(CCObject* sender);
    void onPaste(CCObject* sender);
    void pasteObjectState(GameObject* src, GameObject* dest);
    void replaceObjectIDs(CCArray* objects, int newID);
public:
    void onQuickPaste(CCObject* sender);

    static PasteStatePopup* create();
    static PasteStatePopup* get();
};
