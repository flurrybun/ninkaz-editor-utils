#include <Geode/modify/SetupTriggerPopup.hpp>
#include "MultiEditManager.hpp"
#include "Trigger.hpp"
#include "../../misc/CCScale9SpriteFix.hpp"
#include "../../misc/StringUtils.hpp"

#include <Geode/Geode.hpp>
using namespace geode::prelude;

class $modify(MESetupTriggerPopup, SetupTriggerPopup) {
    struct Fields {
        Ref<MultiEditManager> multiEditManager;
    };

    static void onModify(auto& self) {
        (void)self.setHookPriority("SetupTriggerPopup::createValueControlAdvanced", Priority::VeryLatePost);
        (void)self.setHookPriority("SetupTriggerPopup::init", Priority::VeryEarlyPost);
    }

    $override
    bool init(EffectGameObject* obj, CCArray* objs, float f1, float f2, int i1) {
        if (!SetupTriggerPopup::init(obj, objs, f1, f2, i1)) return false;
        if (!Trigger::isTriggerPopup(this)) return true;

        auto mem = MultiEditManager::create(this, objs);
        m_fields->multiEditManager = mem;

        mem->setGroups(m_groupContainers);
        mem->setPages(m_pageContainers);
        mem->setCallback([this](int property, std::optional<float> value) {
            if (!value.has_value()) return;

            CCTextInputNode* input = m_fields->multiEditManager->getInputs()[property];
            if (!input) return;

            textChanged(m_fields->multiEditManager->getInputs()[property]);
        });


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

        if (!Trigger::isTriggerPopup(this)) return ret;

        for (auto node : CCArrayExt<CCNode>(ret)) {
            if (auto input = typeinfo_cast<CCTextInputNode*>(node)) {
                m_fields->multiEditManager->addInput(input, property);
            } else if (auto slider = typeinfo_cast<Slider*>(node)) {
                m_fields->multiEditManager->addSlider(slider, property);
            } else if (auto button = typeinfo_cast<CCMenuItemSpriteExtra*>(node)) {
                m_fields->multiEditManager->addButton(button, property);
            } else if (auto bg = typeinfo_cast<CCScale9Sprite*>(node)) {
                m_fields->multiEditManager->addInputBG(bg, property);
            } else if (auto label = typeinfo_cast<CCLabelBMFont*>(node)) {
                m_fields->multiEditManager->getInputLabels()[property] = label;
            }
        }

        return ret;
    }

    $override
    void updateDefaultTriggerValues() {
        SetupTriggerPopup::updateDefaultTriggerValues();
        if (!Trigger::isTriggerPopup(this)) return;

        // 4:3 aspect ratio fix
        if (CCDirector::get()->getWinSize().width < 520) {
            m_mainLayer->setPositionX(m_mainLayer->getPositionX() - (35 / 2));
        }

        m_fields->multiEditManager->setupMixed();
    }
};

// some 2.0 triggers were likely made before rob had a good system for making trigger ui,
// so they don't use the usual functions for creating inputs, updating values, etc.
// meaning we have to deal with each on a case by case basis

MultiEditManager* getMultiEditManager(SetupTriggerPopup* stp) {
    return static_cast<MESetupTriggerPopup*>(stp)->m_fields->multiEditManager;
}

#include <Geode/modify/ColorSelectPopup.hpp>

