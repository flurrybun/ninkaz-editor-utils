#include "AdvancedPasteState.hpp"

#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/modify/EditorUI.hpp>

class $modify(APSLevelEditorLayer, LevelEditorLayer) {
    struct Fields {
        int m_copyStateObjectID = 1;
    };

    $override
    void copyObjectState(GameObject* obj) {
        LevelEditorLayer::copyObjectState(obj);
        m_fields->m_copyStateObjectID = obj->m_objectID;

        m_copyStateObject->setPositionX(obj->getPositionX());
        m_copyStateObject->setPositionY(obj->getPositionY());
        m_copyStateObject->setRotationX(obj->getRotationX());
        m_copyStateObject->setRotationY(obj->getRotationY());
        m_copyStateObject->setScaleX(obj->getScaleX());
        m_copyStateObject->setScaleY(obj->getScaleY());
    }
};

class $modify(APSEditorUI, EditorUI) {
    $override
    void onPasteState(CCObject* sender) {
        if (!Mod::get()->getSettingValue<bool>("enable-aps")) {
            return EditorUI::onPasteState(sender);
        }

        if (m_pasteStateBtn->getOpacity() != 255) return;

        if (PasteStatePopup::get()) {
            if (!sender) {
                // used betteredit paste state keybind while popup is open
                PasteStatePopup::get()->onQuickPaste(nullptr);
            }

            return;
        }

        auto popup = PasteStatePopup::create();
        popup->m_noElasticity = true;

        popup->show();
    };
};

PasteStatePopup* PasteStatePopup::s_instance = nullptr;

bool PasteStatePopup::setup() {
    setTitle("Paste State");
    m_closeBtn->removeFromParent();

    addQuickPasteButton();
    EditorUI::get()->m_pasteStateBtn->setVisible(false);

    // TOGGLERS PANEL

    auto togglerBG = CCScale9Sprite::create("square02_001.png", {0, 0, 80, 80});
    togglerBG->setID("toggler-bg"_spr);
    togglerBG->setContentSize({320, 160});
    togglerBG->setOpacity(50);
    m_mainLayer->addChildAtPosition(togglerBG, Anchor::Center, {0, -6});

    auto togglerContainer = CCMenu::create();
    togglerContainer->setID("toggler-container"_spr);
    togglerContainer->setAnchorPoint({0.5, 0.5});
    togglerContainer->setScale(0.55);
    togglerContainer->setContentHeight(250);
    togglerContainer->setLayout(
        ColumnLayout::create()
            ->setAxisReverse(true)
            ->setCrossAxisReverse(true)
            ->setGrowCrossAxis(true)
            ->setAutoScale(false)
            ->setGap(7)
    );

    for (int i = 0; i < 12; i++) {
        auto togglerMenu = createToggler(PROPERTY_NAMES[i], static_cast<Property>(i));

        if (i == 5) {
            togglerMenu->setLayoutOptions(
                AxisLayoutOptions::create()
                    ->setBreakLine(true)
            );
        }

        togglerContainer->addChild(togglerMenu);
    }

    togglerContainer->updateLayout();
    m_mainLayer->addChildAtPosition(togglerContainer, Anchor::Center, {17, -6});

    // PRESET BUTTONS

    auto presetMenu = CCMenu::create();
    presetMenu->setID("preset-menu"_spr);
    presetMenu->setContentWidth(togglerBG->getContentWidth());
    presetMenu->setLayout(
        RowLayout::create()
            ->setGap(7.5)
            ->setAutoScale(false)
    );

    for (int i = 0; i < 4; i++) {
        auto presetSpr = ButtonSprite::create(PRESET_NAMES[i], 80, true, "bigFont.fnt", "GJ_button_01.png", 27, 0.5);
        presetSpr->setScale(0.7);
        auto presetBtn = CCMenuItemSpriteExtra::create(presetSpr, this, menu_selector(PasteStatePopup::onPreset));
        presetBtn->updateSprite();
        presetBtn->setTag(i);

        presetMenu->addChild(presetBtn);
        m_presetButtons[i] = presetBtn;
    }

    presetMenu->updateLayout();
    presetMenu->setAnchorPoint({0.5, 0});
    float presetMenuHeight = togglerBG->getContentHeight() / 2 - 6 + 5;
    m_mainLayer->addChildAtPosition(presetMenu, Anchor::Center, {0, presetMenuHeight});

    // BOTTOM MENU

    auto bottomMenu = CCMenu::create();
    bottomMenu->setID("bottom-menu"_spr);
    bottomMenu->setContentWidth(200);
    bottomMenu->setLayout(
        RowLayout::create()
            ->setGap(5)
    );

    auto cancelSpr = ButtonSprite::create("Cancel", "goldFont.fnt", "GJ_button_01.png", 0.8);
    auto cancelBtn = CCMenuItemSpriteExtra::create(cancelSpr, this, menu_selector(PasteStatePopup::onCancel));
    bottomMenu->addChild(cancelBtn);

    auto pasteSpr = ButtonSprite::create("Paste", "goldFont.fnt", "GJ_button_01.png", 0.8);
    auto pasteBtn = CCMenuItemSpriteExtra::create(pasteSpr, this, menu_selector(PasteStatePopup::onPaste));
    bottomMenu->addChild(pasteBtn);

    bottomMenu->updateLayout();
    m_mainLayer->addChildAtPosition(bottomMenu, Anchor::Bottom, {0, 24});

    // SET DEFAULTS

    Preset savedPreset = static_cast<Preset>(
        Mod::get()->getSavedValue<int>("paste-state-preset", static_cast<int>(Preset::Default))
    );
    onPreset(m_presetButtons[savedPreset]);

    return true;
}

