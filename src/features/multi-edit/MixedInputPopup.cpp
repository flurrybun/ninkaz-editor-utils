#include "MixedInputPopup.hpp"
#include "MixedInputSettingsPopup.hpp"
#include "Trigger.hpp"

#include <Geode/Geode.hpp>
using namespace geode::prelude;

#include <regex>

bool MixedInputPopup::setup(const CCArrayExt<EffectGameObject*>& triggers, const short property, const InputValueType valueType, const std::function<void (std::optional<float>)>& callback) {
    std::vector<float> propertyValues;
    
    for (auto trigger : triggers) {
        if (Trigger::hasProperty(trigger, property)) {
            m_triggers.push_back(trigger);
            propertyValues.push_back(Trigger::getProperty(trigger, property));
        }
    }

    std::sort(m_triggers.begin(), m_triggers.end(), [property](EffectGameObject* a, EffectGameObject* b) {
        float aValue = Trigger::getProperty(a, property);
        float bValue = Trigger::getProperty(b, property);

        return aValue < bValue;
    });

    m_property = property;
    m_callback = callback;
    m_operator = Operator::Equal;
    m_decimalPlaces = Trigger::getPropertyDecimalPlaces(property, valueType);
    m_canBeNegative = valueType != InputValueType::Uint;

    m_modifierValue = 0;
    m_initialValue = 0;
    if (!propertyValues.empty() && std::equal(propertyValues.begin() + 1, propertyValues.end(), propertyValues.begin())) m_initialValue = propertyValues[0];
    m_direction = DirectionType::None;
    m_rounding = RoundingType::Round;
    m_isFirstPage = true;
    
    auto winSize = m_mainLayer->getContentSize();
    m_bgSprite->setZOrder(-10);

    m_closeBtn->removeFromParent();

    // INFO BUTTON

    auto infoText = "Use the <cg>operators</c> to override, add, subtract, multiply, or divide the old values by the <cy>modifier value</c>.\n"
        "When a <cl>direction</c> is selected, the operation is compounded for each subsequent object.\n"
        "The <cp>initial value</c> is used for the first object in the sequence.\n";
    
    auto infoBtn = InfoAlertButton::create("Info", infoText, 0.7f);
    infoBtn->setPosition(winSize - ccp(18, 18));
    m_buttonMenu->addChild(infoBtn);

    createFirstPageRow();
    createScrollLayer(true);

    // SIDE ARROWS

    auto leftArrowSpr = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    leftArrowSpr->setScale(0.5);
    auto leftArrowBtn = CCMenuItemSpriteExtra::create(
        leftArrowSpr, this, menu_selector(MixedInputPopup::onChangePage)
    );
    leftArrowBtn->setPosition({25, 75});
    m_buttonMenu->addChild(leftArrowBtn);

    auto rightArrowSpr = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    rightArrowSpr->setFlipX(true);
    rightArrowSpr->setScale(0.5);
    auto rightArrowBtn = CCMenuItemSpriteExtra::create(
        rightArrowSpr, this, menu_selector(MixedInputPopup::onChangePage)
    );
    rightArrowBtn->setPosition({winSize.width - 25, 75});
    m_buttonMenu->addChild(rightArrowBtn);

    // APPLY, CANCEL, & OPTIONS BUTTONS

    auto buttonLayout = CCMenu::create();
    buttonLayout->setLayout(
        RowLayout::create()
            ->setGap(10.f)
    );
    buttonLayout->setPosition({winSize.width / 2, 24});

    auto cancelSpr = ButtonSprite::create("Cancel", "goldFont.fnt", "GJ_button_01.png", .9f);
    auto cancelBtn = CCMenuItemSpriteExtra::create(
        cancelSpr, this, menu_selector(MixedInputPopup::onClose)
    );

    auto applySpr = ButtonSprite::create("Apply", "goldFont.fnt", "GJ_button_01.png", .9f);
    auto applyBtn = CCMenuItemSpriteExtra::create(
        applySpr, this, menu_selector(MixedInputPopup::onApply)
    );

    buttonLayout->addChild(cancelBtn);
    buttonLayout->addChild(applyBtn);

    auto settingsSpr = CCSprite::createWithSpriteFrameName("GJ_optionsBtn_001.png");
    auto settingsBtn = CCMenuItemSpriteExtra::create(
        settingsSpr, this, menu_selector(MixedInputPopup::onSettings)
    );
    settingsBtn->setLayoutOptions(AxisLayoutOptions::create()
        ->setRelativeScale(0.6f)
    );

    buttonLayout->addChild(settingsBtn);

    m_buttonMenu->addChild(buttonLayout);
    buttonLayout->updateLayout();

    return true;
}

