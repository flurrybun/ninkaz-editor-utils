#include "MultiEditTriggers.hpp"
#include "MixedInputPopup.hpp"
#include <Geode/modify/EditTriggersPopup.hpp>
#include <Geode/ui/TextInput.hpp>

#include <Geode/Geode.hpp>
using namespace geode::prelude;

#include <map>
#include <vector>
#include <algorithm>
#include <iterator>

class $modify(MultiEditTriggersPopup, EditTriggersPopup) {
    struct Fields {
        std::vector<Trigger> m_triggers;
        CCLabelBMFont* m_easingLabel;
    };

    std::string floatToRoundedString(float value) {
        value = std::round(value * 100) / 100;
        auto valueStr = std::to_string(value);
        valueStr.erase(valueStr.size() - 4);

        return valueStr;
    }

    CCMenu* createGroupMenu(GroupMenuType type, std::optional<int> initialValue) {
        auto menu = CCMenu::create();
        menu->setContentSize({ 122, 50 });
        auto center = ccp(61, 25);

        auto input = TextInput::create(70.f, "Num", "bigFont.fnt");
        input->setFilter("0123456789");
        input->setMaxCharCount(4);
        input->setScale(0.8);
        input->setPosition(center + ccp(0, -12));
        
        input->setString(initialValue.has_value() ? std::to_string(*initialValue) : "Mixed");

        input->setCallback([this, type](const std::string &text) {
            if (text.empty()) return;

            auto value = std::stoi(text);
            if (value < 0) value = 0;
            if (value > 9999) value = 9999;

            for (auto& trigger : m_fields->m_triggers) {
                if (type == GroupMenuType::TargetGroup && trigger.hasTargetGroup) {
                    trigger.object->m_targetGroupID = value;
                } else if (type == GroupMenuType::CenterGroup && trigger.hasCenterGroup) {
                    trigger.object->m_centerGroupID = value;
                } else if (type == GroupMenuType::Item && trigger.hasItem) {
                    trigger.object->m_itemID = value;
                }
            }
        });

        auto labelText = type == GroupMenuType::TargetGroup ? "Target Group ID:" :
            type == GroupMenuType::CenterGroup ? "Center Group ID:" : "Item ID:";
        auto label = CCLabelBMFont::create(labelText, "goldFont.fnt");
        label->setPosition(center + ccp(0, 17));
        label->setScale(0.56);

        auto decArrowSpr = CCSprite::createWithSpriteFrameName("edit_leftBtn_001.png");
        decArrowSpr->setScale(0.96);
        auto decArrowBtn = CCMenuItemSpriteExtra::create(
            decArrowSpr, this, nullptr
        );
        decArrowBtn->setPosition(center + ccp(-45, -12));
        decArrowBtn->setTag(-1);

        auto incArrowSpr = CCSprite::createWithSpriteFrameName("edit_rightBtn_001.png");
        incArrowSpr->setScale(0.96);
        auto incArrowBtn = CCMenuItemSpriteExtra::create(
            incArrowSpr, this, nullptr
        );
        incArrowBtn->setPosition(center + ccp(45, -12));
        incArrowBtn->setTag(1);

        menu->addChild(input);
        menu->addChild(label);
        menu->addChild(decArrowBtn);
        menu->addChild(incArrowBtn);

        menu->setTouchPriority(-504);

        return menu;
    };

    void onSliderChanged(CCObject* sender) {
        auto sliderThumb = typeinfo_cast<SliderThumb*>(sender);
        auto tag = sliderThumb->getTag();
        auto value = sliderThumb->getValue();

        if (tag == SliderMenuType::Duration) value *= 10;

        auto menu = typeinfo_cast<CCMenu*>(sliderThumb->getParent()->getParent()->getParent());
        auto input = getChildOfType<TextInput>(menu, 0);

        input->setString(floatToRoundedString(value), true);
    }

