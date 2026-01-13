#include <Geode/modify/EditorUI.hpp>
#include "AdvancedFilter.hpp"
#include "../misc/StringUtils.hpp"

Filter s_filter;

class $modify(AFEditorUI, EditorUI) {
    struct Fields {
        Ref<CCSprite> activeSpr;
        Ref<CCSprite> inactiveSpr;
        Ref<CCMenuItemSpriteExtra> filterBtn;
    };

    $override
    bool init(LevelEditorLayer* lel) {
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

        filterMenu->insertAfter(btn, filterMenu->getChildByID("delete-find-group-id"));
        filterMenu->updateLayout();

        // reset group & color filters, since they're now inaccessible

        GameManager::get()->setIntGameVariable("0133", 0);
        GameManager::get()->setIntGameVariable("0139", 0);

        onUpdateFilter();

        return true;
    }
    
    $override
    void selectObjects(CCArray* objects, bool dontFilter) {
        if (dontFilter) {
            EditorUI::selectObjects(objects, dontFilter);
            return;
        }

        for (size_t i = objects->count(); i-- > 0;) {
            auto object = static_cast<GameObject*>(objects->objectAtIndex(i));
            if (shouldFilterObject(object)) {
                objects->removeObjectAtIndex(i);
            }
        }

        EditorUI::selectObjects(objects, dontFilter);
    }

    $override
    bool canSelectObject(GameObject* object) {
        // the orig function doesn't use `this`, so the compiler passes garbage data into it. trying to use it results in a crash
        // https://discord.com/channels/911701438269386882/979402752121765898/1330700928537460818

        AFEditorUI* that = static_cast<AFEditorUI*>(EditorUI::get());

        if (object && that->shouldFilterObject(object)) return false;
        return EditorUI::canSelectObject(object);
    }

    bool shouldFilterObject(GameObject* object) {
        if (s_filter.m_group.isActive()) {
            bool hasGroup = false;

            for (short i = 0; i < object->m_groupCount; i++) {
                short group = object->m_groups->at(i);

                if (s_filter.m_group.contains(group)) {
                    hasGroup = true;
                    break;
                }
            }

            if (!hasGroup || object->m_groupCount == 0) {
                return true;
            }
        }

        int zOrder = object->m_zOrder != 0 ? object->m_zOrder : object->m_defaultZOrder;
        ZLayer zLayer = object->m_zLayer != ZLayer::Default ? object->m_zLayer : object->m_defaultZLayer;

        if (!s_filter.m_zOrder.contains(zOrder)) return true;
        if (!s_filter.m_zLayer.contains(zLayer)) return true;

        if (s_filter.m_color.isActive()) {
            GJSpriteColor* baseColor = nk::getBaseSpriteColor(object);
            GJSpriteColor* detailColor = nk::getDetailSpriteColor(object);

            int baseColorID = baseColor ? (baseColor->m_colorID != 0 ? baseColor->m_colorID : baseColor->m_defaultColorID) : 0;
            int detailColorID = detailColor ? (detailColor->m_colorID != 0 ? detailColor->m_colorID : detailColor->m_defaultColorID) : 0;

            auto baseHSV = baseColor ? baseColor->m_hsv : std::optional<HSVValue>();
            auto detailHSV = detailColor ? detailColor->m_hsv : std::optional<HSVValue>();

            if (!s_filter.m_color.contains(baseColorID, baseHSV, detailColorID, detailHSV)) return true;
        }

        // only 2 decimal places are visible to the player but gd can store any value
        // a small tolerance is used to account for this

        constexpr float tolerance = 0.01f - std::numeric_limits<float>::epsilon();

        if (!s_filter.m_scale.contains(object->m_scaleX, object->m_scaleY, tolerance)) return true;

        return false;
    }

    CCSprite* createFilterSprite(bool isActive) {
        auto topSpr = CCSprite::createWithSpriteFrameName("filter-btn.png"_spr);
        auto bgSpr = CCSprite::create(isActive ? "GJ_button_02.png" : "GJ_button_04.png");
        
        bgSpr->addChild(topSpr);
        topSpr->setPosition(bgSpr->getContentSize() / 2);
        return bgSpr;
    }

