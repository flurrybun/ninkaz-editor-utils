#pragma once

#include <Geode/Geode.hpp>
#include "MultiEditTriggers.hpp"

using namespace geode::prelude;

enum Operator {
    Add, Subtract, Multiply, Divide, Equal
};

class MixedInputPopup : public Popup<const std::vector<Trigger>&, Trigger::PropType> {
protected:
    std::vector<Trigger> m_triggers;
    Trigger::PropType m_type;
    Operator m_operator;
    CCMenuItemToggler* m_operatorBtn;
    std::variant<int, float> m_value;
    ScrollLayer* m_scroll;

    bool setup(const std::vector<Trigger>&, Trigger::PropType) override;

    void createScrollLayer(bool);
    CCSprite* createOperatorSprite(const Operator&);
    CCMenuItemToggler* createOperatorButton(const Operator&);
    CCScale9Sprite* createOperatorBase(bool);
    void onOperator(CCObject*);
    void onApply(CCObject* sender);

    std::string getValueString();
    template<typename T>
    std::map<std::string, std::pair<std::string, std::vector<Trigger>>> createStringMap(const std::vector<Trigger>&);
    template<typename T>
    T applyOperation(T, T, Operator);
public:
    static MixedInputPopup* create(const std::vector<Trigger>&, Trigger::PropType);
};