class $modify(ColorSelectPopup) {
    bool init(EffectGameObject* obj, CCArray* objs, ColorAction* action) {
        if (!ColorSelectPopup::init(obj, objs, action)) return false;
        if (!Trigger::isTriggerPopup(this)) return true;

        auto mem = getMultiEditManager(this);

        mem->setCallback([this, mem](int property, std::optional<float> value) {
            if (!value.has_value()) return;

            CCTextInputNode* input = mem->getInputs()[property];
            if (!input) return;

            mem->setInputValue(input, value.value());

            if (property == 10) {
                MultiEditManager::setSliderValue(m_fadeTimeSlider, value.value(), 0, 10);
            }
        });

        mem->addInput(m_mainLayer->getChildByType<CCTextInputNode*>(0), 10);
        mem->addInput(m_mainLayer->getChildByType<CCTextInputNode*>(1), 23);
        // mem->addInput(m_mainLayer->getChildByType<CCTextInputNode*>(2), 50);

        mem->addInputBG(m_mainLayer->getChildByType<CCScale9Sprite*>(1), 10);
        mem->addInputBG(m_mainLayer->getChildByType<CCScale9Sprite*>(2), 23);
        // mem->addInputBG(m_mainLayer->getChildByType<CCScale9Sprite*>(3), 50);

        m_mainLayer->getChildByType<CCTextInputNode*>(0)->setMaxLabelWidth(50);
        m_mainLayer->getChildByType<CCTextInputNode*>(1)->setMaxLabelWidth(40);

        mem->addSlider(m_fadeTimeSlider, 10);
        m_opacitySlider->setUserObject("property"_spr, CCInteger::create(35));

        m_fadeTimeSlider->m_delegate = this;
        m_opacitySlider->m_delegate = this;

        mem->addInputLabel(m_mainLayer->getChildByType<CCLabelBMFont*>(1), 10);

        mem->addButton(m_buttonMenu->getChildByType<CCMenuItemSpriteExtra*>(2), 51);
        mem->addButton(m_buttonMenu->getChildByType<CCMenuItemSpriteExtra*>(3), 51);

        mem->setupMixed();

        return true;
    }
};

#include <Geode/modify/SetupPulsePopup.hpp>

class $modify(SetupPulsePopup) {
    bool init(EffectGameObject* obj, CCArray* objs) {
        if (!SetupPulsePopup::init(obj, objs)) return false;

        auto mem = getMultiEditManager(this);

        mem->setCallback([this, mem](int property, std::optional<float> value) {
            if (!value.has_value()) return;

            CCTextInputNode* input = mem->getInputs()[property];
            if (!input) return;

            mem->setInputValue(input, value.value());
        });

        mem->addInput(m_fadeInInput, 45);
        mem->addInput(m_holdInput, 46);
        mem->addInput(m_fadeOutInput, 47);
        mem->addInput(m_idInput, 51);
        // mem->addInput(m_colorIDInput, 50);

        mem->addInputBG(m_mainLayer->getChildByType<CCScale9Sprite*>(1), 45);
        mem->addInputBG(m_mainLayer->getChildByType<CCScale9Sprite*>(2), 46);
        mem->addInputBG(m_mainLayer->getChildByType<CCScale9Sprite*>(3), 47);
        mem->addInputBG(m_mainLayer->getChildByType<CCScale9Sprite*>(4), 51);
        // mem->addInputBG(m_mainLayer->getChildByType<CCScale9Sprite*>(10), 50);

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

        mem->addSlider(m_fadeInSlider, 45);
        mem->addSlider(m_holdSlider, 46);
        mem->addSlider(m_fadeOutSlider, 47);

        m_fadeInSlider->m_delegate = this;
        m_holdSlider->m_delegate = this;
        m_fadeOutSlider->m_delegate = this;

        mem->addInputLabel(m_mainLayer->getChildByType<CCLabelBMFont*>(15), 45);
        mem->addInputLabel(m_mainLayer->getChildByType<CCLabelBMFont*>(16), 46);
        mem->addInputLabel(m_mainLayer->getChildByType<CCLabelBMFont*>(17), 47);

        mem->addButton(m_buttonMenu->getChildByType<CCMenuItemSpriteExtra*>(3), 51);
        mem->addButton(m_buttonMenu->getChildByType<CCMenuItemSpriteExtra*>(4), 51);
        // mem->addButton(m_buttonMenu->getChildByType<CCMenuItemSpriteExtra*>(10), 50);
        // mem->addButton(m_buttonMenu->getChildByType<CCMenuItemSpriteExtra*>(11), 50);

        mem->setupMixed();

        return true;
    }

