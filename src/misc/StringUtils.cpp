#include "StringUtils.hpp"

namespace nk {
    std::string toString(float value, std::optional<int> decimalPlaces, bool removeTrailingZeros) {
        std::string str = std::to_string(value);

        if (decimalPlaces.has_value()) {
            auto factor = std::pow(10, decimalPlaces.value());
            value = std::round(value * factor) / factor;

            std::ostringstream out;
            out << std::fixed << std::setprecision(decimalPlaces.value()) << value;
            str = out.str();
        }

        if (removeTrailingZeros && str.find('.') != std::string::npos) {
            str.erase(str.find_last_not_of('0') + 1, std::string::npos);
            str.erase(str.find_last_not_of('.') + 1, std::string::npos);
        }

        return str;
    }

    int toInt(const std::string& string, int fallbackValue) {
        const char* cstr = string.c_str();
        char* endptr;
        int result = strtol(cstr, &endptr, 10);

        if (endptr == cstr || *endptr != '\0') return fallbackValue;
        if (result < INT_MIN || result > INT_MAX) return fallbackValue;

        return result;
    }

    float toFloat(const std::string& string, float fallbackValue) {
        const char* cstr = string.c_str();
        char* endptr;
        float result = strtof(cstr, &endptr);

        if (endptr == cstr || *endptr != '\0') return fallbackValue;
        if (result < -FLT_MAX || result > FLT_MAX) return fallbackValue;

        return result;
    }
}
