// CreateParticlePopup doesn't extend SetupTriggerPopup, so we have to reimplement mixed inputs again
// lots of duplicated code but truthfully i think trying to keep this dry would be more trouble than it's worth
// since SetupTriggerPopup and CreateParticlePopup work fundamentally differently

#include "CreateParticlePopup.hpp"
#include "MixedInputPopup.hpp"
#include "Trigger.hpp"
#include "../../misc/StringUtils.hpp"

float getParticleValueByKey(CCParticleSystemQuad* particle, int key, bool isSet, float newValue) {
    #define GET_SET_NUMBER(key, name) \
        case key: \
            if (isSet) { particle->set##name(newValue); return newValue; } \
            else { return particle->get##name(); }

    #define GET_SET_POINT(keyX, keyY, name) \
        case keyX: \
            if (isSet) { particle->set##name({newValue, particle->get##name().y}); return newValue; } \
            else { return particle->get##name().x; } \
        case keyY: \
            if (isSet) { particle->set##name({particle->get##name().x, newValue}); return newValue; } \
            else { return particle->get##name().y; } \

    #define GET_SET_INDIVIDUAL_COLOR(rgba, key, name) \
        case key: \
            if (isSet) { \
                auto color = particle->get##name(); \
                color.rgba = newValue; \
                particle->set##name(color); \
                return newValue; \
            } else { return particle->get##name().rgba; }

    #define GET_SET_COLOR(keyR, keyG, keyB, keyA, name) \
        GET_SET_INDIVIDUAL_COLOR(r, keyR, name) \
        GET_SET_INDIVIDUAL_COLOR(g, keyG, name) \
        GET_SET_INDIVIDUAL_COLOR(b, keyB, name) \
        GET_SET_INDIVIDUAL_COLOR(a, keyA, name)

    switch (key) {
        GET_SET_NUMBER(0x1, TotalParticles);
        GET_SET_NUMBER(0x2, Duration);
        GET_SET_NUMBER(0x3, Life);
        GET_SET_NUMBER(0x4, LifeVar);
        GET_SET_NUMBER(0x5, EmissionRate);
        GET_SET_NUMBER(0x6, Angle);
        GET_SET_NUMBER(0x7, AngleVar);
        GET_SET_NUMBER(0x8, Speed);
        GET_SET_NUMBER(0x9, SpeedVar);
        GET_SET_POINT(0xA, 0xB, PosVar);
        GET_SET_POINT(0xC, 0xD, Gravity);
        GET_SET_NUMBER(0xE, RadialAccel);
        GET_SET_NUMBER(0xF, RadialAccelVar);
        GET_SET_NUMBER(0x10, TangentialAccel);
        GET_SET_NUMBER(0x11, TangentialAccelVar);
        GET_SET_NUMBER(0x12, StartSize);
        GET_SET_NUMBER(0x13, StartSizeVar);
        GET_SET_NUMBER(0x14, EndSize);
        GET_SET_NUMBER(0x15, EndSizeVar);
        GET_SET_NUMBER(0x16, StartSpin);
        GET_SET_NUMBER(0x17, StartSpinVar);
        GET_SET_NUMBER(0x18, EndSpin);
        GET_SET_NUMBER(0x19, EndSpinVar);
        GET_SET_COLOR(0x1A, 0x1C, 0x1E, 0x20, StartColor);
        GET_SET_COLOR(0x1B, 0x1D, 0x1F, 0x21, StartColorVar);
        GET_SET_COLOR(0x22, 0x24, 0x26, 0x28, EndColor);
        GET_SET_COLOR(0x23, 0x25, 0x27, 0x29, EndColorVar);
        GET_SET_NUMBER(0x2A, FadeInTime);
        GET_SET_NUMBER(0x2B, FadeInTimeVar);
        GET_SET_NUMBER(0x2C, FadeOutTime);
        GET_SET_NUMBER(0x2D, FadeOutTimeVar);
        GET_SET_NUMBER(0x2E, FrictionPos);
        GET_SET_NUMBER(0x2F, FrictionPosVar);
        GET_SET_NUMBER(0x30, Respawn);
        GET_SET_NUMBER(0x31, RespawnVar);
        GET_SET_NUMBER(0x32, StartRadius);
        GET_SET_NUMBER(0x33, StartRadiusVar);
        GET_SET_NUMBER(0x34, EndRadius);
        GET_SET_NUMBER(0x35, EndRadiusVar);
        GET_SET_NUMBER(0x36, RotatePerSecond);
        GET_SET_NUMBER(0x37, RotatePerSecondVar);
        GET_SET_NUMBER(0x45, FrictionSize);
        GET_SET_NUMBER(0x46, FrictionSizeVar);
        GET_SET_NUMBER(0x47, FrictionRot);
        GET_SET_NUMBER(0x48, FrictionRotVar);
    }

    #undef GET_SET_NUMBER
    #undef GET_SET_POINT
    #undef GET_SET_INDIVIDUAL_COLOR
    #undef GET_SET_COLOR

    return 0.0f;
}

float getParticleValue(CCParticleSystemQuad* particle, int key) {
    return getParticleValueByKey(particle, key, false);
}

void setParticleValue(CCParticleSystemQuad* particle, int key, float value) {
    getParticleValueByKey(particle, key, true, value);
}

std::optional<float> getParticleValue(CCArrayExt<CCParticleSystemQuad*> particles, int key) {
    float firstValue = getParticleValue(particles[0], key);

    for (auto const& particle : particles) {
        if (getParticleValue(particle, key) != firstValue) return std::nullopt;
    }

    return firstValue;
}

void setParticleValue(CCArrayExt<CCParticleSystemQuad*> particles, int key, float value) {
    for (auto const& particle : particles) {
        setParticleValue(particle, key, value);
    }
}


bool MECreateParticlePopup::init(ParticleGameObject* obj, cocos2d::CCArray* objs, gd::string str) {
    if (!CreateParticlePopup::init(obj, objs, str)) return false;
    if (!m_particles || m_particles->count() == 0) return true;

    // m_inputDicts and m_sliderDict structure:
    // index 0: all inputs/sliders
    // index 1-3: inputs/sliders for each page

    for (size_t i = 1; i < m_inputDicts->count(); i++) {
        auto inputDict = static_cast<CCDictionary*>(m_inputDicts->objectAtIndex(i));

        for (auto const& [key, input] : CCDictionaryExt<int, CCTextInputNode*>(inputDict)) {
            if (getParticleValue(m_particles, key) != std::nullopt) continue;

            replaceInputWithButton(input, key, i);
        }
    }

    return true;
}

void MECreateParticlePopup::replaceInputWithButton(CCTextInputNode* input, int property, int page) {
    // create "mixed" button

    auto spr = CCLabelBMFont::create("Mixed", "bigFont.fnt");
    spr->setScale(0.4);

    auto btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(MECreateParticlePopup::onMixedInput));
    btn->setPosition(input->getPosition() - m_buttonMenu->getPosition());
    btn->setID("mixed-input-btn"_spr);
    btn->setTag(input->getTag());

    // add button to the correct page container

    if (auto arr = typeinfo_cast<CCArray*>(m_pageObjectArrays->objectAtIndex(page - 1))) arr->addObject(btn);
    btn->setVisible(page == m_page);

    // swap input with button

    m_fields->removedInputNodes[property] = input;
    input->removeFromParent();

    m_buttonMenu->addChild(btn);
    m_fields->mixedButtons[property] = btn;

    toggleSliderOfKey(property, false);
}

void MECreateParticlePopup::replaceButtonWithInput(
    CCMenuItemSpriteExtra* button, int property, float newValue, int page
) {
    CCTextInputNode* input = m_fields->removedInputNodes[property];

    if (auto arr = typeinfo_cast<CCArray*>(m_pageObjectArrays->objectAtIndex(page - 1))) arr->addObject(input);
    input->setVisible(page == m_page);

    m_mainLayer->addChild(input);
    button->removeFromParent();

    m_fields->mixedButtons.inner()->removeObjectForKey(property);
    m_fields->removedInputNodes.inner()->removeObjectForKey(property);

    // updateInputNodeStringForType(static_cast<gjParticleValue>(property));

    short decimalPlaces = Trigger::getPropertyDecimalPlaces(property + 10000);
    input->setString(nk::toString(newValue, decimalPlaces, false));

    toggleSliderOfKey(property, true);
}

void MECreateParticlePopup::onMixedInput(CCObject* sender) {
    if (!m_targetObjects || m_targetObjects->count() == 0) return;

    int property = static_cast<int>(sender->getTag());

    auto callback = [sender, property, this](std::optional<float> value) {
        auto shouldReplaceInputWithButton = typeinfo_cast<CCTextInputNode*>(sender) && !value.has_value();
        auto shouldReplaceButtonWithInput = typeinfo_cast<CCMenuItemSpriteExtra*>(sender) && value.has_value();
        auto shouldChangeInput = typeinfo_cast<CCTextInputNode*>(sender) && value.has_value();

        if (shouldReplaceInputWithButton)
            replaceInputWithButton(static_cast<CCTextInputNode*>(sender), property, m_page);
        else if (shouldReplaceButtonWithInput)
            replaceButtonWithInput(static_cast<CCMenuItemSpriteExtra*>(sender), property, value.value(), m_page);
        else if (shouldChangeInput)
            updateInputNodeStringForType(static_cast<gjParticleValue>(property));

        updateSliderForType(static_cast<gjParticleValue>(property));
    };

    auto alert = MixedInputPopup::create(m_targetObjects, property + 10000, callback);
    alert->m_noElasticity = true;
    alert->show();
}

void MECreateParticlePopup::toggleSliderOfKey(int key, bool isEnabled) {
    CCDictionaryExt<int, Slider*> sliderDict = static_cast<CCDictionary*>(m_sliderDicts->objectAtIndex(0));

    for (auto const& [sliderKey, slider] : sliderDict) {
        if (sliderKey != key || !slider) continue;

        // for some reason setTouchEnabled doesnt seem to do anything so i just move the slider thumb
        // kinda hacky but it's invisible so it's fine
        slider->getThumb()->setPosition(9999, 0);

        slider->m_groove->setOpacity(isEnabled ? 255 : 100);
        slider->getThumb()->setOpacity(isEnabled ? 255 : 0);
        slider->m_sliderBar->setOpacity(isEnabled ? 255 : 0);

        return;
    }
}
