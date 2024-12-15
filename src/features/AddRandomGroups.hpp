#pragma once

#include "../misc/CCBoundedMenu.hpp"
#include <Geode/modify/EditorPauseLayer.hpp>
#include <Geode/Geode.hpp>
using namespace geode::prelude;

class $modify(ARGEditorPauseLayer, EditorPauseLayer) {
    $override bool init(LevelEditorLayer* levelEditorLayer);
    void onAddRandomGroups(CCObject*);
};

class AddRandomGroupsPopup : public Popup<CCArray*> {
protected:
    TextInput* m_groupInput;
    TextInput* m_coverageInput;
    CCMenu* m_layout;
    ScrollLayer* m_scrollLayer;
    Scrollbar* m_scrollbar;
    std::vector<short> m_groups;
    CCArray* m_selectedObjects;
    CCMenuItemToggler* m_toggleLinkedButton;

    bool setup(CCArray* selectedObjects) override;

    void onChangeInput(CCObject* sender);
    void onNextFree(CCObject* sender);
    void onAddGroup(CCObject* sender);
    void addGroupButton(short group);
    void onRemoveGroup(CCObject* sender);
    void onChangeGroups();
    void assignGroups();
    void onApply(CCObject* sender);

public:
    static AddRandomGroupsPopup* create(CCArray* selectedObjects);
};