void MixedInputPopup::createFirstPageRow() {
    auto menu = CCMenu::create();
    menu->setPosition({0, 0});
    menu->setContentSize(m_mainLayer->getContentSize());
    m_pageMenu = menu;
    
    auto winSize = m_mainLayer->getContentSize();
    
    auto bottomLayout = CCMenu::create();
    bottomLayout->setLayout(
        RowLayout::create()
            ->setGap(10.f)
    );
    bottomLayout->setPosition({winSize.width / 2, 65});
    bottomLayout->setScale(0.8f);

    // OPERATOR BUTTONS

    const Operator operators[] = {Operator::Equal, Operator::Add, Operator::Subtract, Operator::Multiply, Operator::Divide};

    for (const auto& operatorType : operators) {
        auto selectedBaseSpr = CCScale9Sprite::create("GJ_button_02.png", {0, 0, 40, 40});
        selectedBaseSpr->setContentSize({35, 35});
        selectedBaseSpr->setScale(0.8f);

        auto unselectedBaseSpr = CCScale9Sprite::create("GJ_button_04.png", {0, 0, 40, 40});
        unselectedBaseSpr->setContentSize({35, 35});
        unselectedBaseSpr->setScale(0.8f);

        auto name = operatorType == Operator::Add ? "plus-btn.png"_spr :
            operatorType == Operator::Subtract ? "minus-btn.png"_spr :
            operatorType == Operator::Multiply ? "multiply-btn.png"_spr :
            operatorType == Operator::Divide ? "divide-btn.png"_spr : "equal-btn.png"_spr;

        auto topSpr = CCSprite::createWithSpriteFrameName(name);
        topSpr->setPosition(selectedBaseSpr->getContentSize() / 2);
        topSpr->setScale(0.8f);

        selectedBaseSpr->addChild(topSpr);
        unselectedBaseSpr->addChild(topSpr);

        auto btn = CCMenuItemToggler::create(
            unselectedBaseSpr, selectedBaseSpr, this, menu_selector(MixedInputPopup::onOperator)
        );
        btn->setTag(static_cast<int>(operatorType));
        
        btn->toggle(m_operator == operatorType);
        if (m_operator == operatorType) m_operatorBtn = btn;

        if (operatorType == Operator::Equal && m_direction != DirectionType::None) {
            btn->setEnabled(false);
            
            auto cover = CCLayerColor::create(ccc4(153, 85, 51, 127), 25, 25);
            cover->setZOrder(1);
            cover->setPosition({63, 53});
            menu->addChild(cover);
        }

        bottomLayout->addChild(btn);
    }

    // MODIFIER INPUT

    auto input = TextInput::create(70.f, "Num", "bigFont.fnt");
    input->setFilter("0123456789.-");
    input->getInputNode()->m_numberInput = true;
    input->setString(toTruncatedString(m_modifierValue));
    input->setID("modifier-value-input"_spr);
    input->setCallback([&](const std::string& text) {
        if (text.empty()) {
            m_modifierValue = 0;
            createScrollLayer(false);
            return;
        };
        
        auto isValidFloat = std::regex_match(text, std::regex("^[+-]?([0-9]+([.][0-9]*)?|[.][0-9]+)$"));
        if (isValidFloat) {
            m_modifierValue = std::stof(text);
            createScrollLayer(false);
        }
    });
    input->setLayoutOptions(
        AxisLayoutOptions::create()
            ->setNextGap(10.f)
            ->setPrevGap(10.f)
    );

    auto decArrowSpr = CCSprite::createWithSpriteFrameName("edit_leftBtn_001.png");
    auto decArrowBtn = CCMenuItemSpriteExtra::create(
        decArrowSpr, this, menu_selector(MixedInputPopup::onValueArrow)
    );
    decArrowBtn->setTag(-1);
    decArrowBtn->setLayoutOptions(
        AxisLayoutOptions::create()
            ->setPrevGap(15.f)
    );

    auto incArrowSpr = CCSprite::createWithSpriteFrameName("edit_rightBtn_001.png");
    auto incArrowBtn = CCMenuItemSpriteExtra::create(
        incArrowSpr, this, menu_selector(MixedInputPopup::onValueArrow)
    );
    incArrowBtn->setTag(1);
    incArrowBtn->setLayoutOptions(
        AxisLayoutOptions::create()
            ->setNextGap(15.f)
    );

    bottomLayout->addChild(decArrowBtn);
    bottomLayout->addChild(input);
    bottomLayout->addChild(incArrowBtn);

    menu->addChild(bottomLayout);
    bottomLayout->updateLayout();

    // LABELS

    auto operatorLabel = CCLabelBMFont::create("Operator", "goldFont.fnt");
    operatorLabel->setScale(0.6);
    operatorLabel->setPosition({winSize.width / 2 - 54, 93});
    menu->addChild(operatorLabel);

    auto inputLabel = CCLabelBMFont::create("Modifier Value", "goldFont.fnt");
    inputLabel->setScale(0.6);
    inputLabel->setPosition({winSize.width / 2 + 78, 93});
    menu->addChild(inputLabel);

    m_mainLayer->addChild(menu);
}

