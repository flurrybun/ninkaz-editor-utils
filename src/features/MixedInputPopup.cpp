#include "MixedInputPopup.hpp"
#include <Geode/ui/TextInput.hpp>
#include <Geode/ui/BasedButtonSprite.hpp>

#include <Geode/Geode.hpp>
using namespace geode::prelude;

#include <vector>
#include <map>
#include <algorithm>
#include <variant>

bool MixedInputPopup::setup(const std::vector<Trigger>& triggers, Trigger::PropType type) {
    m_triggers = triggers;
    m_type = type;
    m_operator = Operator::Equal;
    
    auto winSize = m_mainLayer->getContentSize();
    m_bgSprite->setZOrder(-10);

    // INFO BUTTON

    auto infoText = "Modify the <cg>mixed value</c> of the <cy>selected input</c>.\n"
        "Use the <cl>equals button</c> to override the old value. Use the <cp>operator buttons</c> to add, subtract, multiply, or divide the old value.\n";
    
    auto infoBtn = InfoAlertButton::create("Info", infoText, 0.7f);
    infoBtn->setPosition(winSize - ccp(18, 18));
    m_buttonMenu->addChild(infoBtn);

    // LABEL

    auto label = CCLabelBMFont::create("Modify Mixed Value", "goldFont.fnt");
    label->setScale(0.6);
    label->setPosition({winSize. width / 2, 93});
    m_mainLayer->addChild(label);

    // OPERATOR BUTTONS AND INPUT ROW

    auto bottomLayout = CCMenu::create();
    bottomLayout->setLayout(
        RowLayout::create()
            ->setGap(10.f)
    );
    bottomLayout->setPosition({winSize.width / 2, 65});
    bottomLayout->setScale(0.8f);
    
    bottomLayout->addChild(createOperatorButton(Operator::Equal));
    bottomLayout->addChild(createOperatorButton(Operator::Add));
    bottomLayout->addChild(createOperatorButton(Operator::Subtract));
    bottomLayout->addChild(createOperatorButton(Operator::Multiply));
    bottomLayout->addChild(createOperatorButton(Operator::Divide));

    auto input = TextInput::create(70.f, "Num", "bigFont.fnt");
    input->setFilter("0123456789");
    input->setMaxCharCount(4);
    input->setString("0");
    input->setCallback([&](const std::string& text) {
        if (text.empty()) return;

        if (std::holds_alternative<int>(m_value)) {
            m_value = std::stoi(text);
        } else {
            m_value = std::stof(text);
        }

        createScrollLayer(false);
    });
    input->setLayoutOptions(
        AxisLayoutOptions::create()
            ->setNextGap(10.f)
            ->setPrevGap(10.f)
    );

    auto decArrowSpr = CCSprite::createWithSpriteFrameName("edit_leftBtn_001.png");
    auto decArrowBtn = CCMenuItemSpriteExtra::create(
        decArrowSpr, this, nullptr
    );
    decArrowBtn->setTag(-1);
    decArrowBtn->setLayoutOptions(
        AxisLayoutOptions::create()
            ->setPrevGap(15.f)
    );

    auto incArrowSpr = CCSprite::createWithSpriteFrameName("edit_rightBtn_001.png");
    auto incArrowBtn = CCMenuItemSpriteExtra::create(
        incArrowSpr, this, nullptr
    );
    incArrowBtn->setTag(1);

    bottomLayout->addChild(decArrowBtn);
    bottomLayout->addChild(input);
    bottomLayout->addChild(incArrowBtn);

    m_buttonMenu->addChild(bottomLayout);
    bottomLayout->updateLayout();

    // SCROLL LAYER

    createScrollLayer(true);

    // APPLY & CANCEL BUTTONS

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
    m_buttonMenu->addChild(buttonLayout);
    buttonLayout->updateLayout();

    return true;
}

