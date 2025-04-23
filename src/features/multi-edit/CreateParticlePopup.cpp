#include "MultiEditManager.hpp"
#include <Geode/modify/CreateParticlePopup.hpp>

#include <Geode/Geode.hpp>
using namespace geode::prelude;

CCScale9Sprite* getBGForInput(CCTextInputNode* input) {
    if (!input || !input->getParent()) return nullptr;

    auto parent = input->getParent();
    CCPoint bgPosition = input->getPosition();

    for (auto child : CCArrayExt<CCNode*>(parent->getChildren())) {
        if (auto bg = typeinfo_cast<CCScale9Sprite*>(child)) {
            if (bg->getPosition() == bgPosition) return bg;
        }
    }

    return nullptr;
}

CCLabelBMFont* getLabelForInput(CCTextInputNode* input) {
    if (!input || !input->getParent()) return nullptr;

    auto parent = input->getParent();
    CCPoint bgPosition = input->getPosition() - ccp(20, 0); // todo: find exact position

    for (auto child : CCArrayExt<CCNode*>(parent->getChildren())) {
        if (auto bg = typeinfo_cast<CCLabelBMFont*>(child)) {
            if (bg->getPosition() == bgPosition) return bg;
        }
    }

    return nullptr;
}

class $modify(MECreateParticlePopup, CreateParticlePopup) {
    struct Fields {
        Ref<MultiEditManager> multiEditManager;
    };

    $override
    bool init(ParticleGameObject* obj, cocos2d::CCArray* objs, gd::string str) {
        if (!CreateParticlePopup::init(obj, objs, str)) return false;

        auto mem = MultiEditManager::create(this, objs);
        m_fields->multiEditManager = mem;

        for (size_t i = 1; i < m_inputDicts->count(); i++) {
            auto inputDict = static_cast<CCDictionary*>(m_inputDicts->objectAtIndex(i));

            for (auto const& [key, input] : CCDictionaryExt<int, CCTextInputNode*>(inputDict)) {
                input->setMaxLabelWidth(40);
                mem->addInput(input, key);

                if (auto bg = getBGForInput(input)) {
                    mem->addInputBG(bg, key);
                }

                if (auto label = getLabelForInput(input)) {
                    mem->addInputLabel(label, key);
                }
            }
        }

        CCDictionaryExt<int, Slider*> sliderDict = static_cast<CCDictionary*>(m_sliderDicts->objectAtIndex(0));

        for (auto const& [key, slider] : sliderDict) {
            mem->addSlider(slider, key);
        }

        mem->setPages(m_pageObjectArrays);
        mem->setCallback([this](int property, std::optional<float> value) {
            if (!value.has_value()) return;

            updateParticleValueForType(value.value(), static_cast<gjParticleValue>(property), m_particle);
            updateInputNodeStringForType(static_cast<gjParticleValue>(property));
            updateSliderForType(static_cast<gjParticleValue>(property));
        });

        mem->setupMixed();

        return true;
    }
};
