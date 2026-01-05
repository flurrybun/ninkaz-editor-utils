#include <Geode/modify/SetupTriggerPopup.hpp>
#include "MultiEditContext.hpp"
#include "../../misc/CCScale9SpriteFix.hpp"
#include "../../misc/StringUtils.hpp"

#include <Geode/Geode.hpp>
using namespace geode::prelude;

class $modify(MESetupTriggerPopup, SetupTriggerPopup) {
    struct Fields : MultiEditContext {
        SetupTriggerPopup* popup;
        std::optional<std::function<void(int, float)>> callback;

        void init(SetupTriggerPopup* popup) {
            this->popup = popup;
            registerSelf(popup);
        }

        float getProperty(GameObject* object, int property) override { 
            if (property == 97) {
                if (auto ego = typeinfo_cast<EnhancedGameObject*>(object)) {
                    return ego->m_rotationSpeed;
                }
            }

            // these properties aren't supported by SetupTriggerPopup::getTriggerValue

            if (auto trigger = typeinfo_cast<EffectGameObject*>(object)) {
                if (property == 45) return trigger->m_fadeInDuration;
                if (property == 46) return trigger->m_holdDuration;
                if (property == 47) return trigger->m_fadeOutDuration;
                if (property == 50) return trigger->m_copyColorID;
                if (property == 72) return trigger->m_followXMod;
                if (property == 73) return trigger->m_followYMod;
                if (property == 23) return trigger->m_targetColor;
            }

            float value = popup->getTriggerValue(property, object);

            // move trigger x/y units are stored in 1/30ths of a block, but displayed as 1/10th
            if (
                (property == 28 || property == 29) && // move trigger x/y
                !popup->getTriggerValue(393, object) // small step enabled
            ) {
                value = std::floor(value / 3);
            }

            return value;
        }

        void setProperty(GameObject* object, int property, float newValue) override {
            if (property == 97) {
                if (auto ego = typeinfo_cast<EnhancedGameObject*>(object)) {
                    ego->m_rotationSpeed = newValue;
                    return;
                }
            }

            // these properties aren't supported by SetupTriggerPopup::updateValue
            // i'm pretty sure...it's been a while

            if (auto trigger = typeinfo_cast<EffectGameObject*>(object)) {
                if (property == 72) {
                    trigger->m_followXMod = newValue;
                    return;
                } else if (property == 73) {
                    trigger->m_followYMod = newValue;
                    return;
                } else if (property == 50) {
                    trigger->m_copyColorID = newValue;
                    return;
                }
            }

            // move trigger x/y units are stored in 1/30ths of a block, but displayed as 1/10th
            if (
                (property == 28 || property == 29) && // move trigger x/y
                !popup->getTriggerValue(393, object) // small step enabled
            ) {
                newValue *= 3;
            }

            // fudging m_gameObjects so we can take advantage of updateValue
            // rather than rewriting it all ourselves

            auto savedGameObjects = popup->m_gameObjects;

            popup->m_gameObjects = CCArray::createWithObject(object);
            popup->updateValue(property, newValue);

            popup->m_gameObjects = savedGameObjects;

            if (object->m_objectID == 1595) {
                LevelEditorLayer::get()->updateObjectLabel(object);
            }
        }

