#pragma once

#include <Geode/Geode.hpp>
using namespace geode::prelude;

namespace nk {
    std::string toString(float value, std::optional<int> decimalPlaces = std::nullopt, bool removeTrailingZeros = true);
    int toInt(const std::string& string, int fallbackValue = 0);
    float toFloat(const std::string& string, float fallbackValue = 0.f);
}
