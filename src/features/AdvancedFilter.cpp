#include "AdvancedFilter.hpp"
#include "../misc/StringUtils.hpp"

void AFEditorUI::selectObjects(CCArray* objects, bool dontFilter) {
    if (dontFilter) {
        EditorUI::selectObjects(objects, dontFilter);
        return;
    }

    auto& filterValues = m_fields->filterValues;
    auto& filterZLayers = m_fields->filterZLayers;
    CCArrayExt<GameObject*> objectsToRemove;

    for (auto object : CCArrayExt<GameObject*>(objects)) {
        if (shouldFilterObject(object)) {
            objectsToRemove.push_back(object);
        }
    }

    for (auto object : objectsToRemove) {
        objects->removeObject(object);
    }

    EditorUI::selectObjects(objects, dontFilter);
}

bool AFEditorUI::canSelectObject(GameObject* object) {
    // the orig function doesn't use `this`, so the compiler passes garbage data into it. trying to use it results in a crash
    // https://discord.com/channels/911701438269386882/979402752121765898/1330700928537460818

    AFEditorUI* that = static_cast<AFEditorUI*>(EditorUI::get());

    if (object && that->shouldFilterObject(object)) return false;
    return EditorUI::canSelectObject(object);
}

bool AFEditorUI::shouldFilterObject(GameObject* object) {
    // not sure if this optimization is necessary but it can't hurt
    if (!m_fields->isFilterActive) return false;

    auto& values = m_fields->filterValues;
    auto& zLayers = m_fields->filterZLayers;
    auto& hsvs = m_fields->filterHSVs;

    // filter by group

    if (values[Filter::GROUP] != 0) {
        if (!object->m_groups || std::find(object->m_groups->begin(), object->m_groups->end(), values[Filter::GROUP]) == object->m_groups->end()) {
            return true;
        }
    }

    // filter by z order & z layer

    int zOrder = object->m_zOrder != 0 ? object->m_zOrder : object->m_defaultZOrder;
    ZLayer zLayer = object->m_zLayer != ZLayer::Default ? object->m_zLayer : object->m_defaultZLayer;

    if (values[Filter::ZORDER] != 0 && zOrder != values[Filter::ZORDER]) return true;
    if (!zLayers.empty() && !zLayers.contains(zLayer)) return true;

    // filter by color

    GJSpriteColor* baseColor = object->m_baseColor;
    GJSpriteColor* detailColor = object->m_detailColor;

    // for objects that have a detail color only, that color is stored in m_baseColor, NOT m_detailColor
    // m_customSpriteColor is only true if an object has a detail color only

    if (object->m_customSpriteColor) {
        baseColor = nullptr;
        detailColor = object->m_baseColor;
    }

    // in 2.2 you can change whether a single color sprite is base or detail, so we account for that here

    if (!object->m_detailColor) {
        switch (object->m_customColorType) {
            case 1: // base
                baseColor = object->m_baseColor;
                detailColor = nullptr;
                break;
            case 2: // detail
                baseColor = nullptr;
                detailColor = object->m_baseColor;
                break;
        }
    }

    auto checkColor = [](GJSpriteColor* color, int colorID, hsvValue& hsv, bool isBoth) {
        if (!color) {
            if (colorID == 0 && hsv == hsvValue(0, 1, 1)) return false;
            return true;
        }

        int objColorID = color->m_colorID != 0 ? color->m_colorID : color->m_defaultColorID;

        if (colorID != 0 && colorID != objColorID) return true;
        if (hsv.h != 0 && hsv.h != color->m_hsv.h) return true;
        if (hsv.s != 1 && hsv.s != color->m_hsv.s) return true;
        if (hsv.v != 1 && hsv.v != color->m_hsv.v) return true;
        return false;
    };

    if (checkColor(baseColor, values[Filter::BASECOLOR], hsvs[ColorType::BASE], false)) return true;
    if (checkColor(detailColor, values[Filter::DETAILCOLOR], hsvs[ColorType::DETAIL], false)) return true;
    if (checkColor(baseColor, values[Filter::COLOR], hsvs[ColorType::BOTH], true) &&
        checkColor(detailColor, values[Filter::COLOR], hsvs[ColorType::BOTH], true)) return true;

    // filter by scale

    // only 2 decimal places are visible to the player but gd can store any value, so we need to account for that
    const float tolerance = 0.01;

    if (values[Filter::SCALEX] != 0 && std::abs(object->m_scaleX - values[Filter::SCALEX]) > tolerance) return true;
    if (values[Filter::SCALEY] != 0 && std::abs(object->m_scaleY - values[Filter::SCALEY]) > tolerance) return true;
    if (values[Filter::SCALE] != 0) {
        if (std::abs(object->m_scaleX - values[Filter::SCALE]) > tolerance || 
            std::abs(object->m_scaleY - values[Filter::SCALE]) > tolerance) {
            return true;
        }
    }

    return false;
}