        bool hasProperty(GameObject* object, int property) override {
            if (property >= 2000 && property <= 2005) return true;

            if (auto ego = typeinfo_cast<EnhancedGameObject*>(object)) {
                if (property == 97) return true;
            }

            auto in = [property](const std::vector<int>& vec) {
                return std::find(vec.begin(), vec.end(), property) != vec.end();
            };

            // the following properties were generated via a script that goes through
            // m_inputNodes and finds the tags of each input

            // some triggers don't use this system and had to be added manually

            switch (object->m_objectID) {
                case 899: return in({10, 23}); // added manually
                case 901: return in({28, 29, 143, 144, 395, 71, 396, 10, 51});
                case 1616: return in({51});
                case 1006: return in({50, 51, 45, 46, 47}); // added manually
                case 1007: return in({10, 51}); // added manually
                case 1049: return in({51});
                case 1268: return in({51, 63, 556}); //removed: -1, -2
                case 2067: return in({150, 151, 10, 71, 51});
                case 1347: return in({10, 72, 73, 51, 71}); // added manually
                case 3033: return in({76, 51, 71, 520, 521, 545, 522, 523, 546});
                case 1346: return in({68, 69, 401, 402, 10, 403, 51, 71, 516, 518, 517, 519});
                case 3016: return in({51, 71, 365, 340, 363, 364, 292, 293, 298, 299, 308, 309, 366, 361, 362, 300, 301, 560, 563, 564, 565, 334, 335, 558, 559, 359, 360, 561, 562, 357, 358, 316, 317, 318, 319, 322, 323, 320, 321, 338, 324, 325, 359, 360, 326, 327, 328, 329, 330, 331, 332, 333});
                case 3660: return in({51, 566, 567, 568, 569, 300, 301, 560, 563, 564, 565});
                case 3661: return in({51, 71});
                case 3032: return in({51, 10, 537, 71, 557, 524});
                case 3006: return in({222, 223, 220, 221, 218, 219, 231, 232, 288, 237, 238, 239, 240, 252, 253, 225, 51, 71, 341, 263, 264, 282});
                case 3007: return in({222, 223, 220, 221, 270, 271, 252, 253, 225, 51, 71, 341, 263, 264, 282});
                case 3008: return in({222, 223, 220, 221, 233, 234, 235, 236, 252, 253, 225, 51, 71, 341, 263, 264, 282});
                case 3009: return in({222, 223, 220, 221, 286, 275, 252, 253, 225, 51, 71, 341, 263, 264, 282});
                case 3010: return in({222, 223, 220, 221, 260, 265, 252, 253, 225, 51, 71, 341, 263, 264, 282});
                case 3011: return in({222, 223, 220, 221, 252, 253, 263, 264, 10, 282, 51, 218, 219, 231, 232, 288, 237, 238, 239, 240});
                case 3012: return in({222, 223, 220, 221, 252, 253, 263, 264, 10, 282, 51, 270, 271});
                case 3013: return in({222, 223, 220, 221, 252, 253, 263, 264, 10, 282, 51, 233, 234, 235, 236});
                case 3014: return in({222, 223, 220, 221, 252, 253, 263, 264, 10, 282, 51, 275});
                case 3015: return in({222, 223, 220, 221, 252, 253, 263, 264, 10, 282, 51, 265});
                case 3024: return in({51});
                case 3029: return in({});
                case 3030: return in({});
                case 3031: return in({});
                case 1595: return in({51}); // added manually
                case 1611: return in({80, 77, 51}); // added manually
                case 1811: return in({80, 77, 51}); // added manually
                case 1817: return in({80, 77, 449});
                case 3614: return in({467, 473, 80, 470, 51});
                case 3615: return in({80, 51, 473});
                case 3617: return in({80});
                case 3619: return in({80, 95, 479, 51});
                case 3620: return in({80, 95, 479, 483, 51, 71, 484});
                case 3641: return in({80});
                case 1912: return in({51, 71, 10}); // added manually
                case 2068: return in({}); // added manually
                case 3607: return in({437, 438}); // removed: -1, -2
                case 3608: return in({51, 71, 547, 548, 549, 550, 552, 553, 554, 555});
                case 3618: return in({51});
                case 1913: return in({371, 10});
                case 1914: return in({71, 213, 454, 10});
                case 1916: return in({10, 28, 29}); // added manually
                case 2901: return in({28, 29});
                case 2015: return in({68, 10});
                case 2062: return in({51}); // added manually
                case 2925: return in({});
                case 2016: return in({371, 28, 29, 506});
                case 2900: return in({582, 583, 173});
                case 1934: return in({406, 404, 432, 408, 409, 410, 411});
                case 3605: return in({432, 10, 404, 406, 51, 71, 421, 424, 422, 425, 423, 426});
                case 3602: return in({404, 405, 406, 408, 409, 410, 411, 416, 455, 434, 51, 71, 421, 424, 422, 425, 423, 426});
                case 3603: return in({457, 455, 416, 10, 404, 406, 51, 71, 421, 424, 422, 425, 423, 426});
                case 3604: return in({51, 447, 525});
                case 3606: return in({143, 144});
                case 3612: return in({143, 144});
                case 1615: return in({80});
                case 3613: return in({51, 71});
                case 3662: return in({51});
                case 1815: return in({80, 95, 51});
                case 3609: return in({80, 95, 51, 71});
                case 3640: return in({51, 71});
                case 1816: return in({80}); // doesn't extend SetupTriggerPopup ??
                case 3643: return in({51});
                case 1812: return in({51});
                case 3600: return in({51, 71});
                case 1932: return in({});
                case 2899: return in({574});
                case 3642: return in({498, 501, 10});
                case 2903: return in({203, 204, 205, 206, 456, 209});
                case 2066: return in({148});
                case 3022: return in({51, 346, 348, 349, 350});
                case 2904: return in({});
                case 2905: return in({175, 176, 180, 179, 181, 182, 177, 512, 290, 291, 51, 183, 191, 10});
                case 2907: return in({175, 176, 180, 179, 181, 182, 177, 512, 290, 51, 10});
                case 2909: return in({10, 176, 175, 191, 179, 181, 182});
                case 2910: return in({180, 189, 10});
                case 2911: return in({175, 180, 176, 10, 179, 189, 191});
                case 2912: return in({180, 189, 10});
                case 2913: return in({179, 181, 10, 176, 290, 291, 51, 71});
                case 2914: return in({179, 10, 176, 71, 181, 290, 291, 51});
                case 2915: return in({180, 189, 10, 176, 181, 71, 191, 51});
                case 2916: return in({176, 180, 10, 290, 291, 51});
                case 2917: return in({180, 189, 10, 179, 512, 290, 291, 51});
                case 2919: return in({176, 10, 51});
                case 2920: return in({176, 10});
                case 2921: return in({176, 10, 179, 180, 189});
                case 2922: return in({176, 10});
                case 2923: return in({10, 176, 191, 175, 179, 180, 189});
                case 2924: return in({180, 189, 10});
                case 3017: return in({222, 223, 220, 221, 218, 219, 231, 232, 237, 238, 239, 240, 344, 225});
                case 3018: return in({222, 223, 220, 221, 270, 271, 344, 225});
                case 3019: return in({222, 223, 220, 221, 233, 234, 235, 236, 344, 225});
                case 3020: return in({222, 223, 220, 221, 275, 344, 225});
                case 3021: return in({222, 223, 220, 221, 260, 265, 344, 225});
                case 3023: return in({344, 225});
            }

            return false;
        }

