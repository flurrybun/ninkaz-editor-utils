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

class MixedInputPopup : public Popup {
protected:
    enum Operator {
        Add, Subtract, Multiply, Divide, Equal
    };

    enum DirectionType {
        None, Up, Down, Left, Right
    };
    
    MultiEditContext* m_context;
    int m_property;
    int m_decimalPlaces;
    PropertyBounds m_propertyBounds = {0, 0};
    
    CCArrayExt<GameObject*> m_objects;
    Operator m_operator = Operator::Equal;
    float m_modifierValue = 0;
    float m_initialValue = 0;
    RoundingType m_rounding = RoundingType::Round;
    DirectionType m_direction = DirectionType::None;

    CCMenu* m_topRow = nullptr;
    CCMenu* m_bottomRow = nullptr;
    CCMenuItemToggler* m_operatorBtn = nullptr;
    CCMenuItemToggler* m_directionBtn = nullptr;
    TextInput* m_modifierInput = nullptr;
    TextInput* m_initialInput = nullptr;
    CCLayerColor* m_equalBtnCover = nullptr;
    CCLayerColor* m_initialInputCover = nullptr;
    ListView* m_list = nullptr;

    struct CalculationInfo {
        std::string propertyString;
        std::string changeString;
        std::string newPropertyString;
        CCArrayExt<GameObject*> objects;

        CalculationInfo(const std::string& propStr, const std::string& changeStr, const std::string& newPropStr, const CCArrayExt<GameObject*>& obj)
            : propertyString(propStr), changeString(changeStr), newPropertyString(newPropStr), objects(obj) {}
    };

    bool init(MultiEditContext*, int);

    CCMenu* createTopRow();
    CCMenu* createBottomRow();
    void createScrollLayer(bool);
    CCSprite* spriteFromObject(GameObject*);

    void onOperator(CCObject*);
    void onDirection(CCObject*);
    void onValueArrow(CCObject*);
    void onSettings(CCObject*);
    void onApply(CCObject*);

    float applyOperation(float, float, Operator, bool = true);
    float roundValue(float);
    std::vector<MixedInputPopup::CalculationInfo> createStringMap();
public:
    static MixedInputPopup* create(MultiEditContext* context, int property) {
        auto popup = new MixedInputPopup;
        if (popup->init(context, property)) {
            popup->autorelease();
            return popup;
        }
        delete popup;
        return nullptr;
    }
};
