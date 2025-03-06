#include <Geode/modify/SetupTriggerPopup.hpp>

#include <Geode/Geode.hpp>
using namespace geode::prelude;

class $modify(TNFSetupTriggerPopup, SetupTriggerPopup) {
    $override
    CCArray* createValueControlAdvanced(
        int property, gd::string label, CCPoint position, float scale, bool disableSlider, InputValueType valueType,
        int charCountLimit, bool enableArrows, float sliderMin, float sliderMax, int page, int group,
        GJInputStyle inputStyle, int decimalPlaces, bool enableTrashCan
    ) {
        // shorten some especially long labels
        if (label == "Target Group ID:") label = "Target Group:";
        else if (label == "Center Group ID:") label = "Center Group:";
        else if (label == "TargetPos Group ID:") label = "TargetPos Group:";

        CCArray* ret = SetupTriggerPopup::createValueControlAdvanced(
            property, label, position, scale, disableSlider, valueType, charCountLimit, enableArrows, sliderMin,
            sliderMax, page, group, inputStyle, decimalPlaces, enableTrashCan
        );

        if (property != 51 && property != 71) return ret;
        if (inputStyle == GJInputStyle::ChatLabel) return ret;
        if (!enableArrows) return ret;
        // area stop trigger uses property 51 for effect id
        if (label == "EffectID:") return ret;

        CCTextInputNode* inputNode;
        CCLabelBMFont* labelNode;

        for (auto node : CCArrayExt<CCNode>(ret)) {
            if (auto input = typeinfo_cast<CCTextInputNode*>(node)) inputNode = input;
            if (auto label = typeinfo_cast<CCLabelBMFont*>(node)) labelNode = label;
        }

        auto plusSpr = CCSprite::createWithSpriteFrameName("GJ_plus2Btn_001.png");
        plusSpr->setScale(scale);

        auto plusBtn = CCMenuItemSpriteExtra::create(plusSpr, this, menu_selector(TNFSetupTriggerPopup::onNextFree));
        plusBtn->setTag(property);
        plusBtn->setID(property == 51 ? "target-group-next-free"_spr : "center-group-next-free"_spr);
        plusBtn->setUserObject("input"_spr, inputNode);

        if (auto arr = typeinfo_cast<CCArray*>(m_groupContainers->objectAtIndex(group))) arr->addObject(plusBtn);
        if (auto arr = typeinfo_cast<CCArray*>(m_pageContainers->objectAtIndex(page))) arr->addObject(plusBtn);

        constexpr float GAP = 3;
        float labelX = -(plusBtn->getScaledContentWidth() + GAP) / 2;
        float btnX = (labelNode->getScaledContentWidth() + GAP) / 2;

        plusBtn->setPosition(position - m_buttonMenu->getPosition() + ccp(btnX, 33 * scale));
        labelNode->setPositionX(position.x + labelX);

        m_buttonMenu->addChild(plusBtn);
        ret->addObject(plusBtn);

        return ret;
    }

    void onNextFree(CCObject* sender) {
        auto btn = static_cast<CCMenuItemSpriteExtra*>(sender);
        auto input = typeinfo_cast<CCTextInputNode*>(btn->getUserObject("input"_spr));
        if (!input) return;

        auto lel = LevelEditorLayer::get();
        int nextFree = lel->getNextFreeGroupID(lel->m_objects);

        input->setString(std::to_string(nextFree));
        input->m_delegate->textChanged(input);
    }
};