        int getPropertyDecimalPlaces(int property) override {
            if (property == 2000 || property == 2003) return 0;
            if (property >= 2000 && property <= 2005) return 2;

            // im sure im missing some obscure properties but oh well
            int fourDecimalPlaces[] = {63, 556};
            int threeDecimalPlaces[] = {143, 144, 150, 151, 90, 91, 479, 483, 484, 371, 175, 176, 180, 179, 181,
                182, 177, 512, 290, 291, 183, 191};
            int twoDecimalPlaces[] = {10, 35, 45, 46, 47, 402, 68, 72, 73, 75, 84, 520, 521, 545, 522, 523, 546, 
                292, 293, 298, 299, 361, 362, 300, 301, 334, 335, 558, 559, 359, 360, 561, 562, 357, 358,
                316, 317, 318, 319, 322, 323, 320, 321, 324, 325, 326, 327, 330, 331, 332, 333, 566, 567,
                568, 569, 300, 301, 557, 288, 243, 249, 263, 264, 282, 467, 473, 470, 437, 438, 554, 555,
                213, 454, 406, 434, 421, 422, 423, 598};

            if (std::find(std::begin(fourDecimalPlaces), std::end(fourDecimalPlaces), property)
                != std::end(fourDecimalPlaces)) return 4;
            if (std::find(std::begin(threeDecimalPlaces), std::end(threeDecimalPlaces), property)
                != std::end(threeDecimalPlaces)) return 3;
            if (std::find(std::begin(twoDecimalPlaces), std::end(twoDecimalPlaces), property)
                != std::end(twoDecimalPlaces)) return 2;

            return 0;
        }

        // bool canPropertyBeNegative(int property) override {
        //     // this doesnt matter much so i only bothered adding common values
        //     short positiveProps[] = {10, 85, 51, 71, 45, 46, 47, 35, 63, 556, 80};

        //     return std::find(std::begin(positiveProps), std::end(positiveProps), property) == std::end(positiveProps);
        // }

        PropertyBounds getPropertyBounds(int property) override {
            switch (property) {
                case 23:
                case 50:
                case 209:
                case 260:
                case 80:
                case 95:
                case 534:
                case 225:
                case 416:
                case 455:
                case 173:
                case 530:
                    return PropertyBounds(0, 999);
                case 51:
                case 71:
                case 516:
                case 517:
                case 518:
                case 519:
                case 76:
                case 457:
                case 448:
                case 203:
                case 204:
                case 205:
                case 206:
                    return PropertyBounds(0, 9999);
                // i am undoubtably missing some here but it shouldn't matter too much
                // 10, 85, 51, 71, 45, 46, 47, 35, 63, 556, 80
                case 10:
                case 85:
                case 45:
                case 46:
                case 47:
                case 35:
                case 63:
                case 556:
                    return PropertyBounds::toInf(0);
                default:
                    return PropertyBounds::negInfToInf();
            }
        }

        void onMixedInputApplied(int property, std::optional<float> value) override {
            updateMixedUI(property, value);

            if (!value) return;

            if (auto input = getInput(property)) {
                popup->textChanged(input);
            }

            if (callback) {
                (*callback)(property, *value);
            }
        }

        CCArray* getObjectArray() override {
            // special case: SetupTouchTogglePopup sets m_gameObjects to nullptr
            if (typeinfo_cast<SetupTouchTogglePopup*>(popup)) {
                auto eui = EditorUI::get();

                if (auto obj = eui->m_selectedObject) {
                    return CCArray::createWithObject(obj);
                } else {
                    return eui->getSelectedObjects();
                }
            }

            if (auto obj = popup->m_gameObject) {
                return CCArray::createWithObject(obj);
            } else {
                return popup->m_gameObjects;
            }
        }
    };

    static void onModify(auto& self) {
        (void)self.setHookPriority("SetupTriggerPopup::createValueControlAdvanced", Priority::VeryLatePost);
        (void)self.setHookPriority("SetupTriggerPopup::init", Priority::VeryEarlyPost);
    }

    $override
    bool init(EffectGameObject* obj, CCArray* objs, float f1, float f2, int i1) {
        if (!SetupTriggerPopup::init(obj, objs, f1, f2, i1)) return false;
        if (!MultiEditContext::isTriggerPopup(this)) return true;

        m_fields->init(this);
        m_fields->setGroups(m_groupContainers);
        m_fields->setPages(m_pageContainers);

        return true;
    }

    $override
    CCArray* createValueControlAdvanced(
        int property, gd::string label, CCPoint position, float scale, bool disableSlider, InputValueType valueType,
        int charCountLimit, bool enableArrows, float sliderMin, float sliderMax, int page, int group,
        GJInputStyle inputStyle, int decimalPlaces, bool enableTrashCan
    ) {
        CCArray* ret = SetupTriggerPopup::createValueControlAdvanced(
            property, label, position, scale, disableSlider, valueType, charCountLimit, enableArrows, sliderMin,
            sliderMax, page, group, inputStyle, decimalPlaces, enableTrashCan
        );

        if (!MultiEditContext::isTriggerPopup(this)) return ret;

        for (auto node : CCArrayExt<CCNode>(ret)) {
            if (auto input = typeinfo_cast<CCTextInputNode*>(node)) {
                m_fields->addInput(input, property);
            } else if (auto slider = typeinfo_cast<Slider*>(node)) {
                m_fields->addSlider(slider, property);
            } else if (auto button = typeinfo_cast<CCMenuItemSpriteExtra*>(node)) {
                m_fields->addButton(button, property);
            } else if (auto bg = typeinfo_cast<CCScale9Sprite*>(node)) {
                m_fields->addInputBG(bg, property);
            } else if (auto label = typeinfo_cast<CCLabelBMFont*>(node)) {
                m_fields->addInputLabel(label, property);
            }
        }

        return ret;
    }