    void onSelectTargetMode(CCObject* sender) {
        SetupPulsePopup::onSelectTargetMode(sender);

        auto mem = getMultiEditManager(this);
        if (!mem->getMixedButtons()[51]) return;

        mem->removeMixed(51, 0);

        if (m_gameObjects) {
            for (auto obj : CCArrayExt<EffectGameObject>(m_gameObjects)) {
                Trigger::setProperty(obj, 51, 0);
            }
        }
    }
};

#include <Geode/modify/SetupOpacityPopup.hpp>

class $modify(SetupOpacityPopup) {
    bool init(EffectGameObject* obj, CCArray* objs) {
        if (!SetupOpacityPopup::init(obj, objs)) return false;

        auto mem = getMultiEditManager(this);

        mem->setCallback([this, mem](int property, std::optional<float> value) {
            if (!value.has_value()) return;

            CCTextInputNode* input = mem->getInputs()[property];
            if (!input) return;

            mem->setInputValue(input, value.value());

            if (property == 10) {
                MultiEditManager::setSliderValue(m_fadeTimeSlider, value.value(), 0, 10);
            }
        });

        mem->addInput(m_mainLayer->getChildByType<CCTextInputNode*>(0), 51);
        mem->addInput(m_mainLayer->getChildByType<CCTextInputNode*>(1), 10);
        mem->addInputBG(m_mainLayer->getChildByType<CCScale9Sprite*>(1), 51);
        mem->addInputBG(m_mainLayer->getChildByType<CCScale9Sprite*>(2), 10);

        mem->addSlider(m_fadeTimeSlider, 10);
        m_opacitySlider->setUserObject("property"_spr, CCInteger::create(35));

        m_fadeTimeSlider->m_delegate = this;
        m_opacitySlider->m_delegate = this;

        mem->addInputLabel(m_mainLayer->getChildByType<CCLabelBMFont*>(2), 10);
        mem->addInputLabel(m_mainLayer->getChildByType<CCLabelBMFont*>(4), 35);

        mem->addButton(m_buttonMenu->getChildByType<CCMenuItemSpriteExtra*>(2), 51);
        mem->addButton(m_buttonMenu->getChildByType<CCMenuItemSpriteExtra*>(3), 51);

        m_mainLayer->getChildByType<CCTextInputNode*>(0)->setMaxLabelWidth(40);
        m_mainLayer->getChildByType<CCTextInputNode*>(1)->setMaxLabelWidth(50);

        mem->setupMixed();

        return true;
    }
};

#include <Geode/modify/GJFollowCommandLayer.hpp>

class $modify(GJFollowCommandLayer) {
    bool init(EffectGameObject* obj, CCArray* objs) {
        if (!GJFollowCommandLayer::init(obj, objs)) return false;

        auto mem = getMultiEditManager(this);

        mem->setCallback([this, mem](int property, std::optional<float> value) {
            if (!value.has_value()) return;

            CCTextInputNode* input = mem->getInputs()[property];
            if (!input) return;

            mem->setInputValue(input, value.value());

            if (property == 10) {
                MultiEditManager::setSliderValue(m_moveTimeSlider, value.value(), 0, 10);
            } else if (property == 72) {
                MultiEditManager::setSliderValue(m_xModSlider, value.value(), -1, 1);
            } else if (property == 73) {
                MultiEditManager::setSliderValue(m_yModSlider, value.value(), -1, 1);
            }
        });

        mem->addInput(m_moveTimeInput, 10);
        mem->addInput(m_xModInput, 72);
        mem->addInput(m_yModInput, 73);
        mem->addInput(m_targetIDInput, 51);
        mem->addInput(m_followIDInput, 71);

        mem->addInputBG(m_mainLayer->getChildByType<CCScale9Sprite*>(1), 10);
        mem->addInputBG(m_mainLayer->getChildByType<CCScale9Sprite*>(2), 72);
        mem->addInputBG(m_mainLayer->getChildByType<CCScale9Sprite*>(3), 73);
        mem->addInputBG(m_mainLayer->getChildByType<CCScale9Sprite*>(4), 51);
        mem->addInputBG(m_mainLayer->getChildByType<CCScale9Sprite*>(5), 71);

        m_moveTimeInput->setMaxLabelWidth(50);
        m_xModInput->setMaxLabelWidth(50);
        m_yModInput->setMaxLabelWidth(50);
        m_targetIDInput->setMaxLabelWidth(40);
        m_followIDInput->setMaxLabelWidth(40);

        mem->addSlider(m_moveTimeSlider, 10);
        mem->addSlider(m_xModSlider, 72);
        mem->addSlider(m_yModSlider, 73);

        m_moveTimeSlider->m_delegate = this;
        m_xModSlider->m_delegate = this;
        m_yModSlider->m_delegate = this;

        mem->addInputLabel(m_mainLayer->getChildByType<CCLabelBMFont*>(1), 10);
        mem->addInputLabel(m_mainLayer->getChildByType<CCLabelBMFont*>(2), 72);
        mem->addInputLabel(m_mainLayer->getChildByType<CCLabelBMFont*>(3), 73);

        mem->addButton(m_buttonMenu->getChildByType<CCMenuItemSpriteExtra*>(2), 51);
        mem->addButton(m_buttonMenu->getChildByType<CCMenuItemSpriteExtra*>(3), 51);
        mem->addButton(m_buttonMenu->getChildByType<CCMenuItemSpriteExtra*>(4), 71);
        mem->addButton(m_buttonMenu->getChildByType<CCMenuItemSpriteExtra*>(5), 71);

        mem->setupMixed();

        return true;
    }
};