    void onFilter(CCObject* sender) {
        auto popup = AdvFilterPopup::create();
        popup->m_noElasticity = true;

        popup->show();
    }

    void onUpdateFilter() {
        m_fields->filterBtn->setSprite(s_filter.isActive() ? m_fields->activeSpr : m_fields->inactiveSpr);
    }
};

void setFilter(FilterType type, std::string string) {
    switch (type) {
        case FilterType::GROUP:
            s_filter.m_group.set(string);
            break;
        case FilterType::COLOR:
        case FilterType::BASECOLOR:
            s_filter.m_color.setBaseColor(string);
            break;
        case FilterType::DETAILCOLOR:
            s_filter.m_color.setDetailColor(string);
            break;
        case FilterType::SCALE:
        case FilterType::SCALEX:
            s_filter.m_scale.setFirst(string);
            break;
        case FilterType::SCALEY:
            s_filter.m_scale.setSecond(string);
            break;
        case FilterType::ZORDER:
            s_filter.m_zOrder.set(string);
            break;
    }
}

const std::string& getFilterInput(FilterType type) {
    switch (type) {
        case FilterType::GROUP:
            return s_filter.m_group.getInput();
        case FilterType::COLOR:
        case FilterType::BASECOLOR:
            return s_filter.m_color.getBaseColor().getInput();
        case FilterType::DETAILCOLOR:
        return s_filter.m_color.getDetailColor().getInput();
        case FilterType::SCALE:
        case FilterType::SCALEX:
            return s_filter.m_scale.getFirst().getInput();
        case FilterType::SCALEY:
            return s_filter.m_scale.getSecond().getInput();
        case FilterType::ZORDER:
            return s_filter.m_zOrder.getInput();
    }
}

std::optional<float> getSingleValue(FilterType type) {
    switch (type) {
        case FilterType::GROUP:
            return s_filter.m_group.getSingleValue();
        case FilterType::COLOR:
        case FilterType::BASECOLOR:
            return s_filter.m_color.getBaseColor().getSingleValue();
        case FilterType::DETAILCOLOR:
            return s_filter.m_color.getDetailColor().getSingleValue();
        case FilterType::SCALE:
        case FilterType::SCALEX:
            return s_filter.m_scale.getFirst().getSingleValue();
        case FilterType::SCALEY:
            return s_filter.m_scale.getSecond().getSingleValue();
        case FilterType::ZORDER:
            return s_filter.m_zOrder.getSingleValue();
    }
}

const FilterValue<int, ColorParser>& getColorFilter(ColorType type) {
    switch (type) {
        case ColorType::BOTH:
        case ColorType::BASE:
            return s_filter.m_color.getBaseColor();
        case ColorType::DETAIL:
            return s_filter.m_color.getDetailColor();
    }
}

FilterHSV& getFilterHSV(ColorType type) {
    switch (type) {
        case ColorType::BOTH:
        case ColorType::BASE:
            return s_filter.m_color.getBaseHSV();
        case ColorType::DETAIL:
            return s_filter.m_color.getDetailHSV();
    }
}

bool isScale(FilterType type) {
    return type == FilterType::SCALE || type == FilterType::SCALEX || type == FilterType::SCALEY;
}

bool isColor(FilterType type) {
    return type == FilterType::COLOR || type == FilterType::BASECOLOR || type == FilterType::DETAILCOLOR;
}

bool isSplit(FilterType type) {
    return type == FilterType::BASECOLOR || type == FilterType::DETAILCOLOR ||
        type == FilterType::SCALEX || type == FilterType::SCALEY;
}