    $override
    void updateDefaultTriggerValues() {
        SetupTriggerPopup::updateDefaultTriggerValues();
        if (!MultiEditContext::isTriggerPopup(this)) return;

        // 4:3 aspect ratio fix
        if (CCDirector::get()->getWinSize().width < 520) {
            m_mainLayer->setPositionX(m_mainLayer->getPositionX() - (35 / 2));
        }

        m_fields->setupMixed();
    }
};

// some 2.0 triggers were likely made before rob had a good system for making trigger ui,
// so they don't use the usual functions for creating inputs, updating values, etc.
// meaning we have to deal with each on a case by case basis

MESetupTriggerPopup::Fields* getMultiEditContext(SetupTriggerPopup* stp) {
    return static_cast<MESetupTriggerPopup*>(stp)->m_fields.self();
}

#include <Geode/modify/ColorSelectPopup.hpp>

class $modify(ColorSelectPopup) {
    bool init(EffectGameObject* obj, CCArray* objs, ColorAction* action) {
        if (!ColorSelectPopup::init(obj, objs, action)) return false;
        if (!MultiEditContext::isTriggerPopup(this)) return true;

        auto ctx = getMultiEditContext(this);

        ctx->callback = [this](int property, float value) {
            if (property == 10) {
                MultiEditContext::setSliderValue(m_fadeTimeSlider, value, 0, 10);
            }
        };

        ctx->addInput(m_mainLayer->getChildByType<CCTextInputNode*>(0), 10);
        ctx->addInput(m_mainLayer->getChildByType<CCTextInputNode*>(1), 23);
        ctx->addInput(m_mainLayer->getChildByType<CCTextInputNode*>(2), 50);

        ctx->addInputBG(m_mainLayer->getChildByType<CCScale9Sprite*>(1), 10);
        ctx->addInputBG(m_mainLayer->getChildByType<CCScale9Sprite*>(2), 23);
        ctx->addInputBG(m_mainLayer->getChildByType<CCScale9Sprite*>(3), 50);

        m_mainLayer->getChildByType<CCTextInputNode*>(0)->setMaxLabelWidth(50);
        m_mainLayer->getChildByType<CCTextInputNode*>(1)->setMaxLabelWidth(40);

        ctx->addSlider(m_fadeTimeSlider, 10);
        m_opacitySlider->setUserObject("property"_spr, CCInteger::create(35));

        m_fadeTimeSlider->m_delegate = this;
        m_opacitySlider->m_delegate = this;

        ctx->addInputLabel(m_mainLayer->getChildByType<CCLabelBMFont*>(1), 10);

        ctx->addButton(m_buttonMenu->getChildByType<CCMenuItemSpriteExtra*>(4), 23);
        ctx->addButton(m_buttonMenu->getChildByType<CCMenuItemSpriteExtra*>(5), 23);
        ctx->addButton(m_buttonMenu->getChildByType<CCMenuItemSpriteExtra*>(6), 23);
        ctx->addButton(m_buttonMenu->getChildByType<CCMenuItemSpriteExtra*>(7), 50);
        ctx->addButton(m_buttonMenu->getChildByType<CCMenuItemSpriteExtra*>(8), 50);
        ctx->addButton(m_buttonMenu->getChildByType<CCMenuItemSpriteExtra*>(9), 50);

        ctx->setupMixed();

        return true;
    }

    void onToggleHSVMode(CCObject* sender) {
        ColorSelectPopup::onToggleHSVMode(sender);

        auto ctx = getMultiEditContext(this);
        ctx->onMixedInputApplied(50, 0);
    }
};

#include <Geode/modify/SetupPulsePopup.hpp>

class $modify(SetupPulsePopup) {
    bool init(EffectGameObject* obj, CCArray* objs) {
        if (!SetupPulsePopup::init(obj, objs)) return false;

        auto ctx = getMultiEditContext(this);
        ctx->addInput(m_fadeInInput, 45);
        ctx->addInput(m_holdInput, 46);
        ctx->addInput(m_fadeOutInput, 47);
        ctx->addInput(m_idInput, 51);
        ctx->addInput(m_colorIDInput, 50);

        ctx->addInputBG(m_mainLayer->getChildByType<CCScale9Sprite*>(1), 45);
        ctx->addInputBG(m_mainLayer->getChildByType<CCScale9Sprite*>(2), 46);
        ctx->addInputBG(m_mainLayer->getChildByType<CCScale9Sprite*>(3), 47);
        ctx->addInputBG(m_mainLayer->getChildByType<CCScale9Sprite*>(4), 51);
        ctx->addInputBG(m_mainLayer->getChildByType<CCScale9Sprite*>(10), 50);

        nk::fixCCScale9Sprite(m_mainLayer->getChildByType<CCScale9Sprite*>(1));
        nk::fixCCScale9Sprite(m_mainLayer->getChildByType<CCScale9Sprite*>(2));
        nk::fixCCScale9Sprite(m_mainLayer->getChildByType<CCScale9Sprite*>(3));
        m_mainLayer->getChildByType<CCScale9Sprite*>(1)->setColor({0, 0, 0});
        m_mainLayer->getChildByType<CCScale9Sprite*>(1)->setOpacity(100);
        m_mainLayer->getChildByType<CCScale9Sprite*>(2)->setColor({0, 0, 0});
        m_mainLayer->getChildByType<CCScale9Sprite*>(2)->setOpacity(100);
        m_mainLayer->getChildByType<CCScale9Sprite*>(3)->setColor({0, 0, 0});
        m_mainLayer->getChildByType<CCScale9Sprite*>(3)->setOpacity(100);

        m_idInput->setMaxLabelWidth(40);
        m_colorIDInput->setMaxLabelWidth(40);

        ctx->addSlider(m_fadeInSlider, 45);
        ctx->addSlider(m_holdSlider, 46);
        ctx->addSlider(m_fadeOutSlider, 47);

        m_fadeInSlider->m_delegate = this;
        m_holdSlider->m_delegate = this;
        m_fadeOutSlider->m_delegate = this;

        ctx->addInputLabel(m_mainLayer->getChildByType<CCLabelBMFont*>(15), 45);
        ctx->addInputLabel(m_mainLayer->getChildByType<CCLabelBMFont*>(16), 46);
        ctx->addInputLabel(m_mainLayer->getChildByType<CCLabelBMFont*>(17), 47);

        ctx->addButton(m_buttonMenu->getChildByType<CCMenuItemSpriteExtra*>(3), 51);
        ctx->addButton(m_buttonMenu->getChildByType<CCMenuItemSpriteExtra*>(4), 51);
        ctx->addButton(m_buttonMenu->getChildByType<CCMenuItemSpriteExtra*>(5), 51);
        ctx->addButton(m_buttonMenu->getChildByType<CCMenuItemSpriteExtra*>(10), 50);
        ctx->addButton(m_buttonMenu->getChildByType<CCMenuItemSpriteExtra*>(11), 50);
        ctx->addButton(m_buttonMenu->getChildByType<CCMenuItemSpriteExtra*>(12), 50);

        ctx->setupMixed();

        return true;
    }