bool AFEditorUI::init(LevelEditorLayer* lel) {
    if (!EditorUI::init(lel)) return false;

    // modify ui

    auto filterMenu = querySelector("delete-category-menu > delete-filter-menu");

    filterMenu->removeChildByID("delete-filter-group-id");
    filterMenu->removeChildByID("delete-filter-color");
    filterMenu->removeChildByID("delete-reset-search");

    m_fields->activeSpr = createFilterSprite(true);
    m_fields->inactiveSpr = createFilterSprite(false);
    auto btn = CCMenuItemSpriteExtra::create(m_fields->inactiveSpr, this, menu_selector(AFEditorUI::onFilter));
    btn->setID("delete-filter-advanced"_spr);
    m_fields->filterBtn = btn;

    // i made a node ids pr that does this but i have little faith it'll get merged anytime soon,
    // so i'm just going to do it myself for the time being. no other mod uses this menu, it's fine

    if (auto layout = typeinfo_cast<AxisLayout*>(filterMenu->getLayout())) {
        layout->setCrossAxisAlignment(AxisAlignment::End);
    }

    filterMenu->insertAfter(btn, filterMenu->getChildByID("delete-find-group-id"));
    filterMenu->updateLayout();

    // reset group & color filters, since they're now inaccessible
    GameManager::get()->setIntGameVariable("0133", 0);
    GameManager::get()->setIntGameVariable("0139", 0);

    // load saved filter

    auto savedFilter = Mod::get()->getSavedValue<SavedFilter>("adv-filter");
    m_fields->filterValues = savedFilter.filterValues;
    m_fields->filterZLayers = savedFilter.filterZLayers;
    m_fields->filterHSVs = savedFilter.filterHSVs;

    onUpdateFilter();

    return true;
}

CCSprite* AFEditorUI::createFilterSprite(bool isActive) {
    auto topSpr = CCSprite::createWithSpriteFrameName("filter-btn.png"_spr);
    auto bgSpr = CCSprite::create(isActive ? "GJ_button_02.png" : "GJ_button_04.png");
    
    bgSpr->addChild(topSpr);
    topSpr->setPosition(bgSpr->getContentSize() / 2);
    return bgSpr;
}

void AFEditorUI::onFilter(CCObject* sender) {
    auto popup = AdvFilterPopup::create();
    popup->m_noElasticity = true;

    popup->show();
}

void AFEditorUI::onUpdateFilter() {
    bool hasFilter = std::any_of(m_fields->filterValues.begin(), m_fields->filterValues.end(), [](float value) {
        return value != 0;
    });

    hasFilter = hasFilter || !m_fields->filterZLayers.empty();

    hasFilter = hasFilter || std::any_of(m_fields->filterHSVs.begin(), m_fields->filterHSVs.end(), [](hsvValue hsv) {
        return hsv != hsvValue(0, 1, 1);
    });

    m_fields->filterBtn->setSprite(hasFilter ? m_fields->activeSpr : m_fields->inactiveSpr);
    m_fields->isFilterActive = hasFilter;

    SavedFilter filter;
    filter.filterValues = m_fields->filterValues;
    filter.filterZLayers = m_fields->filterZLayers;
    filter.filterHSVs = m_fields->filterHSVs;

    Mod::get()->setSavedValue("adv-filter", filter);
}