void PasteStatePopup::onClose(CCObject* sender) {
    EditorUI::get()->m_pasteStateBtn->setVisible(true);
    s_instance = nullptr;

    Popup::onClose(sender);
}

void PasteStatePopup::addQuickPasteButton() {
    auto pasteStateBtn = EditorUI::get()->m_pasteStateBtn;

    auto quickPasteSpr = CCSprite::createWithSpriteFrameName("paste-default-btn.png"_spr);
    auto quickPasteBtn = CCMenuItemSpriteExtra::create(
        quickPasteSpr, this, menu_selector(PasteStatePopup::onQuickPaste)
    );

    m_buttonMenu->addChild(quickPasteBtn);

    float worldScale = pasteStateBtn->getScale()
        * pasteStateBtn->getParent()->getScale()
        * EditorUI::get()->getScale()
        * LevelEditorLayer::get()->getScale();

    CCPoint worldPosition = pasteStateBtn->convertToWorldSpace(pasteStateBtn->getNormalImage()->getPosition());
    CCPoint quickPasteWorldPosition = m_buttonMenu->convertToWorldSpace(quickPasteBtn->getPosition());

    quickPasteSpr->setScale(worldScale);
    quickPasteBtn->setPosition(worldPosition - quickPasteWorldPosition);
}

CCMenu* PasteStatePopup::createToggler(const char* name, Property property) {
    auto menu = CCMenu::create();
    menu->setContentWidth(270);
    menu->setLayout(
        RowLayout::create()
            ->setAxisAlignment(AxisAlignment::Start)
            ->setAutoScale(false)
            ->setGap(8)
    );

    auto toggler = CCMenuItemToggler::createWithStandardSprites(this, menu_selector(PasteStatePopup::onToggler), 1);
    toggler->setTag(property);
    menu->addChild(toggler);
    m_togglers[property] = toggler;

    auto label = CCLabelBMFont::create(name, "bigFont.fnt");
    label->setScale(0.9);
    menu->addChild(label);

    if (property == ObjectID || property == Extra) {
        const char* objectIDInfo = "Replaces the objects with objects with the same properties but different object ID.\n"
            "May lead to weird behavior...<co>use wisely</c>.";
        const char* extraInfo = "Changes all properties found under the Extra and Extra2 menus in the Edit Group menu.";

        const char* infoStr = property == ObjectID ? objectIDInfo : extraInfo;

        auto infoBtn = InfoAlertButton::create("Help", infoStr, 0.75);
        menu->addChild(infoBtn);
    }

    menu->updateLayout();
    return menu;
}

void PasteStatePopup::onToggler(CCObject* sender) {
    Property property = static_cast<Property>(sender->getTag());
    m_properties[property] = !m_properties[property];

    bool isDefault = (
        std::all_of(m_properties.begin(), m_properties.begin() + 5, [](bool prop) { return prop; }) &&
        std::none_of(m_properties.begin() + 5, m_properties.end(), [](bool prop) { return prop; })
    );
    bool isAll = std::all_of(m_properties.begin(), m_properties.end(), [](bool prop) { return prop; });
    bool isNone = std::none_of(m_properties.begin(), m_properties.end(), [](bool prop) { return prop; });

    if (isDefault) setHighlightedPreset(Preset::Default);
    else if (isAll) setHighlightedPreset(Preset::All);
    else if (isNone) setHighlightedPreset(Preset::None);
    else {
        setHighlightedPreset(Preset::Custom);

        // why doesn't matjson support c arrays 😭
        std::vector<bool> propertiesVector(m_properties.begin(), m_properties.end());
        Mod::get()->setSavedValue("paste-state-custom-properties", propertiesVector);
    }
}

