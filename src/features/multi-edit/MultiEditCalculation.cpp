#include "MultiEditCalculation.hpp"
#include "Geode/utils/random.hpp"
#include <numeric>

#include <Geode/Geode.hpp>
using namespace geode::prelude;

namespace me {

double applyOperation(Operation op, double a, double b) {
    switch (op) {
        case Operation::Add:
            return a + b;
        case Operation::Subtract:
            return a - b;
        case Operation::Multiply:
            return a * b;
        case Operation::Divide:
            return b == 0.0 ? a : a / b;
        default:
            return b;
    }
}

double signedValue(double value, double reference) {
    // used for preserve sign
    return (reference < 0.0 ? -1.0 : 1.0) * std::fabs(value);
}

void Calculation::recalculate() {
    bool isCycle = m_operation == Operation::Cycle;
    bool isSet = m_operation == Operation::Set;
    bool directional = m_direction != Direction::None;

    if (isCycle) {
        m_cycle.set(m_cycleText, m_cycleStep);
        m_cycleValues.assign(m_cycle.values().begin(), m_cycle.values().end());
        if (m_cycleValues.empty()) m_cycleValues.push_back(0.0);
    } else {
        m_cycleValues = { m_modifier };
    }

    for (double& value : m_cycleValues) {
        value = normalizeValue(value);
    }

    std::vector<double> rawValues = m_cycleValues;

    if (m_preserveSign) {
        for (double& value : m_cycleValues) {
            value = std::fabs(value);
        }
    }

    buildUnits();

    if (isSet) {
        for (size_t u = 0; u < m_units.size(); u++) {
            Unit& unit = m_units[u];
            unit.step = u;
            unit.previous = currentValue(unit);
            unit.value = m_cycleValues.front();

            if (m_preserveSign) {
                unit.value = signedValue(unit.value, unit.previous);
                unit.signChanged = normalizeValue(unit.value) != rawValues.front();
            }

            unit.value = normalizeValue(unit.value);
            unit.origin = Unit::Origin::Set;
        }
    } else if (!directional) {
        for (size_t u = 0; u < m_units.size(); u++) {
            Unit& unit = m_units[u];
            unit.step = u;

            if (isCycle) {
                unit.cycleIndex = u % m_cycleValues.size();
                unit.operand = m_cycleValues[unit.cycleIndex];
                unit.origin = Unit::Origin::CycleValue;
                unit.value = unit.operand;

                if (m_preserveSign) {
                    unit.value = signedValue(unit.value, currentValue(unit));
                    unit.signChanged = normalizeValue(unit.value) != rawValues[unit.cycleIndex];
                }

                unit.value = normalizeValue(unit.value);
            } else {
                unit.previous = currentValue(unit);
                unit.operand = m_cycleValues.front();
                unit.origin = Unit::Origin::Operated;

                double base = m_preserveSign ? std::fabs(unit.previous) : unit.previous;
                double value = applyOperation(m_operation, base, unit.operand);

                if (m_preserveSign) {
                    value = signedValue(value, unit.previous);
                    double noBox = normalizeValue(applyOperation(m_operation, unit.previous, rawValues.front()));
                    unit.signChanged = normalizeValue(value) != noBox;
                }

                unit.value = normalizeValue(value);
            }
        }
    } else {
        std::vector<size_t> indices = getUnitIndices();
        double current = normalizeValue(m_initial);
        double noBoxCurrent = current;
        if (m_preserveSign) current = std::fabs(current);

        for (size_t i = 0; i < indices.size(); i++) {
            Unit& unit = m_units[indices[i]];
            unit.step = i;

            double sign = 1.0;
            if (m_preserveSign) {
                sign = currentValue(unit) < 0.0 ? -1.0 : 1.0;
            }

            if (isCycle) {
                unit.cycleIndex = i % m_cycleValues.size();
                unit.operand = m_cycleValues[unit.cycleIndex];
                unit.origin = Unit::Origin::CycleValue;
                current = unit.operand;
                noBoxCurrent = rawValues[unit.cycleIndex];
                unit.value = normalizeValue(current * sign);

                if (m_preserveSign) {
                    unit.signChanged = unit.value != noBoxCurrent;
                }
            } else {
                if (i > 0) {
                    unit.previous = current;
                    unit.operand = m_cycleValues.front();
                    current = applyOperation(m_operation, current, unit.operand);

                    if (m_preserveSign) current = std::fabs(current);

                    current = normalizeValue(current);
                    noBoxCurrent = normalizeValue(applyOperation(m_operation, noBoxCurrent, rawValues.front()));
                }

                unit.origin = Unit::Origin::Operated;
                unit.value = normalizeValue(current * sign);

                if (m_preserveSign) unit.signChanged = unit.value != noBoxCurrent;
            }
        }
    }
}

void Calculation::buildUnits() {
    m_units.clear();
    m_units.reserve(m_objects.size());

    std::unordered_map<int, Unit*> linkMap;

    for (GameObject* object : m_objects) {
        int linkedGroup = object->m_linkedGroup;

        if (m_combineLinked && linkedGroup > 0) {
            auto found = linkMap.find(linkedGroup);

            if (found != linkMap.end()) {
                found->second->objects.push_back(object);
                continue;
            }
        }

        m_units.emplace_back();
        m_units.back().objects.push_back(object);

        if (m_combineLinked && linkedGroup > 0) {
            linkMap[linkedGroup] = &(m_units.back());
        }
    }
}

double Calculation::currentValue(const Unit& unit) {
    if (unit.objects.empty()) return 0.0;
    return m_initialValues[unit.objects.front()];
}

double Calculation::normalizeValue(double value) const {
    double scale = std::pow(10.0, static_cast<double>(m_decimalPlaces));
    value = std::round(value * scale) / scale;

    // this looks stupid but it prevents value being -0.0
    if (value == 0.0) value = 0.0;

    return std::clamp(value, m_minValue, m_maxValue);
}

std::vector<size_t> Calculation::getUnitIndices() const {
    size_t size = m_units.size();
    std::vector<size_t> indices(size);
    std::iota(indices.begin(), indices.end(), 0);

    if (m_direction == Direction::Random) {
        random::shuffle(indices);
        return indices;
    }

    std::vector<double> x(size, 0.0), y(size, 0.0);

    for (size_t u = 0; u < size; u++) {
        const auto& objects = m_units[u].objects;

        for (GameObject* object : objects) {
            x[u] += object->getPositionX();
            y[u] += object->getPositionY();
        }

        x[u] /= static_cast<double>(objects.size());
        y[u] /= static_cast<double>(objects.size());
    }

    auto sortBy = [&](const std::vector<double>& key, bool ascending) {
        std::ranges::sort(indices, [&](size_t a, size_t b) {
            return ascending ? key[a] < key[b] : key[a] > key[b];
        });
    };

    switch (m_direction) {
        case Direction::Right:
            sortBy(x, true);
            break;
        case Direction::Left:
            sortBy(x, false);
            break;
        case Direction::Up:
            sortBy(y, true);
            break;
        case Direction::Down:
            sortBy(y, false);
            break;
        default:
            break;
    }

    return indices;
}

std::string Calculation::getDescription(const Unit& unit) const {
    StringBuffer desc;
    std::vector<std::string> sidenotes;

    switch (unit.origin) {
        case Unit::Origin::CycleValue:
            if (unit.cycleIndex == 0) {
                desc.append("First step: ");
            } else if (unit.cycleIndex + 1 == m_cycleValues.size()) {
                desc.append("Last step: ");
            } else {
                desc.append("Step {} of {}: ", unit.cycleIndex + 1, m_cycleValues.size());
            }
            desc.append("{}", formatNumber(unit.value));
            break;
        case Unit::Origin::Set:
            desc.append(
                "Set to {}, previously {}",
                formatNumber(unit.value), formatNumber(unit.previous)
            );
            break;
        case Unit::Origin::Operated:
            if (m_direction != Direction::None) {
                if (unit.step == 0) {
                    desc.append("First step: ");
                    desc.append("{}", formatNumber(unit.value));
                    sidenotes.push_back("initial value");
                    break;
                } else if (unit.step + 1 == m_units.size()) {
                    desc.append("Last step: ");
                } else {
                    desc.append("Step {} of {}: ", unit.step + 1, m_units.size());
                }
            }
            desc.append(
                "{} {} {} = {}",
                formatNumber(m_preserveSign ? std::fabs(unit.previous) : unit.previous),
                symbolForOperation(m_operation),
                formatNumber(unit.operand),
                formatNumber(unit.value)
            );
            break;
    }

    if (unit.signChanged) sidenotes.push_back("sign preserved");

    if (sidenotes.size() > 0) {
        desc.append(" (");
        for (size_t i = 0; i < sidenotes.size(); i++) {
            if (i > 0) desc.append(", ");
            desc.append("{}", sidenotes[i]);
        }
        desc.append(")");
    }

    return desc.str();
}

std::string Calculation::getDescriptionConcise(const Unit& unit) const {
    StringBuffer desc;

    switch (unit.origin) {
        case Unit::Origin::CycleValue:
            desc.append(
                "{} (step {})",
                formatNumber(unit.value), unit.cycleIndex + 1
            );
            break;
        case Unit::Origin::Set:
            desc.append(
                "{} -> {}",
                formatNumber(unit.previous), formatNumber(unit.value)
            );
            break;
        case Unit::Origin::Operated:
            if (m_direction != Direction::None && unit.step == 0) {
                desc.append("{} (initial)", formatNumber(unit.value));
            } else {
                desc.append(
                    "{} {} {} = {}",
                    formatNumber(m_preserveSign ? std::fabs(unit.previous) : unit.previous),
                    symbolForOperation(m_operation),
                    formatNumber(unit.operand),
                    formatNumber(unit.value)
                );
            }
            break;
    }

    return desc.str();
}

std::vector<Calculation::UnitGroup> Calculation::makeUnitGroups(const std::vector<Unit>& units) const {
    if (units.empty()) return {};

    bool isCycle = units.front().origin == Unit::Origin::CycleValue;
    bool isSet = units.front().origin == Unit::Origin::Set;

    std::vector<UnitGroup> groups;
    std::unordered_map<size_t, size_t> cycleKeys;
    std::unordered_map<double, size_t> valueKeys;

    auto addToGroup = [&](auto& keys, auto key, const Unit& unit) {
        auto it = keys.find(key);

        if (it == keys.end()) {
            keys.emplace(key, groups.size());
            groups.push_back({
                .objects = unit.objects,
                .label = this->getDescriptionConcise(unit),
                .description = this->getDescription(unit)
            });
        } else {
            groups[it->second].objects.append_range(unit.objects);
        }
    };

    std::vector<size_t> order(units.size());
    std::iota(order.begin(), order.end(), 0);

    std::ranges::sort(order, [&](size_t a, size_t b) {
        return units[a].step < units[b].step;
    });

    for (size_t u : order) {
        const Unit& unit = units[u];

        if (isCycle) {
            // group by cycle index, separated if sign is preserved
            size_t key = unit.cycleIndex;
            if (m_preserveSign) key = key * 2 + (unit.value < 0.0 ? 1 : 0);

            addToGroup(cycleKeys, key, unit);
        } else if (isSet) {
            // group by previous value
            addToGroup(valueKeys, unit.previous, unit);
        } else {
            // group by new value
            addToGroup(valueKeys, unit.value, unit);
        }
    }

    return groups;
}

std::string Calculation::formatNumber(double value) const {
    return numToString(value, m_decimalPlaces);
}

std::string Calculation::symbolForOperation(Operation op) {
    switch (op) {
        case Operation::Add: 
            return "+";
        case Operation::Subtract:
            return "-";
        case Operation::Multiply:
            return "x";
        case Operation::Divide:
            return "/";
        default:
            return "=";
    }
}

void Calculation::queueRecalculate() {
    if (m_recalculateQueued) return;
    m_recalculateQueued = true;

    queueInMainThread([selfRef = WeakRef(this)]() {
        auto self = selfRef.lock();
        if (!self) return;

        self->recalculate();
        if (self->m_callback) self->m_callback(self->m_units);

        self->m_recalculateQueued = false;
    });
}

} // namespace me