void MixedInputPopup::createSecondPageRow() {
    auto menu = CCMenu::create();
    menu->setPosition({0, 0});
    menu->setContentSize(m_mainLayer->getContentSize());
    m_pageMenu = menu;
    
    auto winSize = m_mainLayer->getContentSize();
    
    auto bottomLayout = CCMenu::create();
    bottomLayout->setLayout(
        RowLayout::create()
            ->setGap(10.f)
    );
    bottomLayout->setPosition({winSize.width / 2, 65});
    bottomLayout->setScale(0.8f);

    // DIRECTION BUTTONS
    
    const std::pair<const char*, DirectionType> directionOptions[] = {
        {"None", DirectionType::None},
        {"Up", DirectionType::Up},
        {"Down", DirectionType::Down},
        {"Left", DirectionType::Left},
        {"Right", DirectionType::Right}
    };

    for (const auto& option : directionOptions) {
        const char* labelText = option.first;
        DirectionType directionType = option.second;

        auto selectedBaseSpr = CCScale9Sprite::create("GJ_button_02.png", {0, 0, 40, 40});
        selectedBaseSpr->setContentSize({35, 35});
        selectedBaseSpr->setScale(0.8f);

        auto unselectedBaseSpr = CCScale9Sprite::create("GJ_button_04.png", {0, 0, 40, 40});
        unselectedBaseSpr->setContentSize({35, 35});
        unselectedBaseSpr->setScale(0.8f);

        auto topSpr = CCSprite::createWithSpriteFrameName(directionType != DirectionType::None ? "direction-btn.png"_spr : "no-direction-btn.png"_spr);
        topSpr->setPosition(selectedBaseSpr->getContentSize() / 2);
        topSpr->setScale(0.8f);
        topSpr->setRotation(directionType == DirectionType::Up ? -90 :
            directionType == DirectionType::Down ? 90 :
            directionType == DirectionType::Left ? 180 : 0
        );
        
        selectedBaseSpr->addChild(topSpr);
        unselectedBaseSpr->addChild(topSpr);

        auto btn = CCMenuItemToggler::create(
            unselectedBaseSpr, selectedBaseSpr, this, menu_selector(MixedInputPopup::onDirection)
        );
        btn->setTag(static_cast<int>(directionType));
        
        btn->toggle(m_direction == directionType);
        if (m_direction == directionType) m_directionBtn = btn;

        bottomLayout->addChild(btn);
    }

    // INITIAL VALUE INPUT

    auto input = TextInput::create(70.f, "Num", "bigFont.fnt");
    input->getInputNode()->m_numberInput = true;
    input->setFilter("0123456789.-");
    input->setString(toTruncatedString(m_initialValue));
    input->setID("initial-value-input"_spr);
    input->setCallback([&](const std::string& text) {
        if (text.empty()) {
            m_modifierValue = 0;
            createScrollLayer(false);
            return;
        };
        
        auto isValidFloat = std::regex_match(text, std::regex("^[+-]?([0-9]+([.][0-9]*)?|[.][0-9]+)$"));
        if (isValidFloat) {
            m_initialValue = std::stof(text);
            createScrollLayer(false);
        }
    });
    input->setLayoutOptions(
        AxisLayoutOptions::create()
            ->setNextGap(10.f)
            ->setPrevGap(10.f)
    );

    auto decArrowSpr = CCSprite::createWithSpriteFrameName("edit_leftBtn_001.png");
    auto decArrowBtn = CCMenuItemSpriteExtra::create(
        decArrowSpr, this, menu_selector(MixedInputPopup::onValueArrow)
    );
    decArrowBtn->setTag(-2);
    decArrowBtn->setID("initial-value-dec-arrow"_spr);
    decArrowBtn->setLayoutOptions(
        AxisLayoutOptions::create()
            ->setPrevGap(15.f)
    );

    auto incArrowSpr = CCSprite::createWithSpriteFrameName("edit_rightBtn_001.png");
    auto incArrowBtn = CCMenuItemSpriteExtra::create(
        incArrowSpr, this, menu_selector(MixedInputPopup::onValueArrow)
    );
    incArrowBtn->setTag(2);
    incArrowBtn->setID("initial-value-inc-arrow"_spr);
    incArrowBtn->setLayoutOptions(
        AxisLayoutOptions::create()
            ->setNextGap(15.f)
    );

    bottomLayout->addChild(decArrowBtn);
    bottomLayout->addChild(input);
    bottomLayout->addChild(incArrowBtn);

    input->getInputNode()->setTouchEnabled(m_direction != DirectionType::None);
    decArrowBtn->setEnabled(m_direction != DirectionType::None);
    incArrowBtn->setEnabled(m_direction != DirectionType::None);

    menu->addChild(bottomLayout);
    bottomLayout->updateLayout();

    auto inputCover = CCLayerColor::create(ccc4(153, 85, 51, 127), 100, 35);
    inputCover->setPosition({218, 47});
    inputCover->setID("initial-value-cover"_spr);
    inputCover->setVisible(m_direction == DirectionType::None);
    menu->addChild(inputCover);

    // LABELS

    auto operatorLabel = CCLabelBMFont::create("Direction", "goldFont.fnt");
    operatorLabel->setScale(0.6);
    operatorLabel->setPosition({winSize.width / 2 - 54, 93});
    menu->addChild(operatorLabel);

    auto inputLabel = CCLabelBMFont::create("Initial Value", "goldFont.fnt");
    inputLabel->setScale(0.6);
    inputLabel->setPosition({winSize.width / 2 + 78, 93});
    menu->addChild(inputLabel);

    m_mainLayer->addChild(menu);
}

