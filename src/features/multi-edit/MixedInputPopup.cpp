#include "MixedInputPopup.hpp"
#include "MixedInputSettingsPopup.hpp"
#include "MultiEditContext.hpp"
#include "../../misc/StringUtils.hpp"

#include <Geode/Geode.hpp>
using namespace geode::prelude;

#include <regex>

bool MixedInputPopup::setup(MultiEditContext* context, int property) {
    if (!context) return false;
    
    m_context = context;
    m_property = property;
    
    std::vector<float> propertyValues;
    
    for (auto object : context->getGameObjects()) {
        if (context->hasProperty(object, property)) {
            m_objects.push_back(object);
            propertyValues.push_back(context->getProperty(object, property));
        }
    }

    std::sort(m_objects.begin(), m_objects.end(), [context, property](GameObject* a, GameObject* b) {
        float aValue = context->getProperty(a, property);
        float bValue = context->getProperty(b, property);

        return aValue < bValue;
    });

    m_operator = Operator::Equal;
    m_decimalPlaces = context->getPropertyDecimalPlaces(property);
    m_propertyBounds = context->getPropertyBounds(property);

    m_modifierValue = 0;
    m_initialValue = 0;
    if (!propertyValues.empty() && std::equal(propertyValues.begin() + 1, propertyValues.end(), propertyValues.begin())) m_initialValue = propertyValues[0];
    m_direction = DirectionType::None;
    m_rounding = RoundingType::Round;
    
    auto winSize = m_mainLayer->getContentSize();
    m_bgSprite->setZOrder(-10);

    m_closeBtn->removeFromParent();

    // INFO BUTTON

    auto infoText = "Use the <cg>operators</c> to override, add, subtract, multiply, or divide the old values by the <cy>modifier value</c>.\n"
        "When a <cl>direction</c> is selected, the operation is compounded for each subsequent trigger, ordered by their x/y position.\n"
        "The <cp>initial value</c> is the first value in the sequence.\n";
    
    auto infoBtn = InfoAlertButton::create("Info", infoText, 0.7f);
    infoBtn->setPosition(winSize - ccp(18, 18));
    m_buttonMenu->addChild(infoBtn);

    auto topRow = createTopRow();
    auto bottomRow = createBottomRow();
    m_mainLayer->addChildAtPosition(topRow, Anchor::Center, {0, -51});
    m_mainLayer->addChildAtPosition(bottomRow, Anchor::Center, {0, -80});

    createScrollLayer(true);

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

CCMenu* MixedInputPopup::createTopRow() {
    auto menu = CCMenu::create();
    menu->setLayout(
        RowLayout::create()
            ->setGap(10.f)
    );
    menu->setScale(0.8f);
    m_topRow = menu;

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

        menu->addChild(btn);
    }

    // MODIFIER INPUT

    auto input = TextInput::create(70.f, "Num", "bigFont.fnt");
    input->setFilter("0123456789.-");
    input->getInputNode()->m_numberInput = true;
    input->setString(nk::toString(m_modifierValue));
    input->setID("modifier-value-input"_spr);
    input->setCallback([&](const std::string& text) {
        m_modifierValue = text.empty() ? 0 : nk::toFloat(text);
        createScrollLayer(false);
    });
    input->setLayoutOptions(
        AxisLayoutOptions::create()
            ->setNextGap(10.f)
            ->setPrevGap(10.f)
    );
    m_modifierInput = input;

    auto decArrowSpr = CCSprite::createWithSpriteFrameName("edit_leftBtn_001.png");
    auto decArrowBtn = CCMenuItemSpriteExtra::create(
        decArrowSpr, this, menu_selector(MixedInputPopup::onValueArrow)
    );
    decArrowBtn->setTag(-1);

    auto incArrowSpr = CCSprite::createWithSpriteFrameName("edit_rightBtn_001.png");
    auto incArrowBtn = CCMenuItemSpriteExtra::create(
        incArrowSpr, this, menu_selector(MixedInputPopup::onValueArrow)
    );
    incArrowBtn->setTag(1);

    auto label = CCLabelBMFont::create("Modifier:", "goldFont.fnt");
    label->setAnchorPoint({1, 0.5});
    label->setLayoutOptions(AxisLayoutOptions::create()
        ->setLength(60)
        ->setRelativeScale(0.6)
        ->setPrevGap(30)
        ->setNextGap(10)
    );

    menu->addChild(label);
    menu->addChild(decArrowBtn);
    menu->addChild(input);
    menu->addChild(incArrowBtn);
    menu->updateLayout();
    
    auto equalBtnCover = CCLayerColor::create(ccc4(153, 85, 51, 127), 29, 29);
    equalBtnCover->setPosition(menu->getChildByType<CCMenuItemToggler>(0)->getPosition());
    equalBtnCover->ignoreAnchorPointForPosition(false);
    equalBtnCover->setVisible(false);
    menu->addChild(equalBtnCover);
    m_equalBtnCover = equalBtnCover;

    return menu;
}