    void onSelectTargetMode(CCObject* sender) {
        SetupPulsePopup::onSelectTargetMode(sender);

        auto ctx = getMultiEditContext(this);
        ctx->onMixedInputApplied(51, 0);
    }

    void onSelectPulseMode(CCObject* sender) {
        SetupPulsePopup::onSelectPulseMode(sender);

        if (auto btn = getMultiEditContext(this)->getMixedButton(50)) {
            btn->setVisible(m_pulseMode == 1);
        }
    }
};

#include <Geode/modify/SetupOpacityPopup.hpp>

class $modify(SetupOpacityPopup) {
    bool init(EffectGameObject* obj, CCArray* objs) {
        if (!SetupOpacityPopup::init(obj, objs)) return false;

        auto ctx = getMultiEditContext(this);

        ctx->callback = [this](int property, float value) {
            if (property == 10) {
                MultiEditContext::setSliderValue(m_fadeTimeSlider, value, 0, 10);
            }
        };

        ctx->addInput(m_mainLayer->getChildByType<CCTextInputNode*>(0), 51);
        ctx->addInput(m_mainLayer->getChildByType<CCTextInputNode*>(1), 10);
        ctx->addInputBG(m_mainLayer->getChildByType<CCScale9Sprite*>(1), 51);
        ctx->addInputBG(m_mainLayer->getChildByType<CCScale9Sprite*>(2), 10);

        ctx->addSlider(m_fadeTimeSlider, 10);
        m_opacitySlider->setUserObject("property"_spr, CCInteger::create(35));

        m_fadeTimeSlider->m_delegate = this;
        m_opacitySlider->m_delegate = this;

        ctx->addInputLabel(m_mainLayer->getChildByType<CCLabelBMFont*>(2), 10);
        ctx->addInputLabel(m_mainLayer->getChildByType<CCLabelBMFont*>(4), 35);

        ctx->addButton(m_buttonMenu->getChildByType<CCMenuItemSpriteExtra*>(2), 51);
        ctx->addButton(m_buttonMenu->getChildByType<CCMenuItemSpriteExtra*>(3), 51);

        m_mainLayer->getChildByType<CCTextInputNode*>(0)->setMaxLabelWidth(40);
        m_mainLayer->getChildByType<CCTextInputNode*>(1)->setMaxLabelWidth(50);

        ctx->setupMixed();

        return true;
    }
};

#include <Geode/modify/GJFollowCommandLayer.hpp>

class $modify(MEGJFollowCommandLayer, GJFollowCommandLayer) {
    bool init(EffectGameObject* obj, CCArray* objs) {
        if (!GJFollowCommandLayer::init(obj, objs)) return false;

        auto ctx = getMultiEditContext(this);

        ctx->callback = [this, ctx](int property, float value) {
            if (property == 10) {
                MultiEditContext::setSliderValue(m_moveTimeSlider, value, 0, 10);
            } else if (property == 72) {
                MultiEditContext::setSliderValue(m_xModSlider, value, -1, 1);
            } else if (property == 73) {
                MultiEditContext::setSliderValue(m_yModSlider, value, -1, 1);
            }
        };

        ctx->addInput(m_moveTimeInput, 10);
        ctx->addInput(m_xModInput, 72);
        ctx->addInput(m_yModInput, 73);
        ctx->addInput(m_targetIDInput, 51);
        ctx->addInput(m_followIDInput, 71);

        ctx->addInputBG(m_mainLayer->getChildByType<CCScale9Sprite*>(1), 10);
        ctx->addInputBG(m_mainLayer->getChildByType<CCScale9Sprite*>(2), 72);
        ctx->addInputBG(m_mainLayer->getChildByType<CCScale9Sprite*>(3), 73);
        ctx->addInputBG(m_mainLayer->getChildByType<CCScale9Sprite*>(4), 51);
        ctx->addInputBG(m_mainLayer->getChildByType<CCScale9Sprite*>(5), 71);

        m_moveTimeInput->setMaxLabelWidth(50);
        m_xModInput->setMaxLabelWidth(50);
        m_yModInput->setMaxLabelWidth(50);
        m_targetIDInput->setMaxLabelWidth(40);
        m_followIDInput->setMaxLabelWidth(40);

        ctx->addSlider(m_moveTimeSlider, 10);
        ctx->addSlider(m_xModSlider, 72);
        ctx->addSlider(m_yModSlider, 73);

        m_moveTimeSlider->m_delegate = this;
        m_xModSlider->m_delegate = this;
        m_yModSlider->m_delegate = this;

        ctx->addInputLabel(m_mainLayer->getChildByType<CCLabelBMFont*>(1), 10);
        ctx->addInputLabel(m_mainLayer->getChildByType<CCLabelBMFont*>(2), 72);
        ctx->addInputLabel(m_mainLayer->getChildByType<CCLabelBMFont*>(3), 73);

        ctx->addButton(m_buttonMenu->getChildByType<CCMenuItemSpriteExtra*>(2), 51);
        ctx->addButton(m_buttonMenu->getChildByType<CCMenuItemSpriteExtra*>(3), 51);
        ctx->addButton(m_buttonMenu->getChildByType<CCMenuItemSpriteExtra*>(4), 71);
        ctx->addButton(m_buttonMenu->getChildByType<CCMenuItemSpriteExtra*>(5), 71);

        ctx->setupMixed();

        return true;
    }
};