void MixedInputPopup::createScrollLayer(bool isInit) {
    auto winSize = m_mainLayer->getContentSize();
    auto scrollPosition = winSize / 2 + ccp(0, 46);
    auto scrollSize = CCSize(300, 150);
    auto rowSize = CCSize(scrollSize.width, 37);

    if (isInit) {
        auto border = ListBorders::create();
        border->setContentSize(scrollSize + CCSize(3, 5));
        border->setPosition(scrollPosition);
        border->setZOrder(9);
        m_mainLayer->addChild(border);

        auto bg = CCLayerColor::create();
        bg->setColor(ccc3(191, 114, 62));
        bg->setOpacity(255);
        bg->setContentSize(scrollSize);
        bg->setPosition(scrollPosition);
        bg->ignoreAnchorPointForPosition(false);
        bg->setZOrder(-1);
        m_mainLayer->addChild(bg);
    } else {
        m_list->removeFromParent();
    }

    auto stringMap = createStringMap();

    int maxTriggerCount = 0;
    std::set<int> uniqueTriggerIDs;

    for (const auto& calcInfo : stringMap) {
        auto triggers = calcInfo.triggers;
        
        uniqueTriggerIDs.clear();
        for (const auto& trigger : triggers) {
            uniqueTriggerIDs.insert(trigger->m_objectID);
        }

        int triggerCount = uniqueTriggerIDs.size();
        if (triggerCount > maxTriggerCount) maxTriggerCount = triggerCount;
    }

    CCArrayExt<CCMenu*> rows;
    size_t index = 0;

    for (const auto& calcInfo : stringMap) {
        auto [oldString, changeString, newString, triggers] = calcInfo;
        
        auto rowMenu = CCMenu::create();
        rowMenu->setContentSize(rowSize);

        // TRIGGER COUNT LAYOUT

        auto triggerLayout = CCMenu::create();
        triggerLayout->setLayout(
            RowLayout::create()
                ->setAxisAlignment(AxisAlignment::Start)
                ->setDefaultScaleLimits(0, 1)
        );
        triggerLayout->setPosition({57, rowSize.height / 2});
        triggerLayout->setContentWidth(112);
        triggerLayout->setScale(0.7);

        std::map<int, std::pair<CCSpriteFrame*, int>> triggerCounts;
        for (const auto& trigger : triggers) {
            auto count = triggerCounts[trigger->m_objectID].second;
            triggerCounts[trigger->m_objectID] = {trigger->displayFrame(), count + 1};
        }

        for (const auto& [id, pair] : triggerCounts) {
            auto [frameName, count] = pair;

            auto spr = CCSprite::createWithSpriteFrame(frameName);

            if (count > 1) {
                auto text = std::to_string(count) + "x";

                auto label = CCLabelBMFont::create(text.c_str(), "bigFont.fnt");
                label->setPosition({spr->getContentWidth() / 2 + 8, -2});
                label->setScale(0.6f);
                label->setAnchorPoint({0.5, 0});

                spr->addChild(label);
            }
            triggerLayout->addChild(spr);
        }

        rowMenu->addChild(triggerLayout);
        triggerLayout->updateLayout();

        // CALCULATION LAYOUT

        auto calcLayout = CCMenu::create();
        calcLayout->setLayout(
            RowLayout::create()
                ->setAxisAlignment(AxisAlignment::Between)
                ->setDefaultScaleLimits(0, 1)
        );
        calcLayout->setPosition({rowSize.width - 22, rowSize.height / 2});
        calcLayout->setAnchorPoint({1, 0.5});
        calcLayout->setScale(0.7f);

        if (maxTriggerCount == 1) calcLayout->setContentWidth(320);
        else if (maxTriggerCount == 2) calcLayout->setContentWidth(283);
        else calcLayout->setContentWidth(247);

        auto oldLabel = CCLabelBMFont::create(oldString.c_str(), "bigFont.fnt");
        auto changeLabel = CCLabelBMFont::create(changeString.c_str(), "bigFont.fnt");
        auto newLabel = CCLabelBMFont::create(newString.c_str(), "bigFont.fnt");

        auto signOptions = AxisLayoutOptions::create()
            ->setRelativeScale(0.6f);

        auto signName = m_operator == Operator::Add ? "plus-bigFont.png"_spr :
            m_operator == Operator::Subtract ? "minus-bigFont.png"_spr :
            m_operator == Operator::Multiply ? "multiply-bigFont.png"_spr :
            m_operator == Operator::Divide ? "divide-bigFont.png"_spr : "equal-bigFont.png"_spr;

        auto sign = CCSprite::createWithSpriteFrameName(signName);

        sign->setLayoutOptions(signOptions);
        auto equalSign = CCSprite::createWithSpriteFrameName("equal-bigFont.png"_spr);
        equalSign->setLayoutOptions(signOptions);

        calcLayout->addChild(oldLabel);
        calcLayout->addChild(sign);
        calcLayout->addChild(changeLabel);
        calcLayout->addChild(equalSign);
        calcLayout->addChild(newLabel);

        if (m_operator == Operator::Equal || (m_direction != DirectionType::None && index == 0)) {
            sign->setVisible(false);
            changeLabel->setVisible(false);
        }

        rowMenu->addChild(calcLayout);
        calcLayout->updateLayout();

        // visually align text with operators
        oldLabel->setPositionY(oldLabel->getPositionY() + 2);
        changeLabel->setPositionY(changeLabel->getPositionY() + 2);
        newLabel->setPositionY(newLabel->getPositionY() + 2);

        // BOTTOM BORDER

        rows.push_back(rowMenu);
        index++;
    }

    auto list = ListView::create(rows.inner(), rowSize.height, scrollSize.width, scrollSize.height);
    list->setPrimaryCellColor(ccc3(144, 79, 39));
    list->setSecondaryCellColor(ccc3(156, 85, 42));

    list->setPosition(scrollPosition);
    list->ignoreAnchorPointForPosition(false);
    
    m_mainLayer->addChild(list);
    m_list = list;
}

