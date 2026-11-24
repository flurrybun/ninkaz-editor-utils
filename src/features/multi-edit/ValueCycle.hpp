#pragma once

#include <Geode/Result.hpp>
#include <Geode/utils/string.hpp>
using namespace geode::prelude;

template <typename T>
class ValueCycle {
    std::string m_input;
    double m_step = 1.0;
    std::vector<T> m_values;

    static size_t decimalPlaces(const std::string& str) {
        size_t dot = str.find('.');
        return dot == std::string::npos ? 0 : str.size() - dot - 1;
    }

    void addSegment(const std::string& segment, double step) {
        size_t dashPos = segment.find('-');
        if (dashPos == 0) dashPos = segment.find('-', 1);

        bool isRange = dashPos != std::string::npos && dashPos > 0;

        if (!isRange) {
            Result<T> valResult = numFromString<T>(segment);
            if (!valResult || !std::isfinite(static_cast<double>(valResult.unwrap()))) return;

            m_values.push_back(valResult.unwrap());
            return;
        }

        std::string minStr = segment.substr(0, dashPos);
        std::string maxStr = segment.substr(dashPos + 1);
        string::trimIP(minStr, " ");
        string::trimIP(maxStr, " ");

        Result<T> minResult = numFromString<T>(minStr);
        Result<T> maxResult = numFromString<T>(maxStr);
        if (!minResult || !maxResult) return;

        double start = static_cast<double>(minResult.unwrap());
        double end = static_cast<double>(maxResult.unwrap());
        if (!std::isfinite(start) || !std::isfinite(end)) return;

        step = std::fabs(step);
        if (step == 0.0) step = 1.0;

        size_t places = std::max(decimalPlaces(minStr), decimalPlaces(maxStr));
        double scale = std::pow(10.0, static_cast<double>(places));
        start = std::round(start * scale) / scale;
        end = std::round(end * scale) / scale;

        double sign = end >= start ? 1.0 : -1.0;
        double steps = std::floor(std::fabs(end - start) / step + 1e-9) + 1.0;
        if (!std::isfinite(steps) || steps > 100000.0) return;
        size_t count = static_cast<size_t>(steps);

        for (size_t i = 0; i < count; i++) {
            m_values.push_back(static_cast<T>(start + sign * step * static_cast<double>(i)));
        }
    }

public:
    void set(const std::string& input, double step = 1.0) {
        if (input == m_input && step == m_step) return;

        m_input = input;
        m_step = step;
        m_values.clear();

        std::stringstream ss(input);
        std::string segment;

        while (std::getline(ss, segment, ',')) {
            string::trimIP(segment, " ");

            if (segment.empty()) continue;

            addSegment(segment, step);
        }
    }

    const std::vector<T>& values() const { return m_values; }
};
