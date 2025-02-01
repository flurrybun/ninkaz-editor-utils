#pragma once

#include <Geode/modify/EditorUI.hpp>
#include <Geode/Geode.hpp>
using namespace geode::prelude;

struct hsvValue {
    float h, s, v;

    hsvValue(float h, float s, float v) : h(h), s(s), v(v) {}

    bool operator==(const hsvValue& other) const {
        return h == other.h && s == other.s && v == other.v;
    }
};

struct SavedFilter {
    std::array<float, 8> filterValues;
    std::set<ZLayer> filterZLayers;
    std::array<hsvValue, 3> filterHSVs = {hsvValue(0, 1, 1), hsvValue(0, 1, 1), hsvValue(0, 1, 1)};
};

template <>
struct matjson::Serialize<SavedFilter> {
    static matjson::Value toJson(SavedFilter const& filter) {
        matjson::Value obj;

        obj["filter-values"] = matjson::Value::array();
        for (auto value : filter.filterValues) {
            obj["filter-values"].asArray().unwrap().push_back(value);
        }

        obj["filter-zlayers"] = matjson::Value::array();
        for (auto zLayer : filter.filterZLayers) {
            obj["filter-zlayers"].asArray().unwrap().push_back(static_cast<int>(zLayer));
        }

        obj["filter-hsvs"] = matjson::Value::array();
        for (auto hsv : filter.filterHSVs) {
            std::vector values = {hsv.h, hsv.s, hsv.v};
            obj["filter-hsvs"].asArray().unwrap().push_back(values);
        }

        return obj;
	}

    static Result<SavedFilter> fromJson(matjson::Value const& value) {
        SavedFilter filter;

        GEODE_UNWRAP_INTO(auto filterValues, value["filter-values"].asArray());
        for (int i = 0; i < filterValues.size(); i++) {
            filter.filterValues[i] = GEODE_UNWRAP(filterValues[i].asDouble());
        }

        GEODE_UNWRAP_INTO(auto filterZLayers, value["filter-zlayers"].asArray());
        for (auto zLayer : filterZLayers) {
            filter.filterZLayers.insert(static_cast<ZLayer>(GEODE_UNWRAP(zLayer.asInt())));
        }

        GEODE_UNWRAP_INTO(auto filterHSVs, value["filter-hsvs"].asArray());
        for (int i = 0; i < filterHSVs.size(); i++) {
            auto hsv = GEODE_UNWRAP(filterHSVs[i].asArray());

            float h = GEODE_UNWRAP(hsv[0].asDouble());
            float s = GEODE_UNWRAP(hsv[1].asDouble());
            float v = GEODE_UNWRAP(hsv[2].asDouble());
            filter.filterHSVs[i] = hsvValue(h, s, v);
        }

        return Ok(filter);
	}
};

class $modify(AFEditorUI, EditorUI) {
    struct Fields {
        bool isFilterActive = false;
        std::array<float, 8> filterValues;
        std::set<ZLayer> filterZLayers;
        std::array<hsvValue, 3> filterHSVs = {hsvValue(0, 1, 1), hsvValue(0, 1, 1), hsvValue(0, 1, 1)};

        Ref<CCSprite> activeSpr;
        Ref<CCSprite> inactiveSpr;
        Ref<CCMenuItemSpriteExtra> filterBtn;
    };
    
    $override void selectObjects(CCArray*, bool);
    $override bool canSelectObject(GameObject*);
    bool shouldFilterObject(GameObject*);
    $override bool init(LevelEditorLayer*);
    CCSprite* createFilterSprite(bool);
    void onFilter(CCObject*);
    void onUpdateFilter();
};

enum Filter {
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

float getFilterValue(Filter filter);
void setFilterValue(Filter filter, float value);
std::set<ZLayer>& getFilterZLayers();
hsvValue getFilterHSV(ColorType colorType);
void setFilterHSV(ColorType colorType, hsvValue hsv);

std::string getColorName(int);

class AdvFilterPopup : public Popup<> {
protected:
    std::array<CCMenu*, 8> m_controlMenus;
    std::array<TextInput*, 8> m_inputs;
    CCMenu* m_zLayerMenu;
    CCMenuItemSpriteExtra* m_resetBtn;
    CCMenuItemToggler* m_colorToggler;
    std::array<CCMenuItemToggler*, 3> m_moreColorBtns;
    CCMenuItemToggler* m_scaleToggler;
    bool m_isToggleColor;
    bool m_isToggleScale;

    bool setup() override;
    void addLine(std::string label, std::string id, Filter filter, CCPoint position);
    CCMenuItemToggler* createToggler(std::string spriteName, SEL_MenuHandler selector);

    void onUpdateValue();
    void onInputArrow(CCObject*);
    void onZLayer(CCObject*);
    void onMoreColors(CCObject*);
    void onToggleColor(CCObject*);
    void onToggleScale(CCObject*);
    void onReset(CCObject*);
public:
    static AdvFilterPopup* create();
};

using updateCallback = std::function<void(int colorID, std::string colorName, hsvValue hsv)>;

class MoreColorsPopup : public Popup<ColorType, updateCallback> {
protected:
    ColorType m_colorType;
    updateCallback m_callback;
    int m_selectedColorID = 0;
    hsvValue m_hsv = {0, 1, 1};
    std::array<ButtonSprite*, 8> m_colorButtonSprites;

    bool setup(ColorType, updateCallback) override;
    void onColor(CCObject*);
    void onUpdateValue();
public:
    static MoreColorsPopup* create(ColorType, updateCallback);
};