CCMenu* MixedInputPopup::createBottomRow() {
    auto menu = CCMenu::create();
    menu->setLayout(
        RowLayout::create()
            ->setGap(10.f)
    );
    menu->setScale(0.8f);
    m_bottomRow = menu;

    // DIRECTION BUTTONS

    const DirectionType directions[] = {DirectionType::None, DirectionType::Up, DirectionType::Down, DirectionType::Left, DirectionType::Right};

    for (const auto& directionType : directions) {

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

        menu->addChild(btn);
    }

    // INITIAL VALUE INPUT

    auto input = TextInput::create(70.f, "Num", "bigFont.fnt");
    input->getInputNode()->m_numberInput = true;
    input->setFilter("0123456789.-");
    input->setString(nk::toString(m_initialValue));
    input->setID("initial-value-input"_spr);
    input->setCallback([&](const std::string& text) {
        m_initialValue = text.empty() ? 0 : nk::toFloat(text);
        createScrollLayer(false);
    });
    input->setLayoutOptions(
        AxisLayoutOptions::create()
            ->setNextGap(10.f)
            ->setPrevGap(10.f)
    );
    m_initialInput = input;

    auto decArrowSpr = CCSprite::createWithSpriteFrameName("edit_leftBtn_001.png");
    auto decArrowBtn = CCMenuItemSpriteExtra::create(
        decArrowSpr, this, menu_selector(MixedInputPopup::onValueArrow)
    );
    decArrowBtn->setTag(-2);
    decArrowBtn->setID("initial-value-dec-arrow"_spr);

    auto incArrowSpr = CCSprite::createWithSpriteFrameName("edit_rightBtn_001.png");
    auto incArrowBtn = CCMenuItemSpriteExtra::create(
        incArrowSpr, this, menu_selector(MixedInputPopup::onValueArrow)
    );
    incArrowBtn->setTag(2);
    incArrowBtn->setID("initial-value-inc-arrow"_spr);

    input->getInputNode()->setTouchEnabled(m_direction != DirectionType::None);
    decArrowBtn->setEnabled(m_direction != DirectionType::None);
    incArrowBtn->setEnabled(m_direction != DirectionType::None);

    auto label = CCLabelBMFont::create("Initial:", "goldFont.fnt");
    label->setAnchorPoint({1, 0.5});
    label->setLayoutOptions(AxisLayoutOptions::create()
        ->setLength(60)
        ->setRelativeScale(0.6)
        ->setPrevGap(30)
        ->setNextGap(10)
    );

    menu->addChild(label);
    menu->addChild(decArrowBtn);
    menu->addChild(input);
    menu->addChild(incArrowBtn);
    menu->updateLayout();

    auto inputCover = CCLayerColor::create(ccc4(153, 85, 51, 127), 127, 35);
    inputCover->setPosition(input->getPosition());
    inputCover->ignoreAnchorPointForPosition(false);
    inputCover->setVisible(m_direction == DirectionType::None);
    menu->addChild(inputCover);
    m_initialInputCover = inputCover;

    return menu;
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
        auto objects = calcInfo.objects;
        
        uniqueTriggerIDs.clear();
        for (const auto& trigger : objects) {
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

    auto btn = static_cast<CCMenuItemToggler*>(sender);
    m_direction = static_cast<DirectionType>(btn->getTag());
    m_directionBtn = btn;

    // UX stuff

    if (m_operator == Operator::Equal) m_topRow->getChildByType<CCMenuItemToggler>(1)->toggleWithCallback(true);
    m_equalBtnCover->setVisible(m_direction != DirectionType::None);
    m_topRow->getChildByType<CCMenuItemToggler>(0)->setEnabled(m_direction == DirectionType::None);

    if (m_modifierValue == 0) m_modifierInput->setString("1", true);

    // set if the initial value input is enabled

    m_initialInput->getInputNode()->setTouchEnabled(m_direction != DirectionType::None);
    m_initialInputCover->setVisible(m_direction == DirectionType::None);
    m_bottomRow->getChildByType<CCMenuItemSpriteExtra>(0)->setEnabled(m_direction != DirectionType::None);
    m_bottomRow->getChildByType<CCMenuItemSpriteExtra>(1)->setEnabled(m_direction != DirectionType::None);

    // sort triggers by direction

    std::sort(m_objects.begin(), m_objects.end(), [&](const GameObject* a, const GameObject* b) {
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

    if (tag == -1 || tag == 1) {
        m_modifierValue += tag;
        m_modifierInput->setString(nk::toString(m_modifierValue));
    } else if (tag == -2 || tag == 2) {
        m_initialValue += tag / 2;
        m_initialInput->setString(nk::toString(m_initialValue));
    }

    createScrollLayer(false);
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
    if (!m_context) {
        onClose(sender);
        return;
    }

    std::vector<float> newProperties;
    
    if (m_direction == DirectionType::None) {
        for (auto& object : m_objects) {
            auto property = m_context->getProperty(object, m_property);
            auto newProperty = applyOperation(property, m_modifierValue, m_operator);

            newProperties.push_back(newProperty);
            m_context->setProperty(object, m_property, newProperty);
        }
    } else {
        size_t count = 0;

        for (auto& trigger : m_objects) {
            float newProperty;

            if (m_operator == Operator::Multiply || m_operator == Operator::Divide) {
                newProperty = applyOperation(m_initialValue, std::pow(m_modifierValue, count), m_operator, false);
            } else {
                newProperty = applyOperation(m_initialValue, m_modifierValue * count, m_operator, false);
            }

            newProperties.push_back(newProperty);
            m_context->setProperty(trigger, m_property, newProperty);

            count++;
        }
    }

    if (newProperties.size() > 1 && std::equal(newProperties.begin() + 1, newProperties.end(), newProperties.begin())) {
        m_context->onMixedInputApplied(m_property, newProperties.front());
    } else if (newProperties.size() == 1) {
        m_context->onMixedInputApplied(m_property, newProperties.front());
    } else {
        m_context->onMixedInputApplied(m_property, std::nullopt);
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

    result = std::clamp(result, m_propertyBounds.min, m_propertyBounds.max);
    if (shouldRound) return roundValue(result);
    return result;
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

    for (const auto& object : m_objects) {
        float property;
        float change;
        float newProperty;

        if (hasDirection) {
            property = roundValue(previousNewProperty);
            change = m_modifierValue;
            newProperty = index != 0 ? applyOperation(property, change, m_operator) : property;
        } else {
            property = m_context->getProperty(object, m_property);
            change = m_modifierValue;
            newProperty = applyOperation(property, change, m_operator);
        }

        auto propertyString = nk::toString(property, m_decimalPlaces);
        auto changeString = m_operator != Operator::Equal ? nk::toString(change, 3) : "0";
        auto newPropertyString = nk::toString(newProperty, m_decimalPlaces, false);

        CalculationInfo calcInfo(propertyString, changeString, newPropertyString, CCArray::createWithObject(object));

        if (!hasDirection) {
            // group triggers with the same property string
            auto it = std::find_if(calcVector.begin(), calcVector.end(), [&](const CalculationInfo& ci) {
                return ci.propertyString == calcInfo.propertyString;
            });

            if (it == calcVector.end()) calcVector.push_back(calcInfo);
            else it->objects.push_back(object);
        } else {
            // don't group triggers
            calcVector.push_back(calcInfo);
        }

        index++;
        previousNewProperty = newProperty;
    }

    return calcVector;
}

MixedInputPopup* MixedInputPopup::create(MultiEditContext* context, int property) {
    auto ret = new MixedInputPopup();
    if (ret && ret->initAnchored(380.f, 280.f, context, property)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}