void MixedInputPopup::onOperator(CCObject* sender) {
    if (m_operatorBtn) m_operatorBtn->toggle(false);

    auto btn = static_cast<CCMenuItemToggler*>(sender);
    m_operator = static_cast<Operator>(btn->getTag());
    m_operatorBtn = btn;

    createScrollLayer(false);
}

void MixedInputPopup::onDirection(CCObject* sender) {
    if (m_directionBtn) m_directionBtn->toggle(false);
    auto oldDirection = m_direction;

    auto btn = static_cast<CCMenuItemToggler*>(sender);
    m_direction = static_cast<DirectionType>(btn->getTag());
    m_directionBtn = btn;

    auto initialValueInput = static_cast<TextInput*>(m_pageMenu->getChildByIDRecursive("initial-value-input"_spr));

    if (m_operator == Operator::Equal) m_operator = Operator::Add;
    if (m_modifierValue == 0) m_modifierValue = 1;

    auto decArrowBtn = static_cast<CCMenuItemSpriteExtra*>(m_pageMenu->getChildByIDRecursive("initial-value-dec-arrow"_spr));
    auto incArrowBtn = static_cast<CCMenuItemSpriteExtra*>(m_pageMenu->getChildByIDRecursive("initial-value-inc-arrow"_spr));
    auto cover = m_pageMenu->getChildByIDRecursive("initial-value-cover"_spr);

    if (initialValueInput) initialValueInput->getInputNode()->setTouchEnabled(m_direction != DirectionType::None);
    if (decArrowBtn) decArrowBtn->setEnabled(m_direction != DirectionType::None);
    if (incArrowBtn) incArrowBtn->setEnabled(m_direction != DirectionType::None);
    if (cover) cover->setVisible(m_direction == DirectionType::None);

    std::sort(m_triggers.begin(), m_triggers.end(), [&](const EffectGameObject* a, const EffectGameObject* b) {
        if (m_direction == DirectionType::Left) return a->m_positionX > b->m_positionX;
        else if (m_direction == DirectionType::Right) return a->m_positionX < b->m_positionX;
        else if (m_direction == DirectionType::Up) return a->m_positionY < b->m_positionY;
        else if (m_direction == DirectionType::Down) return a->m_positionY > b->m_positionY;

        return false;
    });

    createScrollLayer(false);
}

