#pragma once

#include "MixedInputPopup.hpp"

#include <Geode/Geode.hpp>
using namespace geode::prelude;

class MixedInputSettingsPopup : public Popup<MixedInputSettings, std::function<void(MixedInputSettings)>> {
protected:
    MixedInputSettings m_settings;
    std::function<void(MixedInputSettings)> m_callback;
    CCMenuItemToggler* m_roundingBtn;

    bool setup(MixedInputSettings, std::function<void(MixedInputSettings)>) override;
    void onRoundingButton(CCObject*);
public:
    static MixedInputSettingsPopup* create(MixedInputSettings, std::function<void(MixedInputSettings)>);
};