    CCMenu* createSliderMenu(SliderMenuType type, std::optional<float> initialValue) {
        auto menu = CCMenu::create();
        menu->setContentSize({ 146, 46 });
        auto center = ccp(73, 23);

        auto slider = Slider::create(this, menu_selector(MultiEditTriggersPopup::onSliderChanged), 0.64);
        slider->setPosition(center + ccp(0, -15));
        slider->getThumb()->setTag(type);

        if (type == SliderMenuType::Duration) slider->setValue(initialValue.has_value() ? *initialValue / 10 : 0.f);
        else if (type == SliderMenuType::Opacity) slider->setValue(initialValue.has_value() ? *initialValue : 0.f);

        auto input = TextInput::create(70.f, "Num", "bigFont.fnt");
        input->setFilter("0123456789");
        input->setMaxCharCount(4);
        input->setScale(0.8);
        input->setPosition(center + ccp(31, 11));

        input->setString(initialValue.has_value() ? floatToRoundedString(*initialValue) : "Mixed");

        input->setCallback([this, type, slider](const std::string &text) {
            if (text.empty()) return;

            auto value = std::stof(text);
            if (type == SliderMenuType::Opacity) value = std::clamp(value, 0.0f, 1.0f);
            if (type == SliderMenuType::Duration) slider->setValue(value / 10);

            for (auto& trigger : m_fields->m_triggers) {
                if (type == SliderMenuType::Duration && trigger.hasDuration) {
                    trigger.object->m_duration = value;
                } else if (type == SliderMenuType::Opacity && trigger.hasOpacity) {
                    trigger.object->m_opacity = value;
                }
            }
        });
        
        auto labelText = type == SliderMenuType::Duration ? "Duration:" : "Opacity:";
        auto label = CCLabelBMFont::create(labelText, "goldFont.fnt");
        label->setPosition(center + ccp(0, 11));
        label->setAnchorPoint({ 1, 0.5 });
        label->setScale(0.56);

        menu->addChild(input);
        menu->addChild(slider);
        menu->addChild(label);

        menu->setTouchPriority(-504);
        menu->setScale(1.1);

        return menu;
    };

    void onEasingChange(CCObject* sender) {
        auto easingStrings = std::vector<std::string> {
            "None",
            "Ease In Out", "Ease In", "Ease Out",
            "Elastic In Out", "Elastic In", "Elastic Out",
            "Bounce In Out", "Bounce In", "Bounce Out",
            "Exponential In Out", "Exponential In", "Exponential Out",
            "Sine In Out", "Sine In", "Sine Out",
            "Back In Out", "Back In", "Back Out"
        };

        auto& label = m_fields->m_easingLabel;
        auto labelString = label->getString();

        auto index = find(easingStrings.begin(), easingStrings.end(), labelString) - easingStrings.begin();
        if (index >= easingStrings.size()) index = 0;
        
        index += sender->getTag();
        if (index < 0) index = 18;
        if (index > 18) index = 0;

        auto newString = easingStrings[index];
        label->setString(newString.c_str());
        label->limitLabelWidth(125, 0.56, 0.1);

        for (auto& trigger : m_fields->m_triggers) {
            trigger.object->m_easingType = static_cast<EasingType>(index);
        }
    }
    
    CCMenu* createEasingMenu(std::optional<EasingType> initialValue) {
        auto menu = CCMenu::create();
        menu->setContentSize({ 180, 40 });
        auto center = ccp(90, 23);

        auto titleText = "Easing:";
        auto title = CCLabelBMFont::create(titleText, "goldFont.fnt");
        title->setPosition(center + ccp(0, 12));
        title->setScale(0.64);

        auto labelText = initialValue.has_value() ? Trigger::getEasingString(*initialValue) : "Mixed";
        auto label = CCLabelBMFont::create(labelText.c_str(), "bigFont.fnt");
        label->setPosition(center + ccp(0, -12));
        label->limitLabelWidth(125, 0.56, 0.1);
        m_fields->m_easingLabel = label;

        auto decArrowSpr = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
        decArrowSpr->setScale(0.56);
        auto decArrowBtn = CCMenuItemSpriteExtra::create(
            decArrowSpr, this, menu_selector(MultiEditTriggersPopup::onEasingChange)
        );
        decArrowBtn->setPosition(center + ccp(-80, -12));
        decArrowBtn->setTag(-1);

        auto incArrowSpr = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
        incArrowSpr->setScale(0.56);
        incArrowSpr->setFlipX(true);
        auto incArrowBtn = CCMenuItemSpriteExtra::create(
            incArrowSpr, this, menu_selector(MultiEditTriggersPopup::onEasingChange)
        );
        incArrowBtn->setPosition(center + ccp(80, -12));
        incArrowBtn->setTag(1);

        menu->addChild(title);
        menu->addChild(label);
        menu->addChild(decArrowBtn);
        menu->addChild(incArrowBtn);

        menu->setTouchPriority(-504);

        return menu;
    };