void MixedInputPopup::createScrollLayer(bool isInit) {
    auto winSize = m_mainLayer->getContentSize();
    auto scrollSize = CCSize(300, 150);
    auto scrollPosition = winSize / 2 + ccp(0, 46);
    
    if (isInit) {
        auto scroll = ScrollLayer::create(scrollSize);
        scroll->setAnchorPoint({0, 0});
        scroll->setPosition(scrollPosition - scrollSize / 2);
        m_scroll = scroll;

        scroll->m_contentLayer->setLayout(
            ColumnLayout::create()
                ->setAxisReverse(true)
                ->setAxisAlignment(AxisAlignment::End)
                ->setAutoGrowAxis(scrollSize.height)
                ->setGap(0)
        );

        // BORDER

        auto topBorder = CCSprite::createWithSpriteFrameName("GJ_commentTop_001.png");
        topBorder->setPosition(scrollPosition + ccp(0, scrollSize.height / 2 - 4));
        topBorder->setZOrder(10);
        topBorder->setScale(0.885);
        auto bottomBorder = CCSprite::createWithSpriteFrameName("GJ_commentTop_001.png");
        bottomBorder->setPosition(scrollPosition + ccp(0, -scrollSize.height / 2 + 4));
        bottomBorder->setZOrder(10);
        bottomBorder->setFlipY(true);
        bottomBorder->setScale(0.885);
        auto leftBorder = CCSprite::createWithSpriteFrameName("GJ_commentSide_001.png");
        leftBorder->setPosition(scrollPosition + ccp(-scrollSize.width / 2 - 3.4, 0));
        leftBorder->setZOrder(9);
        leftBorder->setScaleX(0.885);
        leftBorder->setScaleY(4.5);
        auto rightBorder = CCSprite::createWithSpriteFrameName("GJ_commentSide_001.png");
        rightBorder->setPosition(scrollPosition + ccp(scrollSize.width / 2 + 3.4, 0));
        rightBorder->setZOrder(9);
        rightBorder->setFlipX(true);
        rightBorder->setScaleX(0.885);
        rightBorder->setScaleY(4.5);

        m_mainLayer->addChild(topBorder);
        m_mainLayer->addChild(bottomBorder);
        m_mainLayer->addChild(leftBorder);
        m_mainLayer->addChild(rightBorder);

        // BACKGROUND

        auto bg = CCLayerColor::create();
        bg->setColor(ccc3(191, 114, 62));
        bg->setOpacity(255);
        bg->setContentSize(scrollSize);
        bg->setPosition(scrollPosition);
        bg->ignoreAnchorPointForPosition(false);
        bg->setZOrder(-1);

        m_mainLayer->addChild(bg);
    } else {
        m_scroll->m_contentLayer->removeAllChildren();
    }

    auto stringMap = std::visit([&](auto&& arg) -> decltype(auto) {
        using T = std::decay_t<decltype(arg)>;
        return createStringMap<T>(m_triggers);
    }, m_value);
    
    auto length = stringMap.size();
    auto index = 0;
    auto rowSize = CCSize(scrollSize.width, 37);

    m_scroll->m_contentLayer->setContentSize({rowSize.width, rowSize.height * length});
    
    for (const auto& [oldString, pair ] : stringMap) {
        index++;
        auto [newString, triggers] = pair;

        auto bg = CCLayerColor::create();
        bg->setColor((index % 2 == 0) ? ccc3(144, 79, 39) : ccc3(156, 85, 42));
        bg->setOpacity(255);
        bg->setContentSize(rowSize);
        bg->setAnchorPoint({0.5, 0});

        // TRIGGER COUNT LAYOUT

        auto triggerBG = CCScale9Sprite::create("square02b_001.png", {0, 0, 80, 80});
        triggerBG->setColor({ 0, 0, 0 });
        triggerBG->setOpacity(50);
        triggerBG->setContentSize({160, 55});
        triggerBG->setPosition({44, rowSize.height / 2});
        triggerBG->setScale(0.5);

        auto triggerLayout = CCMenu::create();
        triggerLayout->setLayout(
            RowLayout::create()
                ->setAxisAlignment(AxisAlignment::Even)
        );
        triggerLayout->setPosition(triggerBG->getPosition());
        triggerLayout->setContentWidth(112);
        triggerLayout->setScale(0.7);

        std::map<int, std::pair<CCSpriteFrame*, int>> triggerCounts;
        for (const auto& trigger : triggers) {
            auto count = triggerCounts[trigger.object->m_objectID].second;
            triggerCounts[trigger.object->m_objectID] = {trigger.object->displayFrame(), count + 1};
        }

        for (const auto& [id, pair] : triggerCounts) {
            auto [frameName, count] = pair;

            auto spr = CCSprite::createWithSpriteFrame(frameName);

            auto label = CCLabelBMFont::create(std::to_string(count).c_str(), "bigFont.fnt");
            label->setPosition({spr->getContentWidth() / 2 + 8, -2});
            label->setScale(0.6f);
            label->setAnchorPoint({0.5, 0});

            spr->addChild(label);
            triggerLayout->addChild(spr);
        }

        bg->addChild(triggerBG);
        bg->addChild(triggerLayout);
        triggerLayout->updateLayout();

        // CALCULATION LAYOUT

        auto calcLayout = CCMenu::create();
        calcLayout->setLayout(
            RowLayout::create()
                ->setAxisAlignment(AxisAlignment::Between)
        );
        calcLayout->setPosition({192, rowSize.height / 2});
        calcLayout->setContentWidth(247);
        calcLayout->setScale(0.7f);

        auto oldLabel = CCLabelBMFont::create(oldString.c_str(), "bigFont.fnt");
        auto changeLabel = CCLabelBMFont::create(getValueString().c_str(), "bigFont.fnt");
        auto newLabel = CCLabelBMFont::create(newString.c_str(), "bigFont.fnt");

        auto signOptions = AxisLayoutOptions::create()
            ->setMaxScale(0.6f);

        auto sign = createOperatorSprite(m_operator);
        sign->setLayoutOptions(signOptions);
        auto equalSign = createOperatorSprite(Operator::Equal);
        equalSign->setLayoutOptions(signOptions);

        calcLayout->addChild(oldLabel);
        if (m_operator != Operator::Equal) calcLayout->addChild(sign);
        if (m_operator != Operator::Equal) calcLayout->addChild(changeLabel);
        calcLayout->addChild(equalSign);
        calcLayout->addChild(newLabel);

        bg->addChild(calcLayout);
        calcLayout->updateLayout();

        // visually align text with operators
        oldLabel->setPositionY(oldLabel->getPositionY() + 2);
        changeLabel->setPositionY(changeLabel->getPositionY() + 2);
        newLabel->setPositionY(newLabel->getPositionY() + 2);

        // BOTTOM BORDER

        auto border = CCLayerColor::create();
        border->setColor(ccc3(0, 0, 0));
        border->setOpacity(255 / 2);
        border->setContentSize({rowSize.width, 0.8});
        border->setPosition({0, -0.4});

        bg->addChild(border);
        m_scroll->m_contentLayer->addChild(bg);
    }

    m_scroll->m_contentLayer->updateLayout();

    if (isInit) {
        m_mainLayer->addChild(m_scroll);
        m_scroll->scrollToTop();
    }
}