void PasteStatePopup::onPreset(CCObject* sender) {
    Preset preset = static_cast<Preset>(sender->getTag());

    setHighlightedPreset(preset);

    auto customProperties = Mod::get()->getSavedValue<std::vector<bool>>("paste-state-custom-properties", {});

    for (int i = 0; i < 12; i++) {
        bool isOn;

        if (preset == Preset::Default) isOn = i < 5;
        else if (preset == Preset::All) isOn = true;
        else if (preset == Preset::None) isOn = false;
        else isOn = (i < customProperties.size()) ? customProperties[i] : false;

        m_togglers[i]->toggle(isOn);
        m_properties[i] = isOn;
    }
}

void PasteStatePopup::setHighlightedPreset(Preset preset) {
    for (auto presetBtn : m_presetButtons) {
        auto spr = static_cast<ButtonSprite*>(presetBtn->getNormalImage());

        if (presetBtn->getTag() == preset) spr->updateBGImage("GJ_button_02.png");
        else spr->updateBGImage("GJ_button_01.png");
    }

    Mod::get()->setSavedValue("paste-state-preset", static_cast<int>(preset));
}

void PasteStatePopup::onCancel(CCObject* sender) {
    onClose(sender);
}

void PasteStatePopup::onQuickPaste(CCObject* sender) {
    m_properties = { true, true, true, true, true, false, false, false, false, false, false, false };
    onPaste(sender);
}

void PasteStatePopup::onPaste(CCObject* sender) {
    GameObject* srcObject = LevelEditorLayer::get()->m_copyStateObject;
    int srcObjectID = static_cast<APSLevelEditorLayer*>(LevelEditorLayer::get())->m_fields->m_copyStateObjectID;

    CCArray* destObjects = CCArray::create();

    if (EditorUI::get()->m_selectedObject) {
        destObjects->addObject(EditorUI::get()->m_selectedObject);
    } else {
        destObjects->addObjectsFromArray(EditorUI::get()->m_selectedObjects);
    }

    for (auto destObject : CCArrayExt<GameObject*>(destObjects)) {
        pasteObjectState(srcObject, destObject);
    }

    if (m_properties[ObjectID]) replaceObjectIDs(destObjects, srcObjectID);

    auto eui = EditorUI::get();
    eui->updateButtons();
    eui->deactivateRotationControl();
    eui->deactivateScaleControl();
    eui->deactivateTransformControl();
    eui->updateObjectInfoLabel();

    onClose(sender);
}

