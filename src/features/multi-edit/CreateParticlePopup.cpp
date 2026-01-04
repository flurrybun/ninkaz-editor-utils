#include "MultiEditContext.hpp"
#include <Geode/modify/CreateParticlePopup.hpp>

#include <Geode/Geode.hpp>
using namespace geode::prelude;

CCScale9Sprite* getBGForInput(CCTextInputNode* input) {
    if (!input || !input->getParent()) return nullptr;

    for (auto child : CCArrayExt<CCNode*>(input->getParent()->getChildren())) {
        if (auto bg = typeinfo_cast<CCScale9Sprite*>(child)) {
            if (bg->getPosition() == input->getPosition()) return bg;
        }
    }

    return nullptr;
}

CCLabelBMFont* getLabelForInput(CCTextInputNode* input) {
    if (!input || !input->getParent()) return nullptr;

    for (auto child : CCArrayExt<CCNode*>(input->getParent()->getChildren())) {
        if (auto bg = typeinfo_cast<CCLabelBMFont*>(child)) {
            if (bg->getPositionY() != input->getPositionY()) continue;

            if (bg->getPositionX() == input->getPositionX() - 30.2f) return bg;
            if (bg->getPositionX() == input->getPositionX() - 32.2f) return bg;
        }
    }

    return nullptr;
}

class $modify(MECreateParticlePopup, CreateParticlePopup) {
    struct Fields : MultiEditContext {
        CreateParticlePopup* popup;

        void init(CreateParticlePopup* popup) {
            this->popup = popup;
            registerSelf(popup);
        }

        CCParticleSystemQuad* getParticleForObject(GameObject* object) {
            unsigned int index = popup->m_targetObjects->indexOfObject(object);
            return static_cast<CCParticleSystemQuad*>(popup->m_particles->objectAtIndex(index));
        }

        float getOrSetProperty(CCParticleSystemQuad* particle, int key, bool isSet = false, float newValue = 0.f) {
            #define GET_SET_NUMBER(key, name) \
                case key: \
                    if (isSet) { \
                        particle->set##name(newValue); \
                        return newValue; \
                    } else { \
                        return particle->get##name(); \
                    }

            #define GET_SET_POINT(keyX, keyY, name) \
                case keyX: \
                    if (isSet) { \
                        particle->set##name({newValue, particle->get##name().y}); \
                        return newValue; \
                    } else { \
                        return particle->get##name().x; \
                    } \
                case keyY: \
                    if (isSet) { \
                        particle->set##name({particle->get##name().x, newValue}); \
                        return newValue; \
                    } else { \
                        return particle->get##name().y; \
                    }

            #define GET_SET_INDIVIDUAL_COLOR(rgba, key, name) \
                case key: \
                    if (isSet) { \
                        auto color = particle->get##name(); \
                        color.rgba = newValue; \
                        particle->set##name(color); \
                        return newValue; \
                    } else { \
                        return particle->get##name().rgba; \
                    }

            #define GET_SET_COLOR(keyR, keyG, keyB, keyA, name) \
                GET_SET_INDIVIDUAL_COLOR(r, keyR, name) \
                GET_SET_INDIVIDUAL_COLOR(g, keyG, name) \
                GET_SET_INDIVIDUAL_COLOR(b, keyB, name) \
                GET_SET_INDIVIDUAL_COLOR(a, keyA, name)

            #define GET_SET_MEMBER(key, member) \
                case key: \
                    if (isSet) { \
                        particle->member = newValue; \
                        return newValue; \
                    } else { \
                        return particle->member; \
                    }

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
                GET_SET_MEMBER(0x2A, m_fFadeInTime);
                GET_SET_MEMBER(0x2B, m_fFadeInTimeVar);
                GET_SET_MEMBER(0x2C, m_fFadeOutTime);
                GET_SET_MEMBER(0x2D, m_fFadeOutTimeVar);
                GET_SET_MEMBER(0x2E, m_fFrictionPos);
                GET_SET_MEMBER(0x2F, m_fFrictionPosVar);
                GET_SET_MEMBER(0x30, m_fRespawn);
                GET_SET_MEMBER(0x31, m_fRespawnVar);
                GET_SET_NUMBER(0x32, StartRadius);
                GET_SET_NUMBER(0x33, StartRadiusVar);
                GET_SET_NUMBER(0x34, EndRadius);
                GET_SET_NUMBER(0x35, EndRadiusVar);
                GET_SET_NUMBER(0x36, RotatePerSecond);
                GET_SET_NUMBER(0x37, RotatePerSecondVar);
                GET_SET_MEMBER(0x45, m_fFrictionSize);
                GET_SET_MEMBER(0x46, m_fFrictionSizeVar);
                GET_SET_MEMBER(0x47, m_fFrictionRot);
                GET_SET_MEMBER(0x48, m_fFrictionRotVar);
            }