void MixedInputPopup::onValueArrow(CCObject* sender) {
    auto btn = static_cast<CCMenuItemSpriteExtra*>(sender);
    auto tag = btn->getTag();

    if (tag == -1) m_modifierValue--;
    else if (tag == 1) m_modifierValue++;
    else if (tag == -2) m_initialValue--;
    else if (tag == 2) m_initialValue++;

    auto modifierValueInput = static_cast<TextInput*>(m_pageMenu->getChildByIDRecursive("modifier-value-input"_spr));
    auto initialValueInput = static_cast<TextInput*>(m_pageMenu->getChildByIDRecursive("initial-value-input"_spr));

    if (modifierValueInput) modifierValueInput->setString(toTruncatedString(m_modifierValue));
    if (initialValueInput) initialValueInput->setString(toTruncatedString(m_initialValue));

    createScrollLayer(false);
}

void MixedInputPopup::onChangePage(CCObject* sender) {
    m_isFirstPage = !m_isFirstPage;

    m_mainLayer->removeChild(m_pageMenu);
    if (m_isFirstPage) createFirstPageRow();
    else createSecondPageRow();
}

void MixedInputPopup::onSettings(CCObject* sender) {
    auto settings = MixedInputSettings{m_rounding};
    
    auto alert = MixedInputSettingsPopup::create(settings, [this](MixedInputSettings settings) {
        m_rounding = settings.rounding;
        createScrollLayer(false);
    });

    alert->m_noElasticity = true;
    alert->show();
}