CCSprite* MixedInputPopup::createOperatorSprite(const Operator& sign) {
    auto name = sign == Operator::Add ? "plus-operator.png"_spr :
        sign == Operator::Subtract ? "minus-operator.png"_spr :
        sign == Operator::Multiply ? "multiply-operator.png"_spr :
        sign == Operator::Divide ? "divide-operator.png"_spr : "equal-operator.png"_spr;

    return CCSprite::createWithSpriteFrameName(name);
}

CCMenuItemToggler* MixedInputPopup::createOperatorButton(const Operator& sign) {
    auto offSpr = createOperatorBase(false);
    auto onSpr = createOperatorBase(true);
    auto topSpr = createOperatorSprite(sign);
    topSpr->setPosition(offSpr->getContentSize() / 2);
    topSpr->setScale(0.8f);
    
    offSpr->addChild(topSpr);
    onSpr->addChild(topSpr);

    auto btn = CCMenuItemToggler::create(
        offSpr, onSpr, this, menu_selector(MixedInputPopup::onOperator)
    );

    btn->setTag(static_cast<int>(sign));

    if (sign == Operator::Equal) {
        btn->toggle(true);
        m_operatorBtn = btn;
    };

    return btn;
}

CCScale9Sprite* MixedInputPopup::createOperatorBase(bool isSelected) {
    auto spr = CCScale9Sprite::create(isSelected ? "GJ_button_02.png" : "GJ_button_04.png", {0, 0, 40, 40});
    spr->setContentSize({35, 35});
    spr->setScale(0.8f);
    return spr;
}