#include <Geode/modify/SetupCollisionTriggerPopup.hpp>

class $modify(SetupCollisionTriggerPopup) {
    bool init(EffectGameObject* obj, CCArray* objs) {
        if (!SetupCollisionTriggerPopup::init(obj, objs)) return false;

        auto ctx = getMultiEditContext(this);

        ctx->addInput(m_blockAInput, 80);
        ctx->addInput(m_blockBInput, 95);
        ctx->addInput(m_targetIDInput, 51);

        ctx->addInputBG(m_mainLayer->getChildByType<CCScale9Sprite*>(1), 80);
        ctx->addInputBG(m_mainLayer->getChildByType<CCScale9Sprite*>(2), 95);
        ctx->addInputBG(m_mainLayer->getChildByType<CCScale9Sprite*>(3), 51);

        m_blockAInput->setMaxLabelWidth(40);
        m_blockBInput->setMaxLabelWidth(40);
        m_targetIDInput->setMaxLabelWidth(40);

        ctx->addButton(m_buttonMenu->getChildByType<CCMenuItemSpriteExtra*>(2), 80);
        ctx->addButton(m_buttonMenu->getChildByType<CCMenuItemSpriteExtra*>(3), 80);
        ctx->addButton(m_buttonMenu->getChildByType<CCMenuItemSpriteExtra*>(4), 95);
        ctx->addButton(m_buttonMenu->getChildByType<CCMenuItemSpriteExtra*>(5), 95);
        ctx->addButton(m_buttonMenu->getChildByType<CCMenuItemSpriteExtra*>(6), 51);
        ctx->addButton(m_buttonMenu->getChildByType<CCMenuItemSpriteExtra*>(7), 51);

        ctx->setupMixed();

        return true;
    }
};

#include <Geode/modify/CollisionBlockPopup.hpp>
// note: CollisionBlockPopup isn't a subclass of SetupTriggerPopup

class $modify(MECollisionBlockPopup, CollisionBlockPopup) {
    struct Fields : MultiEditContext {
        CollisionBlockPopup* popup;

        void init(CollisionBlockPopup* popup) {
            this->popup = popup;
            registerSelf(popup);
        }

        float getProperty(GameObject* object, int property) override {
            if (property == 80) {
                auto collisionBlock = typeinfo_cast<EffectGameObject*>(object);
                return collisionBlock ? collisionBlock->m_itemID : 0;
            }

            return 0;
        }

        void setProperty(GameObject* object, int property, float value) override {
            if (property == 80) {
                auto collisionBlock = typeinfo_cast<EffectGameObject*>(object);
                if (collisionBlock) collisionBlock->m_itemID = value;
            }
        }

        bool hasProperty(GameObject* object, int property) override {
            return property == 80 && object->m_objectID == 1816;
        }

        int getPropertyDecimalPlaces(int property) override {
            return 0;
        }

        PropertyBounds getPropertyBounds(int property) override {
            return PropertyBounds(0, 999);
        }

        void onMixedInputApplied(int property, std::optional<float> value) override {
            updateMixedUI(property, value);

            for (auto obj : m_gameObjects) {
                LevelEditorLayer::get()->updateObjectLabel(obj);
            }

            if (value) {
                popup->m_blockIDInput->setString(nk::toString(*value, 0));
            }
        }

        CCArray* getObjectArray() override {
            if (auto obj = popup->m_gameObject) {
                return CCArray::createWithObject(obj);
            } else {
                return popup->m_gameObjects;
            }
        }
    };

    $override
    bool init(EffectGameObject* obj, CCArray* objs) {
        if (!CollisionBlockPopup::init(obj, objs)) return false;

        m_fields->init(this);

        m_fields->addInput(m_blockIDInput, 80);
        m_fields->addInputBG(m_mainLayer->getChildByType<CCScale9Sprite*>(1), 80);
        m_blockIDInput->setMaxLabelWidth(40);

        m_fields->addButton(m_buttonMenu->getChildByType<CCMenuItemSpriteExtra*>(2), 80);
        m_fields->addButton(m_buttonMenu->getChildByType<CCMenuItemSpriteExtra*>(3), 80);
        m_fields->addButton(m_buttonMenu->getChildByType<CCMenuItemSpriteExtra*>(4), 80);

        m_fields->setupMixed();

        return true;
    }
};

#include <Geode/modify/SetupTouchTogglePopup.hpp>

