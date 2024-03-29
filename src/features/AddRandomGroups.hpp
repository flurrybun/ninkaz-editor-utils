#pragma once

#include <Geode/Geode.hpp>
#include <Geode/ui/TextInput.hpp>

using namespace geode::prelude;

class AddRandomGroupsPopup : public Popup<CCArray*> {
protected:
    TextInput* m_groupInput;
    TextInput* m_coverageInput;
    CCMenu* m_layout;
    std::vector<short> m_groups;
    CCArray m_selectedObjects;
    CCMenuItemToggler* m_toggleLinkedButton;

    bool setup(CCArray* selectedObjects) override;

    void onChangeInput(CCObject* sender);
    void onNextFree(CCObject* sender);
    void onAddGroup(CCObject* sender);
    CCMenuItemSpriteExtra* createGroupButton(short group);
    void onRemoveGroup(CCObject* sender);
    void assignGroups();
    void onApply(CCObject* sender);

public:
    static AddRandomGroupsPopup* create(CCArray* selectedObjects);
};