float getFilterValue(Filter filter) {
    return static_cast<AFEditorUI*>(EditorUI::get())->m_fields->filterValues[filter];
}

void setFilterValue(Filter filter, float value) {
    static_cast<AFEditorUI*>(EditorUI::get())->m_fields->filterValues[filter] = value;
}

std::set<ZLayer>& getFilterZLayers() {
    return static_cast<AFEditorUI*>(EditorUI::get())->m_fields->filterZLayers;
}

hsvValue getFilterHSV(ColorType colorType) {
    return static_cast<AFEditorUI*>(EditorUI::get())->m_fields->filterHSVs[colorType];
}

void setFilterHSV(ColorType colorType, hsvValue hsv) {
    static_cast<AFEditorUI*>(EditorUI::get())->m_fields->filterHSVs[colorType] = hsv;
}

std::string getColorName(int colorID) {
    switch (colorID) {
        case 1000: return "BG";
        case 1001: return "G1";
        case 1009: return "G2";
        case 1013: return "MG";
        case 1014: return "MG2";
        case 1002: return "Line";
        case 1003: return "3DL";
        case 1004: return "OBJ";
        case 1005: return "P1";
        case 1006: return "P2";
        case 1007: return "LBG";
        case 1010: return "Black";
        case 1011: return "White";
        case 1012: return "Lighter";
        default: return "?";
    }
}


