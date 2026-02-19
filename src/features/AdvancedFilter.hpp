#pragma once

#include "../misc/SpriteColor.hpp"

#include <Geode/Geode.hpp>
using namespace geode::prelude;

struct HSVValue {
    float h, s, v;

    HSVValue(float h, float s, float v) : h(h), s(s), v(v) {}
    HSVValue(const ccHSVValue& hsv) : h(hsv.h), s(hsv.s), v(hsv.v) {}
};

struct HSVInput {
    std::string h, s, v;

    HSVInput() : h(""), s(""), v("") {}
    HSVInput(std::string h, std::string s, std::string v) : h(h), s(s), v(v) {}
};

template <typename T>
struct NumberParser {
    Result<T> operator()(const std::string& str) const {
        return numFromString<T>(str);
    }
};

struct ColorParser {
    Result<int> operator()(const std::string& str) const {
        return nk::colorIDFromString(str);
    }
};

template <typename T, typename Parser = NumberParser<T>>
class FilterValue {
    std::vector<std::pair<T, T>> m_ranges;
    std::string m_input;
    Parser m_parser;

public:
    FilterValue() = default;
    FilterValue(const FilterValue&) = delete;
    FilterValue& operator=(const FilterValue&) = delete;

    void set(const std::string& input) {
        m_ranges.clear();
        m_input = input;

        if (input.empty()) return;

        std::stringstream ss(input);
        std::string segment;

        while (std::getline(ss, segment, ',')) {
            string::trimIP(segment, " ");

            if (segment.empty()) continue;

            size_t dashPos = segment.find('-');
            if (dashPos == 0) dashPos = segment.find('-', 1);

            bool isRange = dashPos != std::string::npos && dashPos > 0;

            if (isRange) {
                std::string minStr = segment.substr(0, dashPos);
                std::string maxStr = segment.substr(dashPos + 1);

                Result<T> minResult = m_parser(minStr);
                Result<T> maxResult = m_parser(maxStr);
                if (!minResult || !maxResult) continue;

                T minVal = minResult.unwrap();
                T maxVal = maxResult.unwrap();

                if (minVal > maxVal) {
                    std::swap(minVal, maxVal);
                }

                m_ranges.push_back({minVal, maxVal});
            } else {
                Result<T> valResult = m_parser(segment);
                if (!valResult) continue;

                T val = valResult.unwrap();

                m_ranges.push_back({val, val});
            }
        }
    }

    const std::string& getInput() const {
        return m_input;
    }

    std::optional<T> getSingleValue() const {
        if (m_ranges.size() == 0) return 0;
        if (m_ranges.size() != 1) return std::nullopt;

        const auto& [min, max] = m_ranges[0];

        if (min != max) return std::nullopt;
        else return min;
    }

    bool isActive() const {
        return !m_ranges.empty();
    }

    bool contains(T value, T tolerance = 0) const {
        if (!isActive()) return true;

        for (const auto& [min, max] : m_ranges) {
            if (value + tolerance >= min && value - tolerance <= max) {
                return true;
            }
        }

        return false;
    }

    bool hasSingleValue(T value) const {
        for (const auto& [min, max] : m_ranges) {
            if (min == max && min == value) {
                return true;
            }
        }

        return false;
    }

    void reset() {
        m_ranges.clear();
        m_input.clear();
    }
};

class FilterHSV {
    FilterValue<float> m_h;
    FilterValue<float> m_s;
    FilterValue<float> m_v;

public:
    FilterHSV() = default;
    FilterHSV(const FilterHSV&) = delete;
    FilterHSV& operator=(const FilterHSV&) = delete;

    void set(const HSVInput& hsv) {
        m_h.set(hsv.h);
        m_s.set(hsv.s);
        m_v.set(hsv.v);
    }

    const HSVInput getInput() const {
        return HSVInput(
            m_h.getInput(),
            m_s.getInput(),
            m_v.getInput()
        );
    }

    bool isActive() const {
        return m_h.isActive() || m_s.isActive() || m_v.isActive();
    }