void MixedInputPopup::onOperator(CCObject* sender) {
    if (m_operatorBtn) m_operatorBtn->toggle(false);

    auto btn = static_cast<CCMenuItemToggler*>(sender);
    m_operator = static_cast<Operator>(btn->getTag());
    m_operatorBtn = btn;

    createScrollLayer(false);
}

void MixedInputPopup::onApply(CCObject* sender) {
    for (auto& trigger : m_triggers) {
        if (!trigger.hasProperty(m_type)) continue;
        auto property = trigger.getProperty(m_type);

        if (std::holds_alternative<int>(property)) {
            auto oldProperty = std::get<int>(property);
            auto newProperty = applyOperation(oldProperty, std::get<int>(m_value), m_operator);

            trigger.setProperty(m_type, newProperty);
        } else if (std::holds_alternative<float>(property)) {
            auto oldProperty = std::get<float>(property);
            auto newProperty = applyOperation(oldProperty, std::get<float>(m_value), m_operator);
            
            trigger.setProperty(m_type, newProperty);
        }
    }
    
    onClose(sender);
}

template<typename T>
T MixedInputPopup::applyOperation(T lhs, T rhs, Operator op) {
    switch (op) {
        case Operator::Add: return lhs + rhs;
        case Operator::Subtract: return lhs - rhs;
        case Operator::Multiply: return lhs * rhs;
        case Operator::Divide: 
            if (rhs == 0) return 0;
            return lhs / rhs;
        default: return rhs;
    }
}

std::string MixedInputPopup::getValueString() {
    if (std::holds_alternative<int>(m_value)) return std::to_string(std::get<int>(m_value));
    else return std::to_string(std::get<float>(m_value));
}

template<typename T>
std::map<std::string, std::pair<std::string, std::vector<Trigger>>> MixedInputPopup::createStringMap(const std::vector<Trigger>& triggers) {
    std::map<std::string, std::pair<std::string, std::vector<Trigger>>> map;

    for (const auto& trigger : triggers) {
        if (!trigger.hasProperty(m_type)) continue;

        T property = trigger.getProperty<T>(m_type);
        T newProperty = applyOperation(property, std::get<T>(m_value), m_operator);

        auto propertyString = std::to_string(property);

        auto it = map.find(propertyString);
        if (it == map.end()) {
            std::vector<Trigger> vec = {trigger};
            map[propertyString] = std::make_pair(std::to_string(newProperty), vec);
        } else {
            it->second.second.push_back(trigger);
        }
    }

    return map;
}

MixedInputPopup* MixedInputPopup::create(const std::vector<Trigger>& triggers, Trigger::PropType type){
    auto ret = new MixedInputPopup();
    if (ret && ret->initAnchored(380.f, 280.f, triggers, type)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}