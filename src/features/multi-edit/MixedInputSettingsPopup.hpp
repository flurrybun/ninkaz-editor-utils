#pragma once

#include "MixedInputPopup.hpp"

#include <Geode/Geode.hpp>
using namespace geode::prelude;

class MixedInputSettingsPopup : public Popup {
protected:
    MixedInputSettings m_settings;
    geode::Function<void(MixedInputSettings)> m_callback;
    CCMenuItemToggler* m_roundingBtn;

    bool init(MixedInputSettings, geode::Function<void(MixedInputSettings)>);
    void onRoundingButton(CCObject*);
public:
    static MixedInputSettingsPopup* create(MixedInputSettings settings, geode::Function<void(MixedInputSettings)> callback) {
        auto popup = new MixedInputSettingsPopup;
        if (popup->init(std::move(settings), std::move(callback))) {
            popup->autorelease();
            return popup;
        }
        delete popup;
        return nullptr;
    }
};