    bool contains(const HSVValue& hsv) const {
        return m_h.contains(hsv.h) && m_s.contains(hsv.s) && m_v.contains(hsv.v);
    }

    void reset() {
        m_h.reset();
        m_s.reset();
        m_v.reset();
    }
};

template <typename T, typename Parser>
class SplitFilterValue {
    bool m_isSplit = false;
    FilterValue<T, Parser> m_first;
    FilterValue<T, Parser> m_second;

public:
    SplitFilterValue() = default;
    SplitFilterValue(const SplitFilterValue&) = delete;
    SplitFilterValue& operator=(const SplitFilterValue&) = delete;

    void setSplit(bool split) {
        m_isSplit = split;
        m_first.reset();
        m_second.reset();
    }

    bool isSplit() const {
        return m_isSplit;
    }

    void setFirst(const std::string& input) {
        m_first.set(input);
    }

    void setSecond(const std::string& input) {
        m_second.set(input);
    }

    const FilterValue<T, Parser>& getFirst() const {
        return m_first;
    }

    const FilterValue<T, Parser>& getSecond() const {
        return m_second;
    }

    bool isActive() const {
        if (m_isSplit) {
            return m_first.isActive() || m_second.isActive();
        } else {
            return m_first.isActive();
        }
    }

    bool contains(T first, T second, T tolerance = 0) const {
        if (m_isSplit) {
            return m_first.contains(first, tolerance) && m_second.contains(second, tolerance);
        } else {
            return m_first.contains(first, tolerance) || m_first.contains(second, tolerance);
        }
    }

    void reset() {
        m_first.reset();
        m_second.reset();
        m_isSplit = false;
    }
};

template <typename T>
class FilterSet {
    std::unordered_set<T> m_values;

public:
    FilterSet() = default;
    FilterSet(const FilterSet&) = delete;
    FilterSet& operator=(const FilterSet&) = delete;

    void insert(T value) {
        m_values.insert(value);
    }

    void erase(T value) {
        m_values.erase(value);
    }

    const std::unordered_set<T>& getValues() const {
        return m_values;
    }

    bool isActive() const {
        return !m_values.empty();
    }

    bool contains(T value) const {
        if (!isActive()) return true;
        return m_values.find(value) != m_values.end();
    }

    void reset() {
        m_values.clear();
    }
};

class ColorFilter {
    SplitFilterValue<int, ColorParser> m_color;
    FilterHSV m_baseHSV;
    FilterHSV m_detailHSV;

public:
    ColorFilter() = default;
    ColorFilter(const ColorFilter&) = delete;
    ColorFilter& operator=(const ColorFilter&) = delete;

    void setSplit(bool split) {
        m_color.setSplit(split);
        m_baseHSV.reset();
        m_detailHSV.reset();
    }

    bool isSplit() const {
        return m_color.isSplit();
    }

    void setBaseColor(const std::string& input) {
        m_color.setFirst(input);
    }

    void setDetailColor(const std::string& input) {
        m_color.setSecond(input);
    }

    const FilterValue<int, ColorParser>& getBaseColor() const {
        return m_color.getFirst();
    }

    const FilterValue<int, ColorParser>& getDetailColor() const {
        return m_color.getSecond();
    }

    FilterHSV& getBaseHSV() {
        return m_baseHSV;
    }

    FilterHSV& getDetailHSV() {
        return m_detailHSV;
    }

    bool isActive() const {
        if (isSplit()) {
            return m_color.isActive() || m_baseHSV.isActive() || m_detailHSV.isActive();
        } else {
            return m_color.isActive() || m_baseHSV.isActive();
        }
    }

