#pragma once

#include <Geode/Geode.hpp>
using namespace geode::prelude;

enum RoundingType {
    Round, Floor, Ceiling
};

struct MixedInputSettings {
    RoundingType rounding;
};

class MixedInputPopup : public Popup<const CCArrayExt<EffectGameObject*>&, const short, const InputValueType, const std::function<void (std::optional<float>)>&> {
protected:
    enum Operator {
        Add, Subtract, Multiply, Divide, Equal
    };

    enum DirectionType {
        None, Up, Down, Left, Right
    };
    
    CCArrayExt<EffectGameObject*> m_triggers;
    std::function<void (std::optional<float>)> m_callback;
    short m_property;
    Operator m_operator;
    float m_modifierValue;
    float m_initialValue;
    short m_decimalPlaces;
    bool m_canBeNegative;
    RoundingType m_rounding;
    DirectionType m_direction;

    CCMenu* m_topRow;
    CCMenu* m_bottomRow;
    CCMenuItemToggler* m_operatorBtn;
    CCMenuItemToggler* m_directionBtn;
    TextInput* m_modifierInput;
    TextInput* m_initialInput;
    CCLayerColor* m_equalBtnCover;
    CCLayerColor* m_initialInputCover;
    ListView* m_list;

    struct CalculationInfo {
        std::string propertyString;
        std::string changeString;
        std::string newPropertyString;
        CCArrayExt<EffectGameObject*> triggers;

        CalculationInfo(const std::string& propStr, const std::string& changeStr, const std::string& newPropStr, const CCArrayExt<EffectGameObject*>& trig)
            : propertyString(propStr), changeString(changeStr), newPropertyString(newPropStr), triggers(trig) {}
    };

    bool setup(const CCArrayExt<EffectGameObject*>&, const short, const InputValueType, const std::function<void (std::optional<float>)>&) override;

    CCMenu* createTopRow();
    CCMenu* createBottomRow();
    void createScrollLayer(bool);

    void onOperator(CCObject*);
    void onDirection(CCObject*);
    void onValueArrow(CCObject*);
    void onSettings(CCObject*);
    void onApply(CCObject*);

    float applyOperation(float, float, Operator, bool = true);
    std::string toTruncatedString(float, std::optional<int> = std::nullopt);
    float roundValue(float);
    std::vector<MixedInputPopup::CalculationInfo> createStringMap();
public:
    static MixedInputPopup* create(const CCArrayExt<EffectGameObject*>&, const short, const InputValueType, const std::function<void (std::optional<float>)>&);
};