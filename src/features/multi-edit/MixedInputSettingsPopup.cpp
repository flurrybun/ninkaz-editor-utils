#include "MixedInputSettingsPopup.hpp"

#include <Geode/Geode.hpp>
using namespace geode::prelude;

bool MixedInputSettingsPopup::init(MixedInputSettings settings, geode::Function<void(MixedInputSettings)> callback) {
    if (!Popup::init(250.f, 220.f)) return false;

    auto winSize = m_mainLayer->getContentSize();

    setTitle("Rounding Options");
    m_closeBtn->removeFromParent();

    m_settings = std::move(settings);
    m_callback = std::move(callback);

    // INFO BUTTON

    auto infoText = "<cg>Round</c> rounds the final value to the nearest value.\n"
        "<cy>Floor</c> always rounds down and <cl>ceiling</c> always rounds up.\n";
    
    auto infoBtn = InfoAlertButton::create("Info", infoText, 0.7f);
    infoBtn->setPosition(winSize - ccp(18, 18));
    m_buttonMenu->addChild(infoBtn);

    // ROUNDING TYPE

    auto roundingLayout = CCMenu::create();
    roundingLayout->setLayout(
        RowLayout::create()
            ->setGap(15.f)
            ->setAxisAlignment(AxisAlignment::Start)
            ->setGrowCrossAxis(true)
    );
    roundingLayout->setScale(0.8f);
    roundingLayout->setContentWidth(160);

    auto offSpr = CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");
    auto onSpr = CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png");

    const std::pair<const char*, RoundingType> roundingOptions[] = {
        {"Round", RoundingType::Round},
        {"Floor", RoundingType::Floor},
        {"Ceiling", RoundingType::Ceiling}
    };

    for (const auto& option : roundingOptions) {
        const char* labelText = option.first;
        RoundingType roundingType = option.second;

        auto label = CCLabelBMFont::create(labelText, "bigFont.fnt");
        label->setLayoutOptions(AxisLayoutOptions::create()
            ->setBreakLine(true)
        );

        auto btn = CCMenuItemToggler::create(
            offSpr, onSpr, this, menu_selector(MixedInputSettingsPopup::onRoundingButton)
        );
        btn->setTag(static_cast<int>(roundingType));
        btn->setLayoutOptions(AxisLayoutOptions::create()
            ->setNextGap(10.f)
        );

        if (m_settings.rounding == roundingType) {
            btn->toggle(true);
            m_roundingBtn = btn;
        }

        roundingLayout->addChild(btn);
        roundingLayout->addChild(label);
    }

    roundingLayout->updateLayout();
    m_mainLayer->addChildAtPosition(roundingLayout, Anchor::Center, {0, 8});

    // OK BUTTON

    auto applySpr = ButtonSprite::create("OK", "goldFont.fnt", "GJ_button_01.png", .9f);
    auto applyBtn = CCMenuItemSpriteExtra::create(
        applySpr, this, menu_selector(MixedInputSettingsPopup::onClose)
    );
    applyBtn->setPosition({winSize.width / 2, 24});

    m_buttonMenu->addChild(applyBtn);

    return true;
}

void MixedInputSettingsPopup::onRoundingButton(CCObject* sender) {
    if (m_roundingBtn) m_roundingBtn->toggle(false);

    auto btn = static_cast<CCMenuItemToggler*>(sender);
    m_roundingBtn = btn;
    
    m_settings.rounding = static_cast<RoundingType>(btn->getTag());
    m_callback(m_settings);
}