#include <Geode/modify/SetupCollisionTriggerPopup.hpp>

class $modify(SetupCollisionTriggerPopup) {
    bool init(EffectGameObject* obj, CCArray* objs) {
        if (!SetupCollisionTriggerPopup::init(obj, objs)) return false;

        auto mem = getMultiEditManager(this);

        mem->setCallback([this, mem](int property, std::optional<float> value) {
            if (!value.has_value()) return;

            CCTextInputNode* input = mem->getInputs()[property];
            if (!input) return;

            mem->setInputValue(input, value.value());
        });

        mem->addInput(m_blockAInput, 80);
        mem->addInput(m_blockBInput, 95);
        mem->addInput(m_targetIDInput, 51);

        mem->addInputBG(m_mainLayer->getChildByType<CCScale9Sprite*>(1), 80);
        mem->addInputBG(m_mainLayer->getChildByType<CCScale9Sprite*>(2), 95);
        mem->addInputBG(m_mainLayer->getChildByType<CCScale9Sprite*>(3), 51);

        m_blockAInput->setMaxLabelWidth(40);
        m_blockBInput->setMaxLabelWidth(40);
        m_targetIDInput->setMaxLabelWidth(40);

        mem->addButton(m_buttonMenu->getChildByType<CCMenuItemSpriteExtra*>(2), 80);
        mem->addButton(m_buttonMenu->getChildByType<CCMenuItemSpriteExtra*>(3), 80);
        mem->addButton(m_buttonMenu->getChildByType<CCMenuItemSpriteExtra*>(4), 95);
        mem->addButton(m_buttonMenu->getChildByType<CCMenuItemSpriteExtra*>(5), 95);
        mem->addButton(m_buttonMenu->getChildByType<CCMenuItemSpriteExtra*>(6), 51);
        mem->addButton(m_buttonMenu->getChildByType<CCMenuItemSpriteExtra*>(7), 51);

        mem->setupMixed();

        return true;
    }
};

#include <Geode/modify/CollisionBlockPopup.hpp>
// note: CollisionBlockPopup isn't a subclass of SetupTriggerPopup

class $modify(CollisionBlockPopup) {
    struct Fields {
        Ref<MultiEditManager> multiEditManager;
    };

