#pragma once

#include <Geode/Geode.hpp>
using namespace geode::prelude;

class AddRandomGroupsPopup : public Popup {
protected:
    TextInput* m_groupInput;
    TextInput* m_coverageInput;
    CCMenu* m_layout;
    ScrollLayer* m_scrollLayer;
    Scrollbar* m_scrollbar;
    std::vector<short> m_groups;
    CCArray* m_objects;
    CCMenuItemToggler* m_toggleLinkedButton;

    bool init(CCArray* objects);

    void onChangeInput(CCObject* sender);
    void onNextFree(CCObject* sender);
    void onAddGroup(CCObject* sender);
    void addGroupButton(short group);
    void onRemoveGroup(CCObject* sender);
    void onChangeGroups();
    void assignGroups();
    void onApply(CCObject* sender);

public:
    static AddRandomGroupsPopup* create(CCArray* objects) {
        auto popup = new AddRandomGroupsPopup;
        if (popup->init(objects)) {
            popup->autorelease();
            return popup;
        }
        delete popup;
        return nullptr;
    }
};