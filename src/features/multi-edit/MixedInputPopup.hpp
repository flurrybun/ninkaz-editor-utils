#pragma once

#include "MultiEditContext.hpp"

#include <Geode/Geode.hpp>
using namespace geode::prelude;

enum RoundingType {
    Round, Floor, Ceiling
};

struct MixedInputSettings {
    RoundingType rounding;
};

class MixedInputPopup : public Popup<MultiEditContext*, int> {
protected:
    enum Operator {
        Add, Subtract, Multiply, Divide, Equal
    };

    enum DirectionType {
        None, Up, Down, Left, Right
    };
    
    MultiEditContext* m_context;
    CCArrayExt<GameObject*> m_objects;
    int m_property;
    Operator m_operator;
    float m_modifierValue;
    float m_initialValue;
    int m_decimalPlaces;
    PropertyBounds m_propertyBounds = {0, 0};
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
        CCArrayExt<GameObject*> objects;

        CalculationInfo(const std::string& propStr, const std::string& changeStr, const std::string& newPropStr, const CCArrayExt<GameObject*>& obj)
            : propertyString(propStr), changeString(changeStr), newPropertyString(newPropStr), objects(obj) {}
    };

    bool setup(MultiEditContext*, int) override;

    CCMenu* createTopRow();
    CCMenu* createBottomRow();
    void createScrollLayer(bool);

    void onOperator(CCObject*);
    void onDirection(CCObject*);
    void onValueArrow(CCObject*);
    void onSettings(CCObject*);
    void onApply(CCObject*);

    float applyOperation(float, float, Operator, bool = true);
    float roundValue(float);
    std::vector<MixedInputPopup::CalculationInfo> createStringMap();
public:
    static MixedInputPopup* create(MultiEditContext*, int);
};