    $override
    bool init(EffectGameObject* obj, CCArray* objs) {
        if (!CollisionBlockPopup::init(obj, objs)) return false;

        auto mem = MultiEditManager::create(this, objs);
        m_fields->multiEditManager = mem;

        mem->setCallback([this](int property, std::optional<float> value) {
            for (auto obj : CCArrayExt<GameObject*>(m_gameObjects)) {
                LevelEditorLayer::updateObjectLabel(obj);
            }

            if (!value.has_value()) return;

            m_blockIDInput->setString(nk::toString(value.value(), 0));
        });

        mem->addInput(m_blockIDInput, 80);
        mem->addInputBG(m_mainLayer->getChildByType<CCScale9Sprite*>(1), 80);
        m_blockIDInput->setMaxLabelWidth(40);

        mem->addButton(m_buttonMenu->getChildByType<CCMenuItemSpriteExtra*>(2), 80);
        mem->addButton(m_buttonMenu->getChildByType<CCMenuItemSpriteExtra*>(3), 80);
        mem->addButton(m_buttonMenu->getChildByType<CCMenuItemSpriteExtra*>(4), 80);

        mem->setupMixed();

        return true;
    }
};

#include <Geode/modify/SetupTouchTogglePopup.hpp>

class $modify(SetupTouchTogglePopup) {
    bool init(EffectGameObject* obj, CCArray* fakeObjs) {
        if (!SetupTouchTogglePopup::init(obj, fakeObjs)) return false;

        auto mem = getMultiEditManager(this);

        // for some reason, this trigger popup and this trigger popup alone sets
        // m_gameObjects to nullptr, so we have to work around it

        auto objs = EditorUI::get()->m_selectedObjects;
        mem->setGameObjects(objs);

        mem->setCallback([this, mem](int property, std::optional<float> value) {
            if (!value.has_value()) return;

            CCTextInputNode* input = mem->getInputs()[property];
            if (!input) return;

            mem->setInputValue(input, value.value());

            if (property == 51) {
                for (auto obj : mem->getGameObjects()) {
                    LevelEditorLayer::updateObjectLabel(obj);
                }
            }
        });

        if (objs && objs->count() > 0) {
            if (auto firstObj = typeinfo_cast<EffectGameObject*>(objs->firstObject())) {
                m_groupIDInput->setString(std::to_string(firstObj->m_targetGroupID).c_str());
            }
        }

        mem->addInput(m_groupIDInput, 51);
        m_groupIDInput->setMaxLabelWidth(40);

        mem->addInputBG(m_mainLayer->getChildByType<CCScale9Sprite*>(1), 51);

        mem->addButton(m_buttonMenu->getChildByType<CCMenuItemSpriteExtra*>(2), 51);
        mem->addButton(m_buttonMenu->getChildByType<CCMenuItemSpriteExtra*>(3), 51);

        mem->setupMixed();

        return true;
    }

