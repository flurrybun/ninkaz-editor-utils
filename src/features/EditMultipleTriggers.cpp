#include <Geode/modify/SetupTriggerPopup.hpp>
#include <Geode/modify/EditTriggersPopup.hpp>

#include <Geode/Geode.hpp>
using namespace geode::prelude;

class $modify(SetupTriggerPopup) {
    bool init(EffectGameObject* trigger, CCArray* triggers, float width, float height, int unkEnum) {
        // increase popup size
        if (typeinfo_cast<EditTriggersPopup*>(this)) {
            width = 440;
            height = 310;
        }

        return SetupTriggerPopup::init(trigger, triggers, width, height, unkEnum);
    }
};

class $modify(EditMultipleTriggersPopup, EditTriggersPopup) {
    bool init(EffectGameObject* obj, CCArray* objs) {
        if (!EditTriggersPopup::init(obj, objs)) return false;

        // change spawn/touch trigger toggle positions

        auto touchTriggerBtn = m_buttonMenu->getChildByType<CCMenuItemToggler*>(0);
        auto touchTriggerLabel = m_mainLayer->getChildByType<CCLabelBMFont*>(0);
        auto spawnTriggerBtn = m_buttonMenu->getChildByType<CCMenuItemToggler*>(1);
        auto spawnTriggerLabel = m_mainLayer->getChildByType<CCLabelBMFont*>(1);

        touchTriggerBtn->setPosition(touchTriggerBtn->getPosition() + ccp(0, -30));
        touchTriggerLabel->setPosition(touchTriggerLabel->getPosition() + ccp(0, -30));
        spawnTriggerBtn->setPosition(spawnTriggerBtn->getPosition() + ccp(90, 0));
        spawnTriggerLabel->setPosition(spawnTriggerLabel->getPosition() + ccp(90, 0));

        // SetupTriggerPopup::preSetup
        m_disableTextDelegate = true;
        SetupTriggerPopup::determineStartValues();

        CCPoint center = CCDirector::get()->getWinSize() / 2 + ccp(0, 3);
        createValueControlAdvanced(51, "Target Group ID:", center + ccp(-70, 25), 0.8, true, InputValueType::Int, 10, true, 0, 10, 0, 0, GJInputStyle::GoldLabel, 0, false);
        createValueControlAdvanced(71, "Center Group ID:", center + ccp(70, 25), 0.8, true, InputValueType::Int, 10, true, 0, 10, 0, 0, GJInputStyle::GoldLabel, 0, false);
        createEasingControls(center + ccp(-90, -20), 0.8, 0, 0);
        createValueControlAdvanced(10, "Duration:", center + ccp(90, -25), 0.8, false, InputValueType::Float, 10, false, 0, 10, 0, 0, GJInputStyle::GoldLabel, 0, false);

        // SetupTriggerPopup::postSetup
        this->updateDefaultTriggerValues();
        m_disableTextDelegate = false;

        return true;
    }
};
