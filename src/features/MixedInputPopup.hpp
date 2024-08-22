#pragma once

#include <Geode/Geode.hpp>
#include "MultiEditTriggers.hpp"

using namespace geode::prelude;

enum RoundingType {
    Round, Floor, Ceiling
};

struct MixedInputSettings {
    RoundingType rounding;
};

class MixedInputPopup : public Popup<const std::vector<Trigger>&, Trigger::PropType> {
protected:
    enum Operator {
        Add, Subtract, Multiply, Divide, Equal
    };

    enum DirectionType {
        None, Up, Down, Left, Right
    };
    
    std::vector<Trigger> m_triggers;
    Trigger::PropType m_type;
    Operator m_operator;
    float m_modifierValue;
    float m_initialValue;
    bool m_isFloat;
    bool m_canBeNegative;
    RoundingType m_rounding;
    DirectionType m_direction;
    bool m_isFirstPage;

    CCMenuItemToggler* m_operatorBtn;
    CCMenuItemToggler* m_directionBtn;
    ScrollLayer* m_scroll;
    CCMenu* m_pageMenu;

    struct CalculationInfo {
        std::string propertyString;
        std::string changeString;
        std::string newPropertyString;
        std::vector<Trigger> triggers;

        CalculationInfo(const std::string& propStr, const std::string& changeStr, const std::string& newPropStr, const std::vector<Trigger>& trig)
            : propertyString(propStr), changeString(changeStr), newPropertyString(newPropStr), triggers(trig) {}
    };

    bool setup(const std::vector<Trigger>&, Trigger::PropType) override;

    void createFirstPageRow();
    void createSecondPageRow();
    void createScrollLayer(bool);
    void onOperator(CCObject*);
    void onDirection(CCObject*);
    void onValueArrow(CCObject*);
    void onChangePage(CCObject*);
    void onSettings(CCObject*);
    void onApply(CCObject*);

    std::string toTruncatedString(float);
    std::string toRoundedString(float);
    float applyOperation(float, float, Operator);
    std::vector<MixedInputPopup::CalculationInfo> createStringMap(const std::vector<Trigger>&);
public:
    static MixedInputPopup* create(const std::vector<Trigger>&, Trigger::PropType);
};

class SettingsPopup : public Popup<MixedInputSettings, std::function<void(MixedInputSettings)>> {
protected:
    MixedInputSettings m_settings;
    std::function<void(MixedInputSettings)> m_callback;
    CCMenuItemToggler* m_roundingBtn;

    bool setup(MixedInputSettings, std::function<void(MixedInputSettings)>) override;
    void onRoundingButton(CCObject*);
public:
    static SettingsPopup* create(MixedInputSettings, std::function<void(MixedInputSettings)>);
};