void PasteStatePopup::pasteObjectState(GameObject* src, GameObject* dest) {
    // i have zero idea how sections work but if i don't remove and re-add the object to the section it can crash

    LevelEditorLayer::get()->removeObjectFromSection(dest);

    if (m_properties[GroupID]) {
        // LevelEditorLayer::applyGroupState is inlined on imac & m1
        // and LevelEditorLayer::pasteGroupState is inlined on windows
        // so i have to implement this myself!!

        for (int i = 0; i < dest->m_groupCount; i++) {
            dest->m_groups->at(i) = 0;
        }

        dest->m_groupCount = 0;
        dest->m_enabledGroupsCounter = 0;
        dest->m_isGroupDisabled = false;

        for (int i = 0; i < src->m_groupCount; i++) {
            dest->addToGroup(src->m_groups->at(i));
        }
    }

    if (m_properties[ZLayer]) {
        auto srcZLayer = src->m_zLayer != ZLayer::Default ? src->m_zLayer : src->m_defaultZLayer;
        dest->m_zLayer = srcZLayer;
    }

    if (m_properties[ZOrder]) {
        auto srcZOrder = src->m_zOrder != 0 ? src->m_zOrder : src->m_defaultZOrder;
        dest->m_zOrder = srcZOrder;
    }

    if (m_properties[EditorLayer]) {
        dest->m_editorLayer = src->m_editorLayer;
        dest->m_editorLayer2 = src->m_editorLayer2;
    }

    // note: vanilla paste state doesn't paste all of these properties—notably group & area parent
    // and the effectgameobject exclusive properties. so TECHNICALLY the "default" preset isn't actually default
    // however, i just think it makes sense for these properties to be pasted
    // not like anyone will notice anyways

    if (m_properties[Extra]) {
        dest->m_isDontFade = src->m_isDontFade;
        dest->m_isDontEnter = src->m_isDontEnter;
        dest->m_hasNoEffects = src->m_hasNoEffects;
        dest->m_hasGroupParent = src->m_hasGroupParent;
        dest->m_hasAreaParent = src->m_hasAreaParent;
        dest->m_isDontBoostY = src->m_isDontBoostY;
        dest->m_isDontBoostX = src->m_isDontBoostX;

        dest->m_isHighDetail = src->m_isHighDetail;
        dest->m_isNoTouch = src->m_isNoTouch;
        dest->m_isPassable = src->m_isPassable;
        dest->m_isHide = src->m_isHide;
        dest->m_isNonStickX = src->m_isNonStickX;
        dest->m_isExtraSticky = src->m_isExtraSticky;
        dest->m_hasExtendedCollision = src->m_hasExtendedCollision;

        dest->m_isIceBlock = src->m_isIceBlock;
        dest->m_isGripSlope = src->m_isGripSlope;
        dest->m_hasNoGlow = src->m_hasNoGlow;
        dest->m_hasNoParticles = src->m_hasNoParticles;
        dest->m_isNonStickY = src->m_isNonStickY;
        dest->m_isScaleStick = src->m_isScaleStick;
        dest->m_hasNoAudioScale = src->m_hasNoAudioScale;

        dest->m_enterChannel = src->m_enterChannel;
        dest->m_objectMaterial = src->m_objectMaterial;

        auto effectSrc = typeinfo_cast<EffectGameObject*>(src);
        auto effectDest = typeinfo_cast<EffectGameObject*>(dest);

        if (effectSrc && effectDest) {
            effectDest->m_channelValue = effectSrc->m_channelValue;
            effectDest->m_ordValue = effectSrc->m_ordValue;

            effectDest->m_isSinglePTouch = effectSrc->m_isSinglePTouch;
            effectDest->m_hasCenterEffect = effectSrc->m_hasCenterEffect;
            effectDest->m_isReverse = effectSrc->m_isReverse;

            effectDest->m_controlID = effectSrc->m_controlID;
        }
    }

    if (m_properties[PositionX]) dest->setPositionX(src->getPositionX());
    if (m_properties[PositionY]) dest->setPositionY(src->getPositionY());

    if (m_properties[RotationX]) dest->setRotationX(src->getRotationX());
    if (m_properties[RotationY]) dest->setRotationY(src->getRotationY());

    if (m_properties[ScaleX]) dest->updateCustomScaleX(src->getScaleX());
    if (m_properties[ScaleY]) dest->updateCustomScaleY(src->getScaleY());

    dest->updateStartValues();
    LevelEditorLayer::get()->addToSection(dest);

    dest->m_updateParents = true;
}

void PasteStatePopup::replaceObjectIDs(CCArray* objects, int newID) {
    const int ORANGE_TELEPORT_PORTAL_ID = 749;

    if (newID == ORANGE_TELEPORT_PORTAL_ID) return;

    auto eui = EditorUI::get();
    auto lel = LevelEditorLayer::get();
    eui->onDeleteSelected(nullptr);
    eui->deselectAll();

    for (auto obj : CCArrayExt<GameObject*>(objects)) {
        if (obj->m_objectID == ORANGE_TELEPORT_PORTAL_ID) continue;

        obj->m_objectID = newID;
    }

    // fudging editor layer to prevent pasted objects from going to the current layer

    short prevLayer = lel->m_currentLayer;
    lel->m_currentLayer = -1;

    std::string copiedObjects = eui->copyObjects(objects, false, false);
    CCArray* newObjects = eui->pasteObjects(copiedObjects, false, false);

    lel->m_currentLayer = prevLayer;

    if (newObjects) {
        eui->selectObjects(newObjects, true);
    }
}

PasteStatePopup* PasteStatePopup::create() {
    auto ret = new PasteStatePopup();
    if (ret && ret->initAnchored(390, 280)) {
        ret->autorelease();
        s_instance = ret;

        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

PasteStatePopup* PasteStatePopup::get() {
    return s_instance;
}
