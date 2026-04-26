#pragma once

#include <Geode/Geode.hpp>
using namespace geode::prelude;

class AddRandomGroupsPopup : public Popup {
protected:
    TextInput* m_groupInput = nullptr;
    TextInput* m_coverageInput = nullptr;
    CCMenu* m_layout = nullptr;
    ScrollLayer* m_scrollLayer = nullptr;
    Scrollbar* m_scrollbar = nullptr;
    CCArray* m_objects = nullptr;
    CCMenuItemToggler* m_toggleLinkedButton = nullptr;

    std::vector<short> m_groups;

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