#pragma once

#include "ValueCycle.hpp"

#include <Geode/utils/function.hpp>
#include <limits>
using namespace geode::prelude;

namespace me {

enum class Operation { Set, Add, Subtract, Multiply, Divide, Cycle };
enum class Direction { None, Up, Down, Left, Right, Random };

class Calculation : public CCObject {
public:
    static Calculation* create() {
        auto ret = new Calculation();
        ret->autorelease();
        return ret;
    }

    Operation m_operation = Operation::Set;
    Direction m_direction = Direction::None;
    double m_modifier = 0.0;
    double m_initial = 0.0;
    std::string m_cycleText = "1-5";
    double m_cycleStep = 1.0;
    bool m_combineLinked = false;
    bool m_preserveSign = false;
    int m_decimalPlaces = 0;

    struct Unit {
        enum class Origin { CycleValue, Operated, Set };
        std::vector<GameObject*> objects;
        double value = 0.0;
        size_t step = 0;
        Origin origin = Origin::Operated;
        double previous = 0.0;
        double operand = 0.0;
        size_t cycleIndex = 0;
        bool signChanged = false;
    };

    struct UnitGroup {
        std::vector<GameObject*> objects;
        std::string label;
        std::string description;
    };

private:
    Calculation() = default;

    std::vector<GameObject*> m_objects;
    std::unordered_map<GameObject*, float> m_initialValues;
    ValueCycle<float> m_cycle;
    std::vector<double> m_cycleValues;
    std::vector<Unit> m_units;
    double m_minValue = std::numeric_limits<double>::lowest();
    double m_maxValue = std::numeric_limits<double>::max();
    Function<void(std::vector<Unit>)> m_callback;

    bool m_recalculateQueued = false;

    void recalculate();
    void buildUnits();
    std::vector<size_t> getUnitIndices() const;

    double currentValue(const Unit& unit);
    double normalizeValue(double value) const;

    std::string formatNumber(double value) const;
    static std::string symbolForOperation(Operation op);
public:
    void queueRecalculate();

    void setRecalculateCallback(Function<void(std::vector<Unit>)> callback) { m_callback = std::move(callback); }
    void setObjects(std::vector<GameObject*> objects) { m_objects = std::move(objects); }
    void setInitialValues(std::unordered_map<GameObject*, float> values) { m_initialValues = std::move(values); }
    void setValueBounds(double min, double max) { m_minValue = min; m_maxValue = max; }

    const std::vector<Unit>& getUnits() { return m_units; }
    std::string getDescription(const Unit& unit) const;
    std::string getDescriptionConcise(const Unit& unit) const;
    std::vector<UnitGroup> makeUnitGroups(const std::vector<Unit>& units) const;
};

} // namespace me
