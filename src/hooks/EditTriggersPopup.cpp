#include "../additions/Trigger.hpp"
#include "../additions/MixedInputPopup.hpp"
#include <Geode/modify/EditTriggersPopup.hpp>
#include <Geode/ui/TextInput.hpp>

#include <Geode/Geode.hpp>
using namespace geode::prelude;

class $modify(MultiEditTriggersPopup, EditTriggersPopup) {
    struct Fields {
        CCArrayExt<EffectGameObject*> m_triggers;
        CCLabelBMFont* m_easingLabel;
    };

    enum GroupMenuType {
        TargetGroup, CenterGroup, Item
    };

    enum SliderMenuType {
        Duration, Opacity
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
                auto hasTargetGroup = Trigger::hasProperty(trigger, Trigger::TARGET_GROUP);
                auto hasCenterGroup = Trigger::hasProperty(trigger, Trigger::CENTER_GROUP);
                auto hasItem = Trigger::hasProperty(trigger, Trigger::ITEM);

                if (type == GroupMenuType::TargetGroup && hasTargetGroup) {
                    Trigger::setProperty(trigger, Trigger::TARGET_GROUP, value);
                } else if (type == GroupMenuType::CenterGroup && hasCenterGroup) {
                    Trigger::setProperty(trigger, Trigger::CENTER_GROUP, value);
                } else if (type == GroupMenuType::Item && hasItem) {
                    Trigger::setProperty(trigger, Trigger::ITEM, value);
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
        slider->getThumb()->setEnabled(initialValue.has_value());
        slider->getThumb()->setVisible(initialValue.has_value());

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
            auto hasDuration = Trigger::hasProperty(trigger, Trigger::DURATION);
            auto hasOpacity = Trigger::hasProperty(trigger, Trigger::OPACITY);

                if (type == SliderMenuType::Duration && hasDuration) {
                    Trigger::setProperty(trigger, Trigger::DURATION, value);
                } else if (type == SliderMenuType::Opacity && hasOpacity) {
                    Trigger::setProperty(trigger, Trigger::OPACITY, value);
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
            Trigger::setProperty(trigger, Trigger::EASING, index);
        }
    }
    
    CCMenu* createEasingMenu(std::optional<int> initialValue) {
        auto menu = CCMenu::create();
        menu->setContentSize({ 180, 40 });
        auto center = ccp(90, 23);

        auto titleText = "Easing:";
        auto title = CCLabelBMFont::create(titleText, "goldFont.fnt");
        title->setPosition(center + ccp(0, 12));
        title->setScale(0.64);

        auto labelText = initialValue.has_value() ? Trigger::getEasingString(static_cast<EasingType>(*initialValue)) : "Mixed";
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

    void setMenuToMixed(CCMenu* menu, short property) {
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
            btn->setTag(property);

            menu->addChild(btn);
        };

        auto slider = getChildOfType<Slider>(menu, 1);
        if (slider) slider->setValue(100);
    }

    void onMixedInput(CCObject* sender) {
        auto& triggers = m_fields->m_triggers;
        auto property = static_cast<short>(sender->getTag());

        auto alert = MixedInputPopup::create(triggers, property, nullptr);

        alert->m_noElasticity = true;
        alert->show();
    }

    std::optional<float> getInitialTriggerValue(const CCArrayExt<EffectGameObject*>& triggers, const short property) {
        std::optional<float> firstResult = std::nullopt;

        for (const auto& trigger : triggers) {
            if (Trigger::hasProperty(trigger, property)) {
                if (firstResult == std::nullopt) {
                    firstResult = Trigger::getProperty(trigger, property);
                } else if (firstResult != Trigger::getProperty(trigger, property)) {
                    return std::nullopt;
                }
            }
        }

        return firstResult;
    }

