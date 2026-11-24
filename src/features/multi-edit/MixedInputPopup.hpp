#pragma once

#include "MultiEditContext.hpp"
#include "MultiEditCalculation.hpp"
#include "../../ui/ToggleGroup.hpp"
#include "../../ui/NumberInput.hpp"

#include <Geode/ui/Popup.hpp>
#include <Geode/binding/GameObject.hpp>
using namespace geode::prelude;

namespace me {

struct ObjectIDFrequency {
    int objectID;
    int count;
};

class MixedInputPopup : public Popup {
protected:    
    MultiEditContext* m_context;
    int m_property;
    int m_decimalPlaces;
    PropertyBounds m_propertyBounds = {0, 0};

    Ref<Calculation> m_calculation;

    Ref<ToggleGroup<Operation>> m_operationGroup;
    Ref<ToggleGroup<Direction>> m_directionGroup;
    Ref<CCNode> m_inputColumn;
    Ref<CCNode> m_rangeColumn;
    Ref<NumberInput> m_modifierInput;
    Ref<NumberInput> m_initialInput;
    Ref<CCNode> m_previewFrame;
    Ref<CCNode> m_selectedObjectRow;

    bool init(MultiEditContext* context, int property);

    void updateInputVisibility();
    void createCalculationPreview(const std::vector<Calculation::Unit>& units);
    void onApply();

    static std::string getRandomFrameName();
    static CCNode* createSpriteForObjectID(int id);
    static std::vector<ObjectIDFrequency> getObjectIDFrequencies(const std::vector<GameObject*>& objects);
    static CCNode* createObjectFrequencyRow(const std::vector<GameObject*>& objects, float max);
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

} // namespace me
