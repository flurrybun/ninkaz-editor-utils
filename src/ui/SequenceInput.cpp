#include "SequenceInput.hpp"

#include <Geode/Geode.hpp>
using namespace geode::prelude;

bool SequenceInput::init(InputCallback callback, Options options) {
    if (!CCNode::init()) return false;

    m_callback = std::move(callback);
    m_options = std::move(options);

    this->setAnchorPoint({ 0.5f, 0.5f });
    this->setLayout(
        SimpleRowLayout::create()
            ->setGap(10.f)
            ->setMainAxisScaling(AxisScaling::Fit)
            ->setCrossAxisScaling(AxisScaling::Fit)
    );

    m_input = TextInput::create(119.f, "Range", "bigFont.fnt");
    m_input->setFilter("0123456789.,- ");
    m_input->setString(m_options.initialValue);
    m_input->setCallback([this](const std::string& str) {
        m_callback(str);
    });

    if (auto str = m_options.label) {
        auto label = Label::create(*str, "goldFont.fnt");
        label->setAnchorPoint({ 1.f, 0.5f });
        label->setScale(0.6f);

        this->addChild(label);
    }

    this->addChild(m_input);
    this->updateLayout();

    return true;
}

SequenceInput* SequenceInput::create(InputCallback callback, Options options) {
    auto node = new SequenceInput;

    if (node->init(std::move(callback), std::move(options))) {
        node->autorelease();
        return node;
    }

    delete node;
    return nullptr;
}