    bool contains(
        int baseColor, const std::optional<HSVValue>& baseHSV,
        int detailColor, const std::optional<HSVValue>& detailHSV
    ) const {
        if (!m_color.contains(baseColor, detailColor)) return false;

        if (isSplit()) {
            bool baseOk = !baseHSV.has_value() || m_baseHSV.contains(*baseHSV);
            bool detailOk = !detailHSV.has_value() || m_detailHSV.contains(*detailHSV);
            return baseOk && detailOk;
        } else {
            bool baseOk = baseHSV.has_value() && m_baseHSV.contains(*baseHSV);
            bool detailOk = detailHSV.has_value() && m_baseHSV.contains(*detailHSV);
            return baseOk || detailOk;
        }
    }

    void reset() {
        m_color.reset();
        m_baseHSV.reset();
        m_detailHSV.reset();
    }
};

struct Filter {
    Filter() = default;
    Filter(const Filter&) = delete;
    Filter& operator=(const Filter&) = delete;

    FilterValue<short> m_group;
    ColorFilter m_color;
    SplitFilterValue<float, NumberParser<float>> m_scale;
    FilterValue<int> m_zOrder;
    FilterSet<ZLayer> m_zLayer;

    bool isActive() const {
        return m_group.isActive() ||
            m_color.isActive() ||
            m_scale.isActive() ||
            m_zOrder.isActive() ||
            m_zLayer.isActive();
    }

    void reset() {
        m_group.reset();
        m_color.reset();
        m_scale.reset();
        m_zOrder.reset();
        m_zLayer.reset();
    }
};

enum FilterType {
    GROUP = 0,
    COLOR = 1,
    BASECOLOR = 2,
    DETAILCOLOR = 3,
    SCALE = 4,
    SCALEX = 5,
    SCALEY = 6,
    ZORDER = 7
};

enum ColorType {
    BOTH = 0,
    BASE = 1,
    DETAIL = 2
};

void setFilter(FilterType type, std::string string);
const std::string& getFilterInput(FilterType type);
std::optional<float> getSingleValue(FilterType type);

const FilterValue<int, ColorParser>& getColorFilter(ColorType type);
FilterHSV& getFilterHSV(ColorType type);

bool isScale(FilterType type);
bool isColor(FilterType type);
bool isSplit(FilterType type);

class AdvFilterPopup : public Popup {
protected:
    std::array<CCMenu*, 8> m_controlMenus;
    std::array<TextInput*, 8> m_inputs;
    std::array<std::array<CCMenuItemSpriteExtra*, 2>, 8> m_arrowButtons;
    CCMenu* m_zLayerMenu;
    CCMenuItemSpriteExtra* m_resetBtn;
    CCMenuItemToggler* m_colorToggler;
    std::array<CCMenuItemToggler*, 3> m_moreColorBtns;
    CCMenuItemToggler* m_scaleToggler;
    bool m_isToggleColor;
    bool m_isToggleScale;

    bool init() override;
    void addLine(const std::string& label, const std::string& id, FilterType filter, CCPoint position);
    CCMenuItemToggler* createToggler(const std::string& spriteName, SEL_MenuHandler selector);

    void onUpdateValue();
    void onInputArrow(CCObject*);
    void onZLayer(CCObject*);
    void onMoreColors(CCObject*);
    void onToggleColor(CCObject*);
    void onToggleScale(CCObject*);
    void onReset(CCObject*);
    void onSelectAll(CCObject*);
public:
    static AdvFilterPopup* create() {
        auto popup = new AdvFilterPopup;
        if (popup->init()) {
            popup->autorelease();
            return popup;
        }
        delete popup;
        return nullptr;
    }
};

using updateCallback = geode::Function<void(const std::unordered_set<int>& colors, const HSVInput& hsv)>;

class MoreColorsPopup : public Popup {
protected:
    ColorType m_colorType;
    updateCallback m_callback;
    std::unordered_set<int> m_selectedColors;
    HSVInput m_hsv;

    bool init(ColorType, updateCallback);
    void onColor(CCObject*);
    void onUpdateValue();
public:
    static MoreColorsPopup* create(ColorType colorType, updateCallback callback) {
        auto popup = new MoreColorsPopup;
        if (popup->init(colorType, std::move(callback))) {
            popup->autorelease();
            return popup;
        }
        delete popup;
        return nullptr;
    }
};