class $modify(MESetupTouchTogglePopup, SetupTouchTogglePopup) {
    bool init(EffectGameObject* obj, CCArray* fakeObjs) {
        if (!SetupTouchTogglePopup::init(obj, fakeObjs)) return false;

        auto ctx = getMultiEditContext(this);

        ctx->callback = [this, ctx](int property, float value) {
            for (auto obj : ctx->getGameObjects()) {
                LevelEditorLayer::get()->updateObjectLabel(obj);
            }
        };

        auto objs = EditorUI::get()->m_selectedObjects;

        if (objs && objs->count() > 0) {
            if (auto firstObj = typeinfo_cast<EffectGameObject*>(objs->firstObject())) {
                m_groupIDInput->setString(std::to_string(firstObj->m_targetGroupID).c_str());
            }
        }

        ctx->addInput(m_groupIDInput, 51);
        m_groupIDInput->setMaxLabelWidth(40);

        ctx->addInputBG(m_mainLayer->getChildByType<CCScale9Sprite*>(1), 51);

        ctx->addButton(m_buttonMenu->getChildByType<CCMenuItemSpriteExtra*>(2), 51);
        ctx->addButton(m_buttonMenu->getChildByType<CCMenuItemSpriteExtra*>(3), 51);

        ctx->setupMixed();

        return true;
    }

    $override
    void onTargetIDArrow(CCObject* sender) {
        SetupTouchTogglePopup::onTargetIDArrow(sender);

        auto ctx = getMultiEditContext(this);

        for (auto obj : ctx->getGameObjects()) {
            if (auto trigger = typeinfo_cast<EffectGameObject*>(obj)) {
                trigger->m_targetGroupID = m_groupID;
                LevelEditorLayer::updateObjectLabel(trigger);
            }
        }
    }
};

#include <Geode/modify/SetupCountTriggerPopup.hpp>

class $modify(SetupCountTriggerPopup) {
    bool init(EffectGameObject* obj, CCArray* objs) {
        if (!SetupCountTriggerPopup::init(obj, objs)) return false;

        auto ctx = getMultiEditContext(this);

        ctx->addInput(m_itemIDInput, 80);
        ctx->addInput(m_targetCountInput, 77);
        ctx->addInput(m_targetIDInput, 51);
        m_itemIDInput->setMaxLabelWidth(40);
        m_targetCountInput->setMaxLabelWidth(40);
        m_targetIDInput->setMaxLabelWidth(40);

        ctx->addInputBG(m_mainLayer->getChildByType<CCScale9Sprite*>(1), 80);
        ctx->addInputBG(m_mainLayer->getChildByType<CCScale9Sprite*>(2), 77);
        ctx->addInputBG(m_mainLayer->getChildByType<CCScale9Sprite*>(3), 51);

        ctx->addButton(m_buttonMenu->getChildByType<CCMenuItemSpriteExtra*>(2), 80);
        ctx->addButton(m_buttonMenu->getChildByType<CCMenuItemSpriteExtra*>(3), 80);
        ctx->addButton(m_buttonMenu->getChildByType<CCMenuItemSpriteExtra*>(4), 77);
        ctx->addButton(m_buttonMenu->getChildByType<CCMenuItemSpriteExtra*>(5), 77);
        ctx->addButton(m_buttonMenu->getChildByType<CCMenuItemSpriteExtra*>(6), 51);
        ctx->addButton(m_buttonMenu->getChildByType<CCMenuItemSpriteExtra*>(7), 51);

        ctx->setupMixed();

        return true;
    }
};

#include <Geode/modify/SetupInstantCountPopup.hpp>

class $modify(SetupInstantCountPopup) {
    bool init(CountTriggerGameObject* obj, CCArray* objs) {
        if (!SetupInstantCountPopup::init(obj, objs)) return false;

        auto ctx = getMultiEditContext(this);

        ctx->addInput(m_itemIDInput, 80);
        ctx->addInput(m_targetCountInput, 77);
        ctx->addInput(m_targetIDInput, 51);
        m_itemIDInput->setMaxLabelWidth(40);
        m_targetCountInput->setMaxLabelWidth(40);
        m_targetIDInput->setMaxLabelWidth(40);

        ctx->addInputBG(m_mainLayer->getChildByType<CCScale9Sprite*>(1), 80);
        ctx->addInputBG(m_mainLayer->getChildByType<CCScale9Sprite*>(2), 77);
        ctx->addInputBG(m_mainLayer->getChildByType<CCScale9Sprite*>(3), 51);

        ctx->addButton(m_buttonMenu->getChildByType<CCMenuItemSpriteExtra*>(2), 80);
        ctx->addButton(m_buttonMenu->getChildByType<CCMenuItemSpriteExtra*>(3), 80);
        ctx->addButton(m_buttonMenu->getChildByType<CCMenuItemSpriteExtra*>(4), 77);
        ctx->addButton(m_buttonMenu->getChildByType<CCMenuItemSpriteExtra*>(5), 77);
        ctx->addButton(m_buttonMenu->getChildByType<CCMenuItemSpriteExtra*>(6), 51);
        ctx->addButton(m_buttonMenu->getChildByType<CCMenuItemSpriteExtra*>(7), 51);

        ctx->setupMixed();

        return true;
    }
};

#include <Geode/modify/SetupRandTriggerPopup.hpp>

