#include "NumberInput.hpp"

#include <Geode/Geode.hpp>
using namespace geode::prelude;

bool NumberInput::init(InputCallback callback, Options options) {
    if (!CCNode::init()) return false;

    m_callback = std::move(callback);
    m_options = std::move(options);
    m_value = m_options.initialValue;

    this->setAnchorPoint({ 0.5f, 0.5f });
    this->setLayout(
        SimpleRowLayout::create()
            ->setGap(10.f)
            ->setMainAxisScaling(AxisScaling::Fit)
            ->setCrossAxisScaling(AxisScaling::Fit)
    );

    m_input = TextInput::create(70.f, "Num", "bigFont.fnt");
    m_input->setFilter("0123456789.-");
    m_input->getInputNode()->m_numberInput = true;
    m_input->setString(numToString(m_options.initialValue, m_options.decimalPlaces));
    m_input->setCallback([this](const std::string& str) {
        Result<double> value = numFromString<double>(str.empty() ? "0" : str);
        if (!value) return;

        this->m_value = *value;
        m_callback(*value);
    });

    if (auto str = m_options.label) {
        m_label = Label::create(*str, "goldFont.fnt");
        m_label->setAnchorPoint({ 1.f, 0.5f });
        m_label->setScale(0.6f);

        this->addChild(m_label);
    }

    m_decrementBtn = Button::createWithSpriteFrameName(
        "edit_leftBtn_001.png",
        [this](CCObject* sender) {
            setValue(m_value - 1);
        }
    );

    m_incrementBtn = Button::createWithSpriteFrameName(
        "edit_rightBtn_001.png",
        [this](CCObject* sender) {
            setValue(m_value + 1);
        }
    );

    this->addChild(m_decrementBtn);
    this->addChild(m_input);
    this->addChild(m_incrementBtn);

    this->updateLayout();

    return true;
}

void NumberInput::setValue(double value) {
    m_value = value;
    m_input->setString(numToString(value, m_options.decimalPlaces), true);
}

void NumberInput::setEnabled(bool enabled) {
    m_input->setEnabled(enabled);
    m_decrementBtn->setEnabled(enabled);
    m_incrementBtn->setEnabled(enabled);

    m_label->setColor(enabled ? ccWHITE : ccGRAY);
    m_label->setOpacity(enabled ? 255 : 100);
    m_input->getBGSprite()->setOpacity(enabled ? 90 : 35);
    m_decrementBtn->setColor(enabled ? ccWHITE : ccGRAY);
    m_decrementBtn->setOpacity(enabled ? 255 : 100);
    m_incrementBtn->setColor(enabled ? ccWHITE : ccGRAY);
    m_incrementBtn->setOpacity(enabled ? 255 : 100);
}

NumberInput* NumberInput::create(InputCallback callback, Options options) {
    auto node = new NumberInput;

    if (node->init(std::move(callback), std::move(options))) {
        node->autorelease();
        return node;
    }

    delete node;
    return nullptr;
}