            #undef GET_SET_NUMBER
            #undef GET_SET_POINT
            #undef GET_SET_INDIVIDUAL_COLOR
            #undef GET_SET_COLOR
            #undef GET_SET_MEMBER

            return 0.f;
        }

        float getProperty(GameObject* object, int property) override {
            auto particle = getParticleForObject(object);
            if (!particle) return 0;

            return getOrSetProperty(particle, property, false);
        }

        void setProperty(GameObject* object, int property, float value) override {
            auto particle = getParticleForObject(object);
            if (particle) {
                getOrSetProperty(particle, property, true, value);
            }
        }

        bool hasProperty(GameObject* object, int property) override {
            return object->m_objectID == 2065;
        }

        int getPropertyDecimalPlaces(int property) override {
            if (property >= 0x2 && property <= 0x4) return 2;
            if (property >= 0x1A && property <= 0x31) return 2;
            if (property >= 0x45 && property <= 0x48) return 2;
            return 0;
        }

        PropertyBounds getPropertyBounds(int intProperty) override {
            gjParticleValue property = static_cast<gjParticleValue>(intProperty);

            switch (property) {
                case gjParticleValue::LifetimeVar:
                case gjParticleValue::AngleVar:
                case gjParticleValue::SpeedVar:
                case gjParticleValue::PosVarX:
                case gjParticleValue::PosVarY:
                case gjParticleValue::AccelRadVar:
                case gjParticleValue::AccelTanVar:
                case gjParticleValue::StartSizeVar:
                case gjParticleValue::EndSizeVar:
                case gjParticleValue::StartSpinVar:
                case gjParticleValue::EndSpinVar:
                case gjParticleValue::StartRVar:
                case gjParticleValue::StartGVar:
                case gjParticleValue::StartBVar:
                case gjParticleValue::StartAVar:
                case gjParticleValue::EndRVar:
                case gjParticleValue::EndGVar:
                case gjParticleValue::EndBVar:
                case gjParticleValue::EndAVar:
                case gjParticleValue::FadeInVar:
                case gjParticleValue::FadeOutVar:
                case gjParticleValue::FrictionPVar:
                case gjParticleValue::RespawnVar:
                case gjParticleValue::StartRadVar:
                case gjParticleValue::EndRadVar:
                case gjParticleValue::RotSecVar:
                case gjParticleValue::FrictionSVar:
                case gjParticleValue::FrictionRVar:
                    return PropertyBounds::toInf(0);
                default:
                    return PropertyBounds::negInfToInf();
            }
        }

        void onMixedInputApplied(int property, std::optional<float> value) override {
            updateMixedUI(property, value);

            if (!value) return;

            popup->updateParticleValueForType(*value, static_cast<gjParticleValue>(property), popup->m_particle);
            popup->updateInputNodeStringForType(static_cast<gjParticleValue>(property));
            popup->updateSliderForType(static_cast<gjParticleValue>(property));
        }

        CCArray* getObjectArray() override {
            if (auto obj = popup->m_targetObject) {
                return CCArray::createWithObject(obj);
            } else {
                return popup->m_targetObjects;
            }
        }
    };

    $override
    bool init(ParticleGameObject* obj, cocos2d::CCArray* objs, gd::string str) {
        if (!CreateParticlePopup::init(obj, objs, str)) return false;

        m_fields->init(this);

        for (size_t i = 1; i < m_inputDicts->count(); i++) {
            auto inputDict = static_cast<CCDictionary*>(m_inputDicts->objectAtIndex(i));

            for (auto const& [key, input] : CCDictionaryExt<int, CCTextInputNode*>(inputDict)) {
                input->setMaxLabelWidth(40);
                m_fields->addInput(input, key);

                if (auto bg = getBGForInput(input)) m_fields->addInputBG(bg, key);
                if (auto label = getLabelForInput(input)) m_fields->addInputLabel(label, key);
            }
        }

        auto sliderDict = static_cast<CCDictionary*>(m_sliderDicts->objectAtIndex(0));

        for (auto const& [key, slider] : CCDictionaryExt<int, Slider*>(sliderDict)) {
            m_fields->addSlider(slider, key);
        }

        m_fields->setPages(m_pageObjectArrays);
        m_fields->setupMixed();

        return true;
    }
};
