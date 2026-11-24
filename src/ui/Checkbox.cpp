#include "Checkbox.hpp"

#include <Geode/Geode.hpp>
using namespace geode::prelude;

bool Checkbox::init(ToggleCallback callback, Options options) {
    if (!CCMenu::init()) return false;

    m_callback = std::move(callback);
    m_options = std::move(options);
    m_value = m_options.initialValue;

    this->ignoreAnchorPointForPosition(false);
    this->setAnchorPoint({ 0.5f, 0.5f });
    this->setLayout(
        SimpleRowLayout::create()
            ->setGap(5.f)
            ->setMainAxisScaling(AxisScaling::Fit)
            ->setCrossAxisScaling(AxisScaling::Fit)
            ->setCrossAxisAlignment(CrossAxisAlignment::Center)
    );

    auto onSprite = CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png");
    onSprite->setScale(0.5f);
    auto offSprite = CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");
    offSprite->setScale(0.5f);

    m_toggler = CCMenuItemExt::createToggler(
        onSprite,
        offSprite,
        [this](CCObject* sender) {
            bool value = !static_cast<CCMenuItemToggler*>(sender)->isToggled();
            m_value = value;
            m_callback(value);
        }
    );

    m_label = Label::create(m_options.label, "bigFont.fnt");
    m_label->setScale(0.35f);

    this->addChild(m_toggler);
    this->addChild(m_label);

    m_toggler->toggle(m_value);
    this->updateLayout();

    return true;
}

Checkbox* Checkbox::create(ToggleCallback callback, Options options) {
    auto node = new Checkbox;

    if (node->init(std::move(callback), std::move(options))) {
        node->autorelease();
        return node;
    }

    delete node;
    return nullptr;
}