void MixedInputPopup::onApply(CCObject* sender) {
    std::vector<float> newProperties;
    
    if (m_direction == DirectionType::None) {
        for (auto& trigger : m_triggers) {
            auto property = Trigger::getProperty(trigger, m_property);
            auto newProperty = applyOperation(property, m_modifierValue, m_operator);

            newProperties.push_back(newProperty);
            Trigger::setProperty(trigger, m_property, newProperty);
        }
    } else {
        size_t count = 0;

        for (auto& trigger : m_triggers) {
            float newProperty;

            if (m_operator == Operator::Multiply || m_operator == Operator::Divide) {
                newProperty = applyOperation(m_initialValue, std::pow(m_modifierValue, count), m_operator, false);
            } else {
                newProperty = applyOperation(m_initialValue, m_modifierValue * count, m_operator, false);
            }

            newProperties.push_back(newProperty);
            Trigger::setProperty(trigger, m_property, newProperty);

            count++;
        }
    }

    if (m_callback) {
        if (std::equal(newProperties.begin() + 1, newProperties.end(), newProperties.begin())) {
            m_callback(newProperties.front());
        } else {
            m_callback(std::nullopt);
        }
    }
    
    onClose(sender);
}

float MixedInputPopup::applyOperation(float value, float modifier, Operator op, bool shouldRound) {
    float result;

    switch (op) {
        case Operator::Add: result = value + modifier; break;
        case Operator::Subtract: result = value - modifier; break;
        case Operator::Multiply: result = value * modifier; break;
        case Operator::Divide:
            if (modifier == 0) return 0;
            result = value / modifier;
            break;
        default: result = modifier;
    }

    if (!m_canBeNegative && result < 0) return 0;
    if (shouldRound) return roundValue(result);
    return result;
}

std::string MixedInputPopup::toTruncatedString(float value, std::optional<int> decimalPlaces) {
    if (decimalPlaces.has_value()) {
        auto factor = std::pow(10, decimalPlaces.value());
        value = std::round(value * factor) / factor;

        std::ostringstream out;
        out << std::fixed << std::setprecision(decimalPlaces.value()) << value;
        return out.str();
    }
    
    auto str = std::to_string(value);
    str.erase(str.find_last_not_of('0') + 1, std::string::npos);
    str.erase(str.find_last_not_of('.') + 1, std::string::npos);

    return str;
}

float MixedInputPopup::roundValue(float value) {
    auto factor = std::pow(10, m_decimalPlaces);

    if (m_rounding == RoundingType::Round) {
        return std::round(value * factor) / factor;
    } else if (m_rounding == RoundingType::Floor) {
        return std::floor(value * factor) / factor;
    } else { // RoundingType::Ceiling
        return std::ceil(value * factor) / factor;
    }
}

// this is a mess of a function but it essentially creates a giant vector of CalculationInfo structs
// which are used to display the values in the scroll layer

std::vector<MixedInputPopup::CalculationInfo> MixedInputPopup::createStringMap() {
    std::vector<CalculationInfo> calcVector;

    size_t index = 0;
    float previousNewProperty = m_initialValue;

    auto hasDirection = m_direction != DirectionType::None;

    for (const auto& trigger : m_triggers) {
        float property;
        float change;
        float newProperty;

        if (hasDirection) {
            property = roundValue(previousNewProperty);
            change = m_modifierValue;
            newProperty = index != 0 ? applyOperation(property, change, m_operator) : property;
        } else {
            property = Trigger::getProperty(trigger, m_property);
            change = m_modifierValue;
            newProperty = applyOperation(property, change, m_operator);
        }

        auto propertyString = toTruncatedString(property);
        auto changeString = m_operator != Operator::Equal ? toTruncatedString(change) : "0";
        auto newPropertyString = toTruncatedString(newProperty, m_decimalPlaces);

        CalculationInfo calcInfo(propertyString, changeString, newPropertyString, CCArray::createWithObject(trigger));

        if (!hasDirection) {
            // group triggers with the same property string
            auto it = std::find_if(calcVector.begin(), calcVector.end(), [&](const CalculationInfo& ci) {
                return ci.propertyString == calcInfo.propertyString;
            });

            if (it == calcVector.end()) calcVector.push_back(calcInfo);
            else it->triggers.push_back(trigger);
        } else {
            // don't group triggers
            calcVector.push_back(calcInfo);
        }

        index++;
        previousNewProperty = newProperty;
    }

    return calcVector;
}

MixedInputPopup* MixedInputPopup::create(const CCArrayExt<EffectGameObject*>& triggers, const short property, const InputValueType valueType, const std::function<void (std::optional<float>)>& callback) {
    auto ret = new MixedInputPopup();
    if (ret && ret->initAnchored(380.f, 280.f, triggers, property, valueType, callback)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}