bool AdvFilterPopup::setup() {
    setTitle("Advanced Filter");
    m_closeBtn->removeFromParent();

    auto infoText = "Create a <cy>delete filter</c> so only objects that <cg>match certain conditions</c> can be deleted.\n"
        "Define a <cl>range with a hyphen</c> (1-5) and a <cp>series with commas</c> (1,2,3). These can be combined (1-3,5,10).\n"
        "To also filter objects on selection, enable <cr>select filter</c>.";

    auto infoBtn = InfoAlertButton::create("Help", infoText, 1);
    infoBtn->setID("info-button"_spr);
    m_buttonMenu->addChildAtPosition(infoBtn, Anchor::TopLeft, {18, -18});

    float yOffset = 26;
    float yPositions[] = { 36 + yOffset, 0 + yOffset, -36 + yOffset };
    float xLeft = -107;
    float xRight = 65;

    // VALUE CONTROLS

    addLine("Group:", "group-controls"_spr, FilterType::GROUP, {xLeft, yPositions[0]});
    addLine("Z Order:", "z-order-controls"_spr, FilterType::ZORDER, {xRight, yPositions[0]});
    addLine("Color:", "color-controls"_spr, FilterType::COLOR, {xLeft, yPositions[1]});
    addLine("Color Base:", "base-color-controls"_spr, FilterType::BASECOLOR, {xLeft, yPositions[1]});
    addLine("Detail:", "detail-color-controls"_spr, FilterType::DETAILCOLOR, {xRight, yPositions[1]});
    addLine("Scale:", "scale-xy-controls"_spr, FilterType::SCALE, {xLeft, yPositions[2]});
    addLine("Scale X:", "scale-x-controls"_spr, FilterType::SCALEX, {xLeft, yPositions[2]});
    addLine("Scale Y:", "scale-y-controls"_spr, FilterType::SCALEY, {xRight, yPositions[2]});

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
    auto& values = s_filter.m_zLayer.getValues();

    for (int i = 0; i < 9; i++) {
        auto onSpr = ButtonSprite::create(zLayerStrings[i].c_str(), 220, false, "bigFont.fnt", "GJ_button_02.png", 25, 0.3);
        auto offSpr = ButtonSprite::create(zLayerStrings[i].c_str(), 220, false, "bigFont.fnt", "GJ_button_04.png", 25, 0.3);
        auto btn = CCMenuItemToggler::create(offSpr, onSpr, this, menu_selector(AdvFilterPopup::onZLayer));

        btn->setTag(static_cast<int>(zLayers[i]));
        btn->toggle(values.contains(zLayers[i]));
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

    auto selectAllTop = CCSprite::createWithSpriteFrameName("select-all-btn.png"_spr);
    auto selectAllSpr = CCScale9Sprite::create("GJ_button_04.png");
    selectAllTop->setScale(0.9);
    selectAllSpr->setContentSize({30, 30});
    selectAllSpr->addChildAtPosition(selectAllTop, Anchor::Center);
    auto selectAllBtn = CCMenuItemSpriteExtra::create(selectAllSpr, this, menu_selector(AdvFilterPopup::onSelectAll));

    auto resetSpr = CCSprite::createWithSpriteFrameName("GJ_resetBtn_001.png");
    auto resetBtn = CCMenuItemSpriteExtra::create(resetSpr, this, menu_selector(AdvFilterPopup::onReset));
    resetBtn->setLayoutOptions(
        AxisLayoutOptions::create()
            ->setRelativeScale(1.1)
    );
    m_resetBtn = resetBtn;

    bottomMenu->addChild(okBtn);
    bottomMenu->addChild(selectAllBtn);
    bottomMenu->addChild(resetBtn);
    bottomMenu->updateLayout();
    m_mainLayer->addChildAtPosition(bottomMenu, Anchor::Bottom, {0, 24});

    // UPDATE VALUES

    onUpdateValue();

    if (s_filter.m_scale.isSplit()) {
        scaleToggler->toggle(true);
        onToggleScale(nullptr);
    }

    if (s_filter.m_color.isSplit()) {
        colorToggler->toggle(true);
        onToggleColor(nullptr);
    }

    return true;
}

void AdvFilterPopup::addLine(
    const std::string& labelText, const std::string& id, FilterType filter, CCPoint position
) {
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

    auto input = TextInput::create(60, "NA");

    if (isColor(filter)) input->setFilter("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz,-");
    else if (isScale(filter)) input->setFilter("0123456789.,-");
    else input->setFilter("0123456789,-");

    input->getInputNode()->m_numberInput = !isColor(filter);
    input->setString(getFilterInput(filter).c_str());
    input->setTag(filter);
    input->setCallback([this, filter](const std::string& string) {
        setFilter(filter, string);
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

    if (isSplit(filter)) menu->setVisible(false);

    m_inputs[filter] = input;
    m_controlMenus[filter] = menu;
    m_arrowButtons[filter] = {decArrowBtn, incArrowBtn};
}

CCMenuItemToggler* AdvFilterPopup::createToggler(const std::string& spriteName, SEL_MenuHandler selector) {
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
    for (int i = 0; i < 3; i++) {
        auto& hsv = getFilterHSV(static_cast<ColorType>(i));
        m_moreColorBtns[i]->toggle(hsv.isActive());
    }

    for (int i = 0; i < 8; i++) {
        FilterType filter = static_cast<FilterType>(i);
        std::optional<float> value = getSingleValue(filter);

        bool enabled = value.has_value();
        if (isColor(filter) && value.value_or(0) >= 1000) enabled = false;

        auto& btns = m_arrowButtons[i];

        btns[0]->setEnabled(enabled);
        btns[0]->setOpacity(enabled ? 255 : 127);
        btns[1]->setEnabled(enabled);
        btns[1]->setOpacity(enabled ? 255 : 127);
    }

    bool enableReset = s_filter.isActive();

    m_resetBtn->setEnabled(enableReset);
    m_resetBtn->setOpacity(enableReset ? 255 : 127);

    static_cast<AFEditorUI*>(EditorUI::get())->onUpdateFilter();
}

void AdvFilterPopup::onInputArrow(CCObject* sender) {
    FilterType filter = static_cast<FilterType>(sender->getTag() / 2);

    std::optional<float> value = getSingleValue(filter);
    if (!value.has_value()) return;
    if (isColor(filter) && *value >= 1000) return;

    bool isIncrement = sender->getTag() % 2 == 1;
    float newValue = *value + (isIncrement ? 1 : -1);

    switch (filter) {
        case FilterType::GROUP:
            newValue = std::clamp(newValue, 0.f, 9999.f);
            break;
        case FilterType::COLOR:
        case FilterType::BASECOLOR:
        case FilterType::DETAILCOLOR:
            newValue = std::clamp(newValue, 1.f, 999.f);
            break;
        case FilterType::SCALE:
        case FilterType::SCALEX:
        case FilterType::SCALEY:
        case FilterType::ZORDER:
            break;
    }

    std::string string = nk::toString(newValue, 2);
    m_inputs[filter]->setString(string.c_str(), true);
}

void AdvFilterPopup::onZLayer(CCObject* sender) {
    ZLayer zLayer = static_cast<ZLayer>(sender->getTag());
    bool isActive = static_cast<CCMenuItemToggler*>(sender)->isToggled();

    if (isActive) s_filter.m_zLayer.erase(zLayer);
    else s_filter.m_zLayer.insert(zLayer);

    onUpdateValue();
}

void AdvFilterPopup::onMoreColors(CCObject* sender) {
    auto toggler = static_cast<CCMenuItemToggler*>(sender);
    toggler->toggle(!toggler->isToggled());

    ColorType colorType = static_cast<ColorType>(sender->getTag());

    updateCallback onUpdate = [this, colorType](const std::unordered_set<int>& colors, const HSVInput& hsv) {
        getFilterHSV(colorType).set(hsv);

        FilterType filter = static_cast<FilterType>(static_cast<int>(colorType) + 1);
        auto input = m_inputs[filter];

        fmt::memory_buffer mb;
        for (int color : colors) {
            auto str = nk::getColorName(color);

            if (mb.size() > 0) fmt::format_to(std::back_inserter(mb), ",{}", str);
            else fmt::format_to(std::back_inserter(mb), "{}", str);
        }

        input->setString(fmt::to_string(mb), true);

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

    m_controlMenus[FilterType::COLOR]->setVisible(!m_isToggleColor);
    m_controlMenus[FilterType::BASECOLOR]->setVisible(m_isToggleColor);
    m_controlMenus[FilterType::DETAILCOLOR]->setVisible(m_isToggleColor);

    m_moreColorBtns[ColorType::BOTH]->setVisible(!m_isToggleColor);
    m_moreColorBtns[ColorType::BASE]->setVisible(m_isToggleColor);
    m_moreColorBtns[ColorType::DETAIL]->setVisible(m_isToggleColor);

    if (!sender) return;

    m_inputs[FilterType::COLOR]->setString("");
    m_inputs[FilterType::BASECOLOR]->setString("");
    m_inputs[FilterType::DETAILCOLOR]->setString("");

    s_filter.m_color.setSplit(m_isToggleColor);
    onUpdateValue();
}

void AdvFilterPopup::onToggleScale(CCObject* sender) {
    m_isToggleScale = !m_isToggleScale;

    float xOffset = m_isToggleScale ? 167 : -5;
    float yOffset = static_cast<AnchorLayoutOptions*>(m_scaleToggler->getLayoutOptions())->getOffset().y;
    m_scaleToggler->updateAnchoredPosition(Anchor::Center, {xOffset, yOffset});

    m_controlMenus[FilterType::SCALE]->setVisible(!m_isToggleScale);
    m_controlMenus[FilterType::SCALEX]->setVisible(m_isToggleScale);
    m_controlMenus[FilterType::SCALEY]->setVisible(m_isToggleScale);

    if (!sender) return;

    m_inputs[FilterType::SCALE]->setString("");
    m_inputs[FilterType::SCALEX]->setString("");
    m_inputs[FilterType::SCALEY]->setString("");

    s_filter.m_scale.setSplit(m_isToggleScale);
    onUpdateValue();
}

void AdvFilterPopup::onReset(CCObject* sender) {
    s_filter.reset();

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

void AdvFilterPopup::onSelectAll(CCObject* sender) {
    CCArray* objects = CCArray::create();
    objects->addObjectsFromArray(LevelEditorLayer::get()->m_objects);

    auto eui = EditorUI::get();
    eui->processSelectObjects(objects);
    eui->updateButtons();
    eui->deactivateRotationControl();
    eui->deactivateScaleControl();
    eui->deactivateTransformControl();
    eui->updateObjectInfoLabel();

    onClose(nullptr);
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

    FilterType filter = static_cast<FilterType>(static_cast<int>(colorType) + 1);

    m_colorType = colorType;
    m_callback = callback;
    m_hsv = getFilterHSV(colorType).getInput();

    auto& colorFilter = getColorFilter(colorType);

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
        std::string name = nk::getColorName(colorIDs[i]);

        auto offSpr = ButtonSprite::create(name.c_str(), 40, true, "bigFont.fnt", "GJ_button_04.png", 30, 0.4);
        auto onSpr = ButtonSprite::create(name.c_str(), 40, true, "bigFont.fnt", "GJ_button_02.png", 30, 0.4);
        auto btn = CCMenuItemToggler::create(offSpr, onSpr, this, menu_selector(MoreColorsPopup::onColor));

        btn->setTag(colorIDs[i]);

        if (colorFilter.hasSingleValue(colorIDs[i])) {
            btn->toggle(true);
            m_selectedColors.insert(colorIDs[i]);
        }

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
        std::string str = i == 0 ? m_hsv.h : i == 1 ? m_hsv.s : m_hsv.v;

        auto input = TextInput::create(60, "NA");
        if (isDecimal) input->setFilter("0123456789.,-");
        else input->setFilter("0123456789,-");
        input->getInputNode()->m_numberInput = true;
        input->setString(str.c_str());
        input->setTag(i);
        input->setCallback([this, i](const std::string& string) {
            if (i == 0) m_hsv.h = string;
            else if (i == 1) m_hsv.s = string;
            else m_hsv.v = string;

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
    bool enabled = !static_cast<CCMenuItemToggler*>(sender)->isToggled();

    if (enabled) m_selectedColors.insert(colorID);
    else m_selectedColors.erase(colorID);

    onUpdateValue();
}

void MoreColorsPopup::onUpdateValue() {
    m_callback(m_selectedColors, m_hsv);
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