    void setMenuToMixed(CCMenu* menu, Trigger::PropType type) {
        auto input = getChildOfType<TextInput>(menu, 0);
        if (input) {
            input->setEnabled(false);
            input->setVisible(false);

            auto spr = CCScale9Sprite::create("square02_001.png", {0, 0, 80, 80});
            spr->setContentSize(input->getBGSprite()->getContentSize());
            spr->setScale(0.4);
            spr->setOpacity(100);

            auto label = CCLabelBMFont::create("Mixed", "bigFont.fnt");
            label->setScale(1.2);
            label->setPosition(spr->getContentSize() / 2);

            spr->addChild(label);

            auto btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(MultiEditTriggersPopup::onMixedInput));
            btn->setPosition(input->getPosition());
            btn->setTag(static_cast<int>(type));

            menu->addChild(btn);
        };

        auto slider = getChildOfType<Slider>(menu, 1);
        if (slider) slider->setValue(100);
    }

    void onMixedInput(CCObject* sender) {
        auto& triggers = m_fields->m_triggers;
        auto type = static_cast<Trigger::PropType>(sender->getTag());

        auto alert = MixedInputPopup::create(triggers, type);

        alert->m_noElasticity = true;
        alert->show();
    }

    template<typename T>
    std::optional<T> getInitialTriggerValue(const std::vector<Trigger>& triggers, Trigger::PropType type) {
        std::variant<std::monostate, int, float, EasingType> firstResult = std::monostate{};

        for (const auto& trigger : triggers) {
            if (trigger.hasProperty(type)) {
                if (std::holds_alternative<std::monostate>(firstResult)) {
                    firstResult = trigger.getProperty(type);
                } else if (firstResult != trigger.getProperty(type)) {
                    return std::nullopt;
                }
            }
        }

        if (std::holds_alternative<std::monostate>(firstResult))
            return std::nullopt;

        return std::get<T>(firstResult);
    }

    bool init(EffectGameObject* p0, CCArray* p1) {
        if (!EditTriggersPopup::init(p0, p1)) return false;

        auto winSize = CCDirector::sharedDirector()->getWinSize();

        // GET TRIGGERS

        auto& triggers = m_fields->m_triggers;
        auto triggerGameObjects = static_cast<CCArrayExt<EffectGameObject*>>(p1);

        std::transform(triggerGameObjects.begin(), triggerGameObjects.end(), std::back_inserter(m_fields->m_triggers), [](EffectGameObject* trigger) {
            return Trigger(trigger);
        });

        auto initialDuration = getInitialTriggerValue<float>(triggers, Trigger::PropType::Duration);
        auto initialOpacity = getInitialTriggerValue<float>(triggers, Trigger::PropType::Opacity);
        auto initialTargetGroupID = getInitialTriggerValue<int>(triggers, Trigger::PropType::TargetGroup);
        auto initialCenterGroupID = getInitialTriggerValue<int>(triggers, Trigger::PropType::CenterGroup);
        auto initialEasingType = getInitialTriggerValue<EasingType>(triggers, Trigger::PropType::Easing);
        auto initialItemID = getInitialTriggerValue<int>(triggers, Trigger::PropType::Item);

        auto hasProperty = [triggers](auto predicate) {
            return std::any_of(triggers.begin(), triggers.end(), predicate);
        };

        bool hasTargetGroup = hasProperty([](const Trigger& trigger){ return trigger.hasTargetGroup; });
        bool hasCenterGroup = hasProperty([](const Trigger& trigger){ return trigger.hasCenterGroup; });
        bool hasItem = hasProperty([](const Trigger& trigger){ return trigger.hasItem; });
        bool hasEasing = hasProperty([](const Trigger& trigger){ return trigger.hasEasing; });
        bool hasDuration = hasProperty([](const Trigger& trigger){ return trigger.hasDuration; });
        bool hasOpacity = hasProperty([](const Trigger& trigger){ return trigger.hasOpacity; });
        
        // ADD MENUS

        auto targetGroupMenu = createGroupMenu(GroupMenuType::TargetGroup, initialTargetGroupID);
        auto centerGroupMenu = createGroupMenu(GroupMenuType::CenterGroup, initialCenterGroupID);
        auto itemMenu = createGroupMenu(GroupMenuType::Item, initialItemID);
        auto easingMenu = createEasingMenu(initialEasingType);
        auto durationMenu = createSliderMenu(SliderMenuType::Duration, initialDuration);
        auto opacityMenu = createSliderMenu(SliderMenuType::Opacity, initialOpacity);

        auto layout = CCMenu::create();
        layout->setLayout(
            RowLayout::create()
                ->setGap(20)
                ->setGrowCrossAxis(true)
                ->setAutoScale(false)
        );

        easingMenu->setLayoutOptions(
            AxisLayoutOptions::create()
                ->setNextGap(30.f)
        );

        if (hasTargetGroup) layout->addChild(targetGroupMenu);
        if (hasCenterGroup) layout->addChild(centerGroupMenu);
        if (hasItem) layout->addChild(itemMenu);
        if (hasEasing) layout->addChild(easingMenu);
        if (hasDuration) layout->addChild(durationMenu);
        if (hasOpacity) layout->addChild(opacityMenu);

        if (!initialTargetGroupID.has_value()) setMenuToMixed(targetGroupMenu, Trigger::PropType::TargetGroup);
        if (!initialCenterGroupID.has_value()) setMenuToMixed(centerGroupMenu, Trigger::PropType::CenterGroup);
        if (!initialItemID.has_value()) setMenuToMixed(itemMenu, Trigger::PropType::Item);
        if (!initialEasingType.has_value()) setMenuToMixed(easingMenu, Trigger::PropType::Easing);
        if (!initialDuration.has_value()) setMenuToMixed(durationMenu, Trigger::PropType::Duration);
        if (!initialOpacity.has_value()) setMenuToMixed(opacityMenu, Trigger::PropType::Opacity);

        auto numberChildren = layout->getChildrenCount();
        CCSize newAlertSize;

        if (numberChildren > 4) {
            newAlertSize = CCSize(440, 310);
        } else if (numberChildren > 2) {
            newAlertSize = CCSize(420, 230);
        } else {
            newAlertSize = CCSize(390, 220); //TODO: change to a better value and make it align column not row
        }

        layout->setContentSize(newAlertSize - CCSize(10, 70));

        layout->setPosition(winSize / 2 + ccp(0, 9));
        layout->updateLayout();
        this->addChild(layout);

        // INCREASE ALERT SIZE

        // old alert size: { 330, 120 }

        auto changeWidth = (newAlertSize.width - 330) / 2;
        auto changeHeight = (newAlertSize.height - 120) / 2;

        auto bg = as<CCScale9Sprite*>(m_mainLayer->getChildren()->objectAtIndex(0));
        auto title = as<CCLabelBMFont*>(getChildren()->objectAtIndex(1));
        auto okBtn = as<CCMenuItemSpriteExtra*>(m_buttonMenu->getChildren()->objectAtIndex(0));
        auto infoBtn = as<InfoAlertButton*>(m_buttonMenu->getChildren()->objectAtIndex(1));

        auto touchLabel = as<CCLabelBMFont*>(m_mainLayer->getChildren()->objectAtIndex(2));
        auto touchBtn = as<CCMenuItemSpriteExtra*>(m_buttonMenu->getChildren()->objectAtIndex(2));
        auto spawnLabel = as<CCLabelBMFont*>(m_mainLayer->getChildren()->objectAtIndex(3));
        auto spawnBtn = as<CCMenuItemSpriteExtra*>(m_buttonMenu->getChildren()->objectAtIndex(3));
        auto multiLabel = as<CCLabelBMFont*>(m_mainLayer->getChildren()->objectAtIndex(4));
        auto multiBtn = as<CCMenuItemSpriteExtra*>(m_buttonMenu->getChildren()->objectAtIndex(4));

        bg->setContentSize(newAlertSize);
        title->setPositionY(title->getPositionY() + changeHeight);
        okBtn->setPositionY(okBtn->getPositionY() - changeHeight);
        infoBtn->setPosition(infoBtn->getPosition() + ccp(-changeWidth, changeHeight));

        touchLabel->setPosition(touchLabel->getPosition() + ccp(-changeWidth, -changeHeight - 30));
        touchBtn->setPosition(touchBtn->getPosition() + ccp(-changeWidth, -changeHeight - 30));
        spawnLabel->setPosition(spawnLabel->getPosition() + ccp(-changeWidth + 90, -changeHeight));
        spawnBtn->setPosition(spawnBtn->getPosition() + ccp(-changeWidth + 90, -changeHeight));
        multiLabel->setPosition(multiLabel->getPosition() + ccp(changeWidth, -changeHeight));
        multiBtn->setPosition(multiBtn->getPosition() + ccp(changeWidth, -changeHeight));

        return true;
    }
};