bool AdvFilterPopup::setup() {
    setTitle("Advanced Filter");
    m_closeBtn->removeFromParent();

    auto infoText = "Create a <cy>delete filter</c> so only objects that <cg>match certain conditions</c> can be deleted.\n"
        "Scale and color can be <cl>combined or separated</c>. When combined, objects will pass the filter if they match <cp>either condition</c> (base or detail, scale x or y).\n"
        "To also filter objects on selection, enable <cr>select filter</c>.";

    auto infoBtn = InfoAlertButton::create("Help", infoText, 1);
    infoBtn->setID("info-button"_spr);
    m_buttonMenu->addChildAtPosition(infoBtn, Anchor::TopLeft, {18, -18});

    float yOffset = 26;
    float yPositions[] = { 36 + yOffset, 0 + yOffset, -36 + yOffset };
    float xLeft = -107;
    float xRight = 65;

    // VALUE CONTROLS

    addLine("Group:", "group-controls"_spr, Filter::GROUP, {xLeft, yPositions[0]});
    addLine("Z Order:", "z-order-controls"_spr, Filter::ZORDER, {xRight, yPositions[0]});
    addLine("Color:", "color-controls"_spr, Filter::COLOR, {xLeft, yPositions[1]});
    addLine("Color Base:", "base-color-controls"_spr, Filter::BASECOLOR, {xLeft, yPositions[1]});
    addLine("Detail:", "detail-color-controls"_spr, Filter::DETAILCOLOR, {xRight, yPositions[1]});
    addLine("Scale:", "scale-xy-controls"_spr, Filter::SCALE, {xLeft, yPositions[2]});
    addLine("Scale X:", "scale-x-controls"_spr, Filter::SCALEX, {xLeft, yPositions[2]});
    addLine("Scale Y:", "scale-y-controls"_spr, Filter::SCALEY, {xRight, yPositions[2]});

    auto colorToggler = createToggler("expand-btn.png"_spr, menu_selector(AdvFilterPopup::onToggleColor));
    colorToggler->setID("expand-color-btn"_spr);
    m_buttonMenu->addChildAtPosition(colorToggler, Anchor::Center, {16, yPositions[1]});
    m_colorToggler = colorToggler;

    auto scaleToggler = createToggler("expand-btn.png"_spr, menu_selector(AdvFilterPopup::onToggleScale));
    scaleToggler->setID("expand-scale-btn"_spr);
    m_buttonMenu->addChildAtPosition(scaleToggler, Anchor::Center, {-5, yPositions[2]});
    m_scaleToggler = scaleToggler;

    auto moreColorsToggler = createToggler("more-colors-btn.png"_spr, menu_selector(AdvFilterPopup::onMoreColors));
    moreColorsToggler->setID("more-colors-btn"_spr);
    moreColorsToggler->setTag(ColorType::BOTH);
    m_buttonMenu->addChildAtPosition(moreColorsToggler, Anchor::Center, {-5, yPositions[1]});

    auto moreBaseColorsToggler = createToggler("more-colors-btn.png"_spr, menu_selector(AdvFilterPopup::onMoreColors));
    moreBaseColorsToggler->setID("more-base-colors-btn"_spr);
    moreBaseColorsToggler->setTag(ColorType::BASE);
    m_buttonMenu->addChildAtPosition(moreBaseColorsToggler, Anchor::Center, {-5, yPositions[1]});

    auto moreDetailColorsToggler = createToggler("more-colors-btn.png"_spr, menu_selector(AdvFilterPopup::onMoreColors));
    moreDetailColorsToggler->setID("more-detail-colors-btn"_spr);
    moreDetailColorsToggler->setTag(ColorType::DETAIL);
    m_buttonMenu->addChildAtPosition(moreDetailColorsToggler, Anchor::Center, {167, yPositions[1]});

    m_moreColorBtns[ColorType::BOTH] = moreColorsToggler;
    m_moreColorBtns[ColorType::BASE] = moreBaseColorsToggler;
    m_moreColorBtns[ColorType::DETAIL] = moreDetailColorsToggler;

    moreBaseColorsToggler->setVisible(false);
    moreDetailColorsToggler->setVisible(false);

    // Z LAYER CONTROLS

    auto zLayerMenu = CCMenu::create();
    m_zLayerMenu = zLayerMenu;
    zLayerMenu->setContentWidth(350);
    zLayerMenu->setID("z-layer-controls"_spr);
    zLayerMenu->setLayout(
        RowLayout::create()
            ->setGap(3)
    );

    ZLayer zLayers[] = {ZLayer::B5, ZLayer::B4, ZLayer::B3, ZLayer::B2, ZLayer::B1, ZLayer::T1, ZLayer::T2, ZLayer::T3, ZLayer::T4};
    std::string zLayerStrings[] = {"B5", "B4", "B3", "B2", "B1", "T1", "T2", "T3", "T4"};

    for (int i = 0; i < 9; i++) {
        auto onSpr = ButtonSprite::create(zLayerStrings[i].c_str(), 220, false, "bigFont.fnt", "GJ_button_02.png", 25, 0.3);
        auto offSpr = ButtonSprite::create(zLayerStrings[i].c_str(), 220, false, "bigFont.fnt", "GJ_button_04.png", 25, 0.3);
        auto btn = CCMenuItemToggler::create(offSpr, onSpr, this, menu_selector(AdvFilterPopup::onZLayer));

        btn->setTag(static_cast<int>(zLayers[i]));
        btn->toggle(getFilterZLayers().contains(zLayers[i]));
        zLayerMenu->addChild(btn);
    }

    zLayerMenu->updateLayout();
    m_mainLayer->addChildAtPosition(zLayerMenu, Anchor::Center, {0, -52});

    // BOTTOM MENU 

    auto bottomMenu = CCMenu::create();
    bottomMenu->setContentWidth(200);
    bottomMenu->setID("bottom-menu"_spr);
    bottomMenu->setLayout(
        RowLayout::create()
            ->setGap(5)
    );

    auto okSpr = ButtonSprite::create("OK", 40, true, "goldFont.fnt", "GJ_button_01.png", 30, 0.8);
    auto okBtn = CCMenuItemSpriteExtra::create(okSpr, this, menu_selector(AdvFilterPopup::onClose));

    auto resetSpr = CCSprite::createWithSpriteFrameName("GJ_resetBtn_001.png");
    auto resetBtn = CCMenuItemSpriteExtra::create(resetSpr, this, menu_selector(AdvFilterPopup::onReset));
    resetBtn->setLayoutOptions(
        AxisLayoutOptions::create()
            ->setRelativeScale(1.1)
    );
    m_resetBtn = resetBtn;

    bottomMenu->addChild(okBtn);
    bottomMenu->addChild(resetBtn);
    bottomMenu->updateLayout();
    m_mainLayer->addChildAtPosition(bottomMenu, Anchor::Bottom, {0, 24});

    // UPDATE VALUES

    onUpdateValue();

    if (getFilterValue(Filter::SCALEX) != 0 || getFilterValue(Filter::SCALEY) != 0) {
        scaleToggler->toggle(true);
        onToggleScale(nullptr);
    }

    if (getFilterValue(Filter::BASECOLOR) != 0 || getFilterValue(Filter::DETAILCOLOR) != 0 ||
        getFilterHSV(ColorType::BASE) != hsvValue(0, 1, 1) || getFilterHSV(ColorType::DETAIL) != hsvValue(0, 1, 1)) {
        colorToggler->toggle(true);
        onToggleColor(nullptr);
    }

    return true;
}

void AdvFilterPopup::addLine(std::string labelText, std::string id, Filter filter, CCPoint position) {
    bool isDecimal = filter == Filter::SCALE || filter == Filter::SCALEX || filter == Filter::SCALEY;

    auto menu = CCMenu::create();
    menu->setScale(0.8);
    menu->setContentWidth(220);
    menu->setID(id);
    menu->setLayout(
        RowLayout::create()
            ->setGap(24)
            ->setAutoScale(false)
    );

    auto label = CCLabelBMFont::create(labelText.c_str(), "goldFont.fnt");
    label->setAnchorPoint({1, 0.5});
    label->setScale(0.65);
    label->setLayoutOptions(
        AxisLayoutOptions::create()
            ->setNextGap(10)
            ->setLength(100)
    );

    float value = getFilterValue(filter);
    std::string valueStr = value == 0 ? "" : strutils::toString(value, 2);

    if (value >= 1000 && (filter == Filter::COLOR || filter == Filter::BASECOLOR || filter == Filter::DETAILCOLOR)) {
        valueStr = getColorName(std::round(value));
    }

    auto input = TextInput::create(60, "NA");
    if (filter == Filter::ZORDER) input->setFilter("0123456789-");
    else if (!isDecimal) input->setFilter("0123456789");
    else input->setFilter("0123456789.");
    input->getInputNode()->m_numberInput = true;
    input->setString(valueStr.c_str());
    input->setTag(filter);
    input->setCallback([this, filter](const std::string& string) {
        setFilterValue(filter, strutils::toFloat(string));
        onUpdateValue();
    });
    input->setLayoutOptions(
        AxisLayoutOptions::create()
            ->setNextGap(10.f)
            ->setPrevGap(10.f)
    );

    auto decArrowSpr = CCSprite::createWithSpriteFrameName("edit_leftBtn_001.png");
    auto decArrowBtn = CCMenuItemSpriteExtra::create(
        decArrowSpr, this, menu_selector(AdvFilterPopup::onInputArrow)
    );
    decArrowBtn->setTag(filter * 2);

    auto incArrowSpr = CCSprite::createWithSpriteFrameName("edit_rightBtn_001.png");
    auto incArrowBtn = CCMenuItemSpriteExtra::create(
        incArrowSpr, this, menu_selector(AdvFilterPopup::onInputArrow)
    );
    incArrowBtn->setTag(filter * 2 + 1);

    menu->addChild(label);
    menu->addChild(decArrowBtn);
    menu->addChild(input);
    menu->addChild(incArrowBtn);
    menu->updateLayout();
    m_mainLayer->addChildAtPosition(menu, Anchor::Center, position);

    if (filter == Filter::SCALEX || filter == Filter::SCALEY) menu->setVisible(false);
    if (filter == Filter::BASECOLOR || filter == Filter::DETAILCOLOR) menu->setVisible(false);

    m_inputs[filter] = input;
    m_controlMenus[filter] = menu;
}

CCMenuItemToggler* AdvFilterPopup::createToggler(std::string spriteName, SEL_MenuHandler selector) {
    auto offSpr = CCSprite::create("GJ_button_01.png");
    auto offTop = CCSprite::createWithSpriteFrameName(spriteName.c_str());
    offSpr->addChildAtPosition(offTop, Anchor::Center);
    offSpr->setScale(0.45);
    offTop->setScale(1 / 0.45);

    auto onSpr = CCSprite::create("GJ_button_02.png");
    auto onTop = CCSprite::createWithSpriteFrameName(spriteName.c_str());
    onSpr->addChildAtPosition(onTop, Anchor::Center);
    onSpr->setScale(0.45);
    onTop->setScale(1 / 0.45);

    if (spriteName == "expand-btn.png"_spr) onTop->setFlipX(true);

    return CCMenuItemToggler::create(offSpr, onSpr, this, selector);
}

void AdvFilterPopup::onUpdateValue() {
    // update more color buttons

    for (int i = 0; i < 3; i++) {
        auto hsv = getFilterHSV(static_cast<ColorType>(i));
        auto btn = m_moreColorBtns[i];

        btn->toggle(hsv != hsvValue(0, 1, 1));
    }

    // enable reset button if anything needs to be reset
    
    bool enableReset = false;

    for (auto row : m_controlMenus) {
        auto input = row->getChildByType<TextInput>(0);
        if (input->getString() != "") enableReset = true;
    }

    for (auto moreColorBtn : m_moreColorBtns) {
        if (moreColorBtn->isToggled()) enableReset = true;
    }

    if (!getFilterZLayers().empty()) enableReset = true;

    m_resetBtn->setEnabled(enableReset);
    m_resetBtn->setOpacity(enableReset ? 255 : 127);

    // update filter button

    static_cast<AFEditorUI*>(EditorUI::get())->onUpdateFilter();
}

void AdvFilterPopup::onInputArrow(CCObject* sender) {
    Filter filter = static_cast<Filter>(sender->getTag() / 2);
    bool isIncrement = sender->getTag() % 2 == 1;
    float newValue = getFilterValue(filter) + (isIncrement ? 1 : -1);

    if (filter == Filter::GROUP || filter == Filter::COLOR || filter == Filter::ZORDER) {
        if (newValue < 0) return;
    }

    std::string string = strutils::toString(newValue, 2);
    m_inputs[filter]->setString(string.c_str(), true);
}

void AdvFilterPopup::onZLayer(CCObject* sender) {
    ZLayer zLayer = static_cast<ZLayer>(sender->getTag());
    bool isActive = static_cast<CCMenuItemToggler*>(sender)->isToggled();

    if (isActive) getFilterZLayers().erase(zLayer);
    else getFilterZLayers().insert(zLayer);

    onUpdateValue();
}

void AdvFilterPopup::onMoreColors(CCObject* sender) {
    auto toggler = static_cast<CCMenuItemToggler*>(sender);
    toggler->toggle(!toggler->isToggled());

    ColorType colorType = static_cast<ColorType>(sender->getTag());

    updateCallback onUpdate = [this, colorType](int colorID, std::string colorName, hsvValue hsv) {
        Filter filter = static_cast<Filter>(static_cast<int>(colorType) + 1);

        setFilterHSV(colorType, hsv);

        if (colorID != 0) {
            setFilterValue(filter, colorID);
            m_inputs[filter]->setString(colorName);
        }

        onUpdateValue();
    };

    auto popup = MoreColorsPopup::create(colorType, onUpdate);
    popup->m_noElasticity = true;

    popup->show();
}

void AdvFilterPopup::onToggleColor(CCObject* sender) {
    m_isToggleColor = !m_isToggleColor;

    float xOffset = (m_isToggleColor ? 167 : -5) + 21;
    float yOffset = static_cast<AnchorLayoutOptions*>(m_colorToggler->getLayoutOptions())->getOffset().y;
    m_colorToggler->updateAnchoredPosition(Anchor::Center, {xOffset, yOffset});

    m_controlMenus[Filter::COLOR]->setVisible(!m_isToggleColor);
    m_controlMenus[Filter::BASECOLOR]->setVisible(m_isToggleColor);
    m_controlMenus[Filter::DETAILCOLOR]->setVisible(m_isToggleColor);

    m_moreColorBtns[ColorType::BOTH]->setVisible(!m_isToggleColor);
    m_moreColorBtns[ColorType::BASE]->setVisible(m_isToggleColor);
    m_moreColorBtns[ColorType::DETAIL]->setVisible(m_isToggleColor);

    if (m_isToggleColor) {
        m_inputs[Filter::COLOR]->setString("", true);
        setFilterHSV(ColorType::BOTH, {0, 1, 1});
    } else {
        m_inputs[Filter::BASECOLOR]->setString("", true);
        m_inputs[Filter::DETAILCOLOR]->setString("", true);
        setFilterHSV(ColorType::BASE, {0, 1, 1});
        setFilterHSV(ColorType::DETAIL, {0, 1, 1});
    }

    onUpdateValue();
}

void AdvFilterPopup::onToggleScale(CCObject* sender) {
    m_isToggleScale = !m_isToggleScale;

    float xOffset = m_isToggleScale ? 167 : -5;
    float yOffset = static_cast<AnchorLayoutOptions*>(m_scaleToggler->getLayoutOptions())->getOffset().y;
    m_scaleToggler->updateAnchoredPosition(Anchor::Center, {xOffset, yOffset});

    m_controlMenus[Filter::SCALE]->setVisible(!m_isToggleScale);
    m_controlMenus[Filter::SCALEX]->setVisible(m_isToggleScale);
    m_controlMenus[Filter::SCALEY]->setVisible(m_isToggleScale);

    if (m_isToggleScale) {
        m_inputs[Filter::SCALE]->setString("", true);
    } else {
        m_inputs[Filter::SCALEX]->setString("", true);
        m_inputs[Filter::SCALEY]->setString("", true);
    }

    onUpdateValue();
}

void AdvFilterPopup::onReset(CCObject* sender) {
    // resetting values

    for (int i = 0; i < 8; i++) {
        setFilterValue(static_cast<Filter>(i), 0);
    }

    for (int i = 0; i < 3; i++) {
        setFilterHSV(static_cast<ColorType>(i), {0, 1, 1});
    }

    getFilterZLayers().clear();

    // resetting ui

    for (auto input : m_inputs) {
        input->setString("");
    }

    for (auto zLayer : CCArrayExt<CCMenuItemToggler*>(m_zLayerMenu->getChildren())) {
        zLayer->toggle(false);
    }

    if (m_isToggleColor) {
        m_colorToggler->toggle(false);
        onToggleColor(nullptr);
    }

    if (m_isToggleScale) {
        m_scaleToggler->toggle(false);
        onToggleScale(nullptr);
    }

    onUpdateValue();
}