    $override
    void onTargetIDArrow(CCObject* sender) {
        SetupTouchTogglePopup::onTargetIDArrow(sender);

        auto mem = getMultiEditManager(this);
        auto objs = mem->getGameObjects();

        for (auto obj : objs) {
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

        auto mem = getMultiEditManager(this);

        mem->setCallback([this, mem](int property, std::optional<float> value) {
            if (!value.has_value()) return;

            CCTextInputNode* input = mem->getInputs()[property];
            if (!input) return;

            mem->setInputValue(input, value.value());
        });

        mem->addInput(m_itemIDInput, 80);
        mem->addInput(m_targetCountInput, 77);
        mem->addInput(m_targetIDInput, 51);
        m_itemIDInput->setMaxLabelWidth(40);
        m_targetCountInput->setMaxLabelWidth(40);
        m_targetIDInput->setMaxLabelWidth(40);

        mem->addInputBG(m_mainLayer->getChildByType<CCScale9Sprite*>(1), 80);
        mem->addInputBG(m_mainLayer->getChildByType<CCScale9Sprite*>(2), 77);
        mem->addInputBG(m_mainLayer->getChildByType<CCScale9Sprite*>(3), 51);

        mem->addButton(m_buttonMenu->getChildByType<CCMenuItemSpriteExtra*>(2), 80);
        mem->addButton(m_buttonMenu->getChildByType<CCMenuItemSpriteExtra*>(3), 80);
        mem->addButton(m_buttonMenu->getChildByType<CCMenuItemSpriteExtra*>(4), 77);
        mem->addButton(m_buttonMenu->getChildByType<CCMenuItemSpriteExtra*>(5), 77);
        mem->addButton(m_buttonMenu->getChildByType<CCMenuItemSpriteExtra*>(6), 51);
        mem->addButton(m_buttonMenu->getChildByType<CCMenuItemSpriteExtra*>(7), 51);

        mem->setupMixed();

        return true;
    }
};

#include <Geode/modify/SetupInstantCountPopup.hpp>

class $modify(SetupInstantCountPopup) {
    bool init(CountTriggerGameObject* obj, CCArray* objs) {
        if (!SetupInstantCountPopup::init(obj, objs)) return false;

        auto mem = getMultiEditManager(this);

        mem->setCallback([this, mem](int property, std::optional<float> value) {
            if (!value.has_value()) return;

            CCTextInputNode* input = mem->getInputs()[property];
            if (!input) return;

            mem->setInputValue(input, value.value());
        });

        mem->addInput(m_itemIDInput, 80);
        mem->addInput(m_targetCountInput, 77);
        mem->addInput(m_targetIDInput, 51);
        m_itemIDInput->setMaxLabelWidth(40);
        m_targetCountInput->setMaxLabelWidth(40);
        m_targetIDInput->setMaxLabelWidth(40);

        mem->addInputBG(m_mainLayer->getChildByType<CCScale9Sprite*>(1), 80);
        mem->addInputBG(m_mainLayer->getChildByType<CCScale9Sprite*>(2), 77);
        mem->addInputBG(m_mainLayer->getChildByType<CCScale9Sprite*>(3), 51);

        mem->addButton(m_buttonMenu->getChildByType<CCMenuItemSpriteExtra*>(2), 80);
        mem->addButton(m_buttonMenu->getChildByType<CCMenuItemSpriteExtra*>(3), 80);
        mem->addButton(m_buttonMenu->getChildByType<CCMenuItemSpriteExtra*>(4), 77);
        mem->addButton(m_buttonMenu->getChildByType<CCMenuItemSpriteExtra*>(5), 77);
        mem->addButton(m_buttonMenu->getChildByType<CCMenuItemSpriteExtra*>(6), 51);
        mem->addButton(m_buttonMenu->getChildByType<CCMenuItemSpriteExtra*>(7), 51);

        mem->setupMixed();

        return true;
    }
};

#include <Geode/modify/SetupRandTriggerPopup.hpp>

class $modify(SetupRandTriggerPopup) {
    bool init(EffectGameObject* obj, CCArray* objs) {
        if (!SetupRandTriggerPopup::init(obj, objs)) return false;

        auto mem = getMultiEditManager(this);

        mem->setCallback([this, mem](int property, std::optional<float> value) {
            if (!value.has_value()) return;

            CCTextInputNode* input = mem->getInputs()[property];
            if (!input) return;

            mem->setInputValue(input, value.value());

            if (property == 10) {
                MultiEditManager::setSliderValue(m_chanceSlider, value.value(), 0, 100);
            }
        });

        mem->addInput(m_groupID1Input, 51);
        mem->addInput(m_groupID2Input, 71);
        mem->addInput(m_chanceInput, 10);
        m_groupID1Input->setMaxLabelWidth(40);
        m_groupID2Input->setMaxLabelWidth(40);
        m_chanceInput->setMaxLabelWidth(50);

        mem->addInputBG(m_mainLayer->getChildByType<CCScale9Sprite*>(1), 51);
        mem->addInputBG(m_mainLayer->getChildByType<CCScale9Sprite*>(2), 71);
        mem->addInputBG(m_mainLayer->getChildByType<CCScale9Sprite*>(3), 10);

        mem->addSlider(m_chanceSlider, 10);
        m_chanceSlider->m_delegate = this;

        mem->addInputLabel(m_mainLayer->getChildByType<CCLabelBMFont*>(3), 10);

        mem->addButton(m_buttonMenu->getChildByType<CCMenuItemSpriteExtra*>(2), 51);
        mem->addButton(m_buttonMenu->getChildByType<CCMenuItemSpriteExtra*>(3), 51);
        mem->addButton(m_buttonMenu->getChildByType<CCMenuItemSpriteExtra*>(4), 71);
        mem->addButton(m_buttonMenu->getChildByType<CCMenuItemSpriteExtra*>(5), 71);

        mem->setupMixed();

        return true;
    }
};

#include <Geode/modify/SetupCameraOffsetTrigger.hpp>

class $modify(SetupCameraOffsetTrigger) {
    bool init(CameraTriggerGameObject* obj, CCArray* objs) {
        if (!SetupCameraOffsetTrigger::init(obj, objs)) return false;

        auto mem = getMultiEditManager(this);

        mem->setCallback([this, mem](int property, std::optional<float> value) {
            if (!value.has_value()) return;

            CCTextInputNode* input = mem->getInputs()[property];
            if (!input) return;

            mem->setInputValue(input, value.value());

            if (property == 10) {
                MultiEditManager::setSliderValue(m_moveTimeSlider, value.value(), 0, 10);
            } else if (property == 28) {
                MultiEditManager::setSliderValue(m_offsetXSlider, value.value(), -100, 100);
            } else if (property == 29) {
                MultiEditManager::setSliderValue(m_offsetYSlider, value.value(), -100, 100);
            }
        });

        mem->addInput(m_moveTimeInput, 10);
        mem->addInput(m_offsetXInput, 28);
        mem->addInput(m_offsetYInput, 29);

        mem->addInputBG(m_mainLayer->getChildByType<CCScale9Sprite*>(1), 10);
        mem->addInputBG(m_mainLayer->getChildByType<CCScale9Sprite*>(2), 28);
        mem->addInputBG(m_mainLayer->getChildByType<CCScale9Sprite*>(3), 29);

        m_moveTimeInput->setMaxLabelWidth(50);
        m_offsetXInput->setMaxLabelWidth(50);
        m_offsetYInput->setMaxLabelWidth(50);

        mem->addSlider(m_moveTimeSlider, 10);
        mem->addSlider(m_offsetXSlider, 28);
        mem->addSlider(m_offsetYSlider, 29);

        m_moveTimeSlider->m_delegate = this;
        m_offsetXSlider->m_delegate = this;
        m_offsetYSlider->m_delegate = this;

        mem->addInputLabel(m_mainLayer->getChildByType<CCLabelBMFont*>(1), 10);
        mem->addInputLabel(m_mainLayer->getChildByType<CCLabelBMFont*>(4), 28);
        mem->addInputLabel(m_mainLayer->getChildByType<CCLabelBMFont*>(6), 29);

        mem->setupMixed();

        return true;
    }
};

#include <Geode/modify/SetupCameraEdgePopup.hpp>

class $modify(SetupCameraEdgePopup) {
    bool init(CameraTriggerGameObject* obj, CCArray* objs) {
        if (!SetupCameraEdgePopup::init(obj, objs)) return false;

        auto mem = getMultiEditManager(this);

        mem->setCallback([this, mem](int property, std::optional<float> value) {
            if (!value.has_value()) return;

            CCTextInputNode* input = mem->getInputs()[property];
            if (!input) return;

            mem->setInputValue(input, value.value());
        });

        mem->addInput(m_targetIDInput, 51);
        m_targetIDInput->setMaxLabelWidth(40);
        mem->addInputBG(m_mainLayer->getChildByType<CCScale9Sprite*>(1), 51);

        mem->setupMixed();

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

        auto mem = getMultiEditManager(this);
        bool isVisible = newValue == 0;

        if (auto btn = mem->getMixedButton(28)) btn->setVisible(isVisible);
        if (auto btn = mem->getMixedButton(29)) btn->setVisible(isVisible);
        if (auto btn = mem->getMixedButton(143)) btn->setVisible(isVisible);
        if (auto btn = mem->getMixedButton(144)) btn->setVisible(isVisible);
    }
};
