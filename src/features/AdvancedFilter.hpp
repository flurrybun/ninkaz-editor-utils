#pragma once

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
    bool isActive;
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
hsvValue& getFilterHSV(ColorType colorType);
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
    void onSelectAll(CCObject*);
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