AdvFilterPopup* AdvFilterPopup::create() {
    auto ret = new AdvFilterPopup();
    if (ret && ret->initAnchored(420, 265)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}


bool MoreColorsPopup::setup(ColorType colorType, updateCallback callback) {
    if (colorType == ColorType::BASE) setTitle("Base Color Special");
    else if (colorType == ColorType::DETAIL) setTitle("Detail Color Special");
    else setTitle("Color Special");
    m_closeBtn->removeFromParent();

    Filter filter = static_cast<Filter>(static_cast<int>(colorType) + 1);

    m_colorType = colorType;
    m_callback = callback;
    m_hsv = getFilterHSV(colorType);

    // COLOR BUTTONS

    std::vector<int> colorIDs = {1004, 1003, 1005, 1006, 1007, 1010, 1011, 1012};

    auto colorMenu = CCMenu::create();
    colorMenu->setID("special-color-menu"_spr);
    colorMenu->setContentWidth(300);
    colorMenu->setLayout(
        RowLayout::create()
            ->setGap(15)
            ->setGrowCrossAxis(true)
    );

    for (int i = 0; i < colorIDs.size(); i++) {
        std::string name = getColorName(colorIDs[i]);
        auto spr = ButtonSprite::create(name.c_str(), 40, true, "bigFont.fnt", "GJ_button_04.png", 30, 0.4);
        auto btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(MoreColorsPopup::onColor));

        if (colorIDs[i] == getFilterValue(filter)) spr->updateBGImage("GJ_button_02.png");

        m_colorButtonSprites[i] = spr;
        btn->setTag(colorIDs[i]);
        colorMenu->addChild(btn);
    }

    colorMenu->updateLayout();
    m_mainLayer->addChildAtPosition(colorMenu, Anchor::Center, {0, 30});

    // HSV INPUTS

    auto hsvMenu = CCMenu::create();
    hsvMenu->setID("hsv-controls"_spr);
    hsvMenu->setContentWidth(380);
    hsvMenu->setScale(0.8);
    hsvMenu->setLayout(
        RowLayout::create()
            ->setGap(10)
            ->setAutoScale(false)
    );

    const char* labels[] = {"Hue:", "Sat:", "Val:"};

    for (size_t i = 0; i < 3; i++) {
        auto label = CCLabelBMFont::create(labels[i], "goldFont.fnt");
        label->setAnchorPoint({1, 0.5});
        label->setScale(0.65);
        label->setLayoutOptions(
            AxisLayoutOptions::create()
                ->setPrevGap(20)
        );

        bool isDecimal = i != 0;
        float value = i == 0 ? m_hsv.h : i == 1 ? m_hsv.s : m_hsv.v;
        std::string valueStr = strutils::toString(value, 2);
        if (i == 0 && value == 0) valueStr = "";
        if (i != 0 && value == 1) valueStr = "";

        auto input = TextInput::create(60, "NA");
        if (isDecimal) input->setFilter("0123456789.-");
        else input->setFilter("0123456789-");
        input->getInputNode()->m_numberInput = true;
        input->setString(valueStr.c_str());
        input->setTag(i);
        input->setCallback([this, i](const std::string& string) {
            if (i == 0) m_hsv.h = strutils::toFloat(string);
            else if (i == 1) m_hsv.s = strutils::toFloat(string);
            else m_hsv.v = strutils::toFloat(string);

            onUpdateValue();
        });

        hsvMenu->addChild(label);
        hsvMenu->addChild(input);
    }

    hsvMenu->updateLayout();
    m_mainLayer->addChildAtPosition(hsvMenu, Anchor::Center, {0, -43});

    // OK BUTTON

    auto okSpr = ButtonSprite::create("OK", 40, true, "goldFont.fnt", "GJ_button_01.png", 30, 0.8);
    auto okBtn = CCMenuItemSpriteExtra::create(okSpr, this, menu_selector(MoreColorsPopup::onClose));
    m_buttonMenu->addChildAtPosition(okBtn, Anchor::Bottom, {0, 24});

    return true;
}

void MoreColorsPopup::onColor(CCObject* sender) {
    int colorID = sender->getTag();
    m_selectedColorID = colorID;

    for (auto spr : m_colorButtonSprites) {
        spr->updateBGImage("GJ_button_04.png");
    }

    auto spr = static_cast<CCNode*>(sender)->getChildByType<ButtonSprite>(0);
    spr->updateBGImage("GJ_button_02.png");

    onUpdateValue();
}

void MoreColorsPopup::onUpdateValue() {
    m_callback(m_selectedColorID, getColorName(m_selectedColorID), m_hsv);
}

MoreColorsPopup* MoreColorsPopup::create(ColorType colorType, updateCallback callback) {
    auto ret = new MoreColorsPopup();
    if (ret && ret->initAnchored(360, 230, colorType, callback)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}