class $modify(MESetupRandTriggerPopup, SetupRandTriggerPopup) {
    bool init(EffectGameObject* obj, CCArray* objs) {
        if (!SetupRandTriggerPopup::init(obj, objs)) return false;

        auto ctx = getMultiEditContext(this);

        ctx->callback = [this, ctx](int property, float value) {
            if (property == 10) {
                MultiEditContext::setSliderValue(m_chanceSlider, value, 0, 100);
            }
        };

        ctx->addInput(m_groupID1Input, 51);
        ctx->addInput(m_groupID2Input, 71);
        ctx->addInput(m_chanceInput, 10);
        m_groupID1Input->setMaxLabelWidth(40);
        m_groupID2Input->setMaxLabelWidth(40);
        m_chanceInput->setMaxLabelWidth(50);

        ctx->addInputBG(m_mainLayer->getChildByType<CCScale9Sprite*>(1), 51);
        ctx->addInputBG(m_mainLayer->getChildByType<CCScale9Sprite*>(2), 71);
        ctx->addInputBG(m_mainLayer->getChildByType<CCScale9Sprite*>(3), 10);

        ctx->addSlider(m_chanceSlider, 10);
        m_chanceSlider->m_delegate = this;

        ctx->addInputLabel(m_mainLayer->getChildByType<CCLabelBMFont*>(3), 10);

        ctx->addButton(m_buttonMenu->getChildByType<CCMenuItemSpriteExtra*>(2), 51);
        ctx->addButton(m_buttonMenu->getChildByType<CCMenuItemSpriteExtra*>(3), 51);
        ctx->addButton(m_buttonMenu->getChildByType<CCMenuItemSpriteExtra*>(4), 71);
        ctx->addButton(m_buttonMenu->getChildByType<CCMenuItemSpriteExtra*>(5), 71);

        ctx->setupMixed();

        return true;
    }
};

#include <Geode/modify/SetupCameraOffsetTrigger.hpp>

class $modify(MESetupCameraOffsetTrigger, SetupCameraOffsetTrigger) {
    bool init(CameraTriggerGameObject* obj, CCArray* objs) {
        if (!SetupCameraOffsetTrigger::init(obj, objs)) return false;

        auto ctx = getMultiEditContext(this);
        
        ctx->callback = [this](int property, float value) {
            if (property == 10) {
                MultiEditContext::setSliderValue(m_moveTimeSlider, value, 0, 10);
            } else if (property == 28) {
                MultiEditContext::setSliderValue(m_offsetXSlider, value, -100, 100);
            } else if (property == 29) {
                MultiEditContext::setSliderValue(m_offsetYSlider, value, -100, 100);
            }
        };

        ctx->addInput(m_moveTimeInput, 10);
        ctx->addInput(m_offsetXInput, 28);
        ctx->addInput(m_offsetYInput, 29);

        ctx->addInputBG(m_mainLayer->getChildByType<CCScale9Sprite*>(1), 10);
        ctx->addInputBG(m_mainLayer->getChildByType<CCScale9Sprite*>(2), 28);
        ctx->addInputBG(m_mainLayer->getChildByType<CCScale9Sprite*>(3), 29);

        m_moveTimeInput->setMaxLabelWidth(50);
        m_offsetXInput->setMaxLabelWidth(50);
        m_offsetYInput->setMaxLabelWidth(50);

        ctx->addSlider(m_moveTimeSlider, 10);
        ctx->addSlider(m_offsetXSlider, 28);
        ctx->addSlider(m_offsetYSlider, 29);

        m_moveTimeSlider->m_delegate = this;
        m_offsetXSlider->m_delegate = this;
        m_offsetYSlider->m_delegate = this;

        ctx->addInputLabel(m_mainLayer->getChildByType<CCLabelBMFont*>(1), 10);
        ctx->addInputLabel(m_mainLayer->getChildByType<CCLabelBMFont*>(4), 28);
        ctx->addInputLabel(m_mainLayer->getChildByType<CCLabelBMFont*>(6), 29);

        ctx->setupMixed();

        return true;
    }
};

#include <Geode/modify/SetupCameraEdgePopup.hpp>

class $modify(SetupCameraEdgePopup) {
    bool init(CameraTriggerGameObject* obj, CCArray* objs) {
        if (!SetupCameraEdgePopup::init(obj, objs)) return false;

        auto ctx = getMultiEditContext(this);

        ctx->addInput(m_targetIDInput, 51);
        m_targetIDInput->setMaxLabelWidth(40);
        ctx->addInputBG(m_mainLayer->getChildByType<CCScale9Sprite*>(1), 51);

        ctx->setupMixed();

        return true;
    }
};

// special case for move triggers because move/mod x/y doesn't use rob's group and page system

#include <Geode/modify/SetupMoveCommandPopup.hpp>

class $modify(SetupMoveCommandPopup) {
    $override
    void valueDidChange(int property, float newValue) {
        SetupMoveCommandPopup::valueDidChange(property, newValue);
        if (property != 100 && property != 394) return;

        auto ctx = getMultiEditContext(this);
        bool isVisible = newValue == 0;

        if (auto btn = ctx->getMixedButton(28)) btn->setVisible(isVisible);
        if (auto btn = ctx->getMixedButton(29)) btn->setVisible(isVisible);
        if (auto btn = ctx->getMixedButton(143)) btn->setVisible(isVisible);
        if (auto btn = ctx->getMixedButton(144)) btn->setVisible(isVisible);
    }
};

// changing the mode in the advanced follow menu creates an entirely new popup for some reason
// this causes an issue where the advanced follow popup has a higher priority than the multi-edit popup

#include <Geode/modify/SetupAdvFollowPopup.hpp>

class $modify(SetupAdvFollowPopup) {
    $override
    void updateMode(int mode) {
        CCTouchDispatcher::get()->unregisterForcePrio(this);
        SetupAdvFollowPopup::updateMode(mode);
    }
};