    bool init(EffectGameObject* p0, CCArray* p1) {
        if (!EditTriggersPopup::init(p0, p1)) return false;

        auto winSize = CCDirector::sharedDirector()->getWinSize();

        // GET TRIGGERS

        CCArrayExt<EffectGameObject*> triggers = p1;
        for (auto* trigger : triggers) m_fields->m_triggers.push_back(trigger);

        auto initialDuration = getInitialTriggerValue(triggers, Trigger::DURATION);
        auto initialOpacity = getInitialTriggerValue(triggers, Trigger::OPACITY);
        auto initialTargetGroup = getInitialTriggerValue(triggers, Trigger::TARGET_GROUP);
        auto initialCenterGroup = getInitialTriggerValue(triggers, Trigger::CENTER_GROUP);
        auto initialEasingType = getInitialTriggerValue(triggers, Trigger::EASING);
        auto initialItem = getInitialTriggerValue(triggers, Trigger::ITEM);

        auto hasProperty = [&](short property) {
            for (const auto& trigger : triggers)
                if (Trigger::hasProperty(trigger, property)) return true;

            return false;
        };
        
        bool hasTargetGroup = hasProperty(Trigger::TARGET_GROUP);
        bool hasCenterGroup = hasProperty(Trigger::CENTER_GROUP);
        bool hasItem = hasProperty(Trigger::ITEM);
        bool hasEasing = hasProperty(Trigger::EASING);
        bool hasDuration = hasProperty(Trigger::DURATION);
        bool hasOpacity = hasProperty(Trigger::OPACITY);
        
        // ADD MENUS

        auto targetGroupMenu = createGroupMenu(GroupMenuType::TargetGroup, initialTargetGroup);
        auto centerGroupMenu = createGroupMenu(GroupMenuType::CenterGroup, initialCenterGroup);
        auto itemMenu = createGroupMenu(GroupMenuType::Item, initialItem);
        auto easingMenu = createEasingMenu(static_cast<std::optional<int>>(initialEasingType));
        auto durationMenu = createSliderMenu(SliderMenuType::Duration, initialDuration);
        auto opacityMenu = createSliderMenu(SliderMenuType::Opacity, initialOpacity);

        auto menu = CCMenu::create();
        auto menuLayout = RowLayout::create()
            ->setGap(20)
            ->setGrowCrossAxis(true)
            ->setAutoScale(false);

        easingMenu->setLayoutOptions(
            AxisLayoutOptions::create()
                ->setNextGap(30.f)
        );

        if (hasTargetGroup) menu->addChild(targetGroupMenu);
        if (hasCenterGroup) menu->addChild(centerGroupMenu);
        if (hasItem) menu->addChild(itemMenu);
        if (hasEasing) menu->addChild(easingMenu);
        if (hasDuration) menu->addChild(durationMenu);
        if (hasOpacity) menu->addChild(opacityMenu);

        if (!initialTargetGroup.has_value()) setMenuToMixed(targetGroupMenu, Trigger::TARGET_GROUP);
        if (!initialCenterGroup.has_value()) setMenuToMixed(centerGroupMenu, Trigger::CENTER_GROUP);
        if (!initialItem.has_value()) setMenuToMixed(itemMenu, Trigger::ITEM);
        if (!initialEasingType.has_value()) setMenuToMixed(easingMenu, Trigger::EASING);
        if (!initialDuration.has_value()) setMenuToMixed(durationMenu, Trigger::DURATION);
        if (!initialOpacity.has_value()) setMenuToMixed(opacityMenu, Trigger::OPACITY);

        auto numberChildren = menu->getChildrenCount();
        CCSize newAlertSize;

        if (numberChildren > 4) newAlertSize = CCSize(440, 310);
        else if (numberChildren > 2) newAlertSize = CCSize(420, 230);
        else {
            newAlertSize = CCSize(390, 220);
            menuLayout = menuLayout->setAxis(Axis::Column);
        }

        menu->setContentSize(newAlertSize - CCSize(10, 70));

        menu->setPosition(winSize / 2 + ccp(0, 9));
        menu->setLayout(menuLayout);
        menu->updateLayout();
        this->addChild(menu);

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