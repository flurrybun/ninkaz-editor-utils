#include "AddRandomGroups.hpp"
#include "../misc/CCBoundedMenu.hpp"
#include "../misc/StringUtils.hpp"
#include <Geode/modify/EditorPauseLayer.hpp>

class $modify(ARGEditorPauseLayer, EditorPauseLayer) {
    $override
    bool init(LevelEditorLayer* lel) {
        if (!EditorPauseLayer::init(lel)) return false;

        auto spr = ButtonSprite::create("Random\nGroups", 30, true, "bigFont.fnt", "GJ_button_04.png", 30.f, 0.3f);
        spr->setScale(0.8f);

        auto btn = CCMenuItemSpriteExtra::create(
            spr, this, menu_selector(ARGEditorPauseLayer::onAddRandomGroups)
        );
        btn->setID("random-groups-button"_spr);

        auto menu = getChildByID("small-actions-menu");
        auto afterNode = static_cast<CCNode*>(menu->getChildren()->objectAtIndex(0));
        menu->insertBefore(btn, afterNode);
        menu->updateLayout();

        return true;
    }

    void onAddRandomGroups(CCObject* sender) {
        CCArray* objects = m_editorLayer->m_editorUI->m_selectedObjects;

        if (objects->count() == 0) {
            FLAlertLayer::create(
                "Add Random Groups",
                "You must have at least <cr>one object</c> selected to add groups to.",
                "OK"
            )->show();

            return;
        }

        auto popup = AddRandomGroupsPopup::create(objects);
        popup->m_noElasticity = true;
        popup->show();
    }
};

bool AddRandomGroupsPopup::init(CCArray* selectedObjects) {
    if (!Popup::init(350.f, 250.f)) return false;

    auto winSize = m_mainLayer->getContentSize();
    auto winCenter = winSize / 2;
    setTitle("Add Random Groups");

    // INFO BUTTON

    auto infoText = "<cg>Randomly distributes groups</c> between the <cy>selected objects</c>.\n"
        "Define a <cl>range of groups</c> with a hypen. (e.g. 1-10)\n"
        "By default, linked objects are treated as a single object. Use <cp>ignore linked</c> to prevent this.\n"
        "<cr>% coverage</c> controls the percentage of blocks that are assigned a group.";
    
    auto infoBtn = InfoAlertButton::create("Info", infoText, 0.7f);
    infoBtn->setPosition(winCenter + ccp(158, 108));
    m_buttonMenu->addChild(infoBtn);

    // GROUP INPUT

    auto groupInput = TextInput::create(70.f, "Num", "bigFont.fnt");
    groupInput->setPosition(winCenter + ccp(0, 70));
    groupInput->setFilter("0123456789-");
    groupInput->setMaxCharCount(9);

    m_mainLayer->addChild(groupInput); 
    m_groupInput = groupInput;

    groupInput->setCallback([this](auto text) {
        if (
            text != "" &&
            text.find('-') == std::string::npos &&
            nk::toInt(text) > 9999
        ) m_groupInput->setString("9999");
    });

    // DECREMENT GROUP ARROW

    auto decArrowSpr = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    decArrowSpr->setScale(0.7f);
    auto decArrowBtn = CCMenuItemSpriteExtra::create(
        decArrowSpr, this, menu_selector(AddRandomGroupsPopup::onChangeInput)
    );
    decArrowBtn->setPosition(winCenter + ccp(-55, 70));
    decArrowBtn->setTag(-1);
    m_buttonMenu->addChild(decArrowBtn);

    // INCREMENT GROUP ARROW

    auto incArrowSpr = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    incArrowSpr->setScale(0.7f);
    incArrowSpr->setFlipX(true);
    auto incArrowBtn = CCMenuItemSpriteExtra::create(
        incArrowSpr, this, menu_selector(AddRandomGroupsPopup::onChangeInput)
    );
    incArrowBtn->setPosition(winCenter + ccp(55, 70));
    incArrowBtn->setTag(1);
    m_buttonMenu->addChild(incArrowBtn);

    // NEXT FREE BUTTON

    auto nextFreeSpr = ButtonSprite::create("Next Free", 50, true, "goldFont.fnt", "GJ_button_04.png", 25.f, 0.382f);
    auto nextFreeBtn = CCMenuItemSpriteExtra::create(
        nextFreeSpr, this, menu_selector(AddRandomGroupsPopup::onNextFree)
    );
    nextFreeBtn->setPosition(winCenter + ccp(-120, 70));
    m_buttonMenu->addChild(nextFreeBtn);

    // ADD BUTTON

    auto addSpr = ButtonSprite::create("Add", 50, true, "goldFont.fnt", "GJ_button_04.png", 25.f, 0.7f);
    auto addBtn = CCMenuItemSpriteExtra::create(
        addSpr, this, menu_selector(AddRandomGroupsPopup::onAddGroup)
    );
    addBtn->setPosition(winCenter + ccp(120, 70));
    m_buttonMenu->addChild(addBtn);

    // GROUPS SCROLLING PANEL

    auto scrollLayer = ScrollLayer::create({300, 80});
    scrollLayer->setAnchorPoint({0, 0});
    scrollLayer->setPosition(winCenter - scrollLayer->getContentSize() / 2);

    auto scrollbar = Scrollbar::create(scrollLayer);
    scrollbar->setPosition(winCenter + ccp(158 + 300, 0));

    auto groupBG = CCScale9Sprite::create("square02b_001.png", {0, 0, 80, 80});
    groupBG->setColor({ 0, 0, 0 });
    groupBG->setOpacity(50);
    groupBG->setPosition(winCenter);
    groupBG->setContentSize({ 300, 80 });
    m_mainLayer->addChild(groupBG);

    auto realWinCenter = CCDirector::get()->getWinSize() / 2;
    auto scrollRect = CCRect { realWinCenter.width - 150, realWinCenter.height - 40, 300, 80 };

    auto groupLayout = CCBoundedMenu::create(scrollRect);
    groupLayout->setLayout(
        RowLayout::create()
            ->setGap(10.f)
            ->setGrowCrossAxis(true)
            ->setAxisAlignment(AxisAlignment::Start)
    );
    groupLayout->setContentSize({ 280.f, 0.f });
    groupLayout->setPosition({13, 65});
    groupLayout->setAnchorPoint({0, 1});

    scrollLayer->m_contentLayer->addChild(groupLayout);
    m_mainLayer->addChild(scrollbar);
    m_mainLayer->addChild(scrollLayer);

    m_layout = groupLayout;
    m_scrollLayer = scrollLayer;
    m_scrollbar = scrollbar;

    handleTouchPriority(this);

    // BOTTOM ROW CONTROLS

    // auto coverageLayout = CCMenu::create();
    // coverageLayout->setLayout(
    //     RowLayout::create()
    //         ->setGap(5.f)
    //         ->setGrowCrossAxis(true)
    // );
    // coverageLayout->setScale(0.8f);
    // coverageLayout->setContentWidth(160.f);

    auto bottomLayout = CCMenu::create();
    bottomLayout->setLayout(
        RowLayout::create()
            ->setGap(40.f)
    );
    bottomLayout->setPosition(winCenter + ccp(0, -60));
    bottomLayout->setScale(0.6f);

    auto coverageInput = TextInput::create(70.f, "Num", "bigFont.fnt");
    coverageInput->setMaxCharCount(3);
    coverageInput->setFilter("0123456789");
    coverageInput->setString("100");
    coverageInput->setCallback([this](auto text) {
        if (
            text != "" &&
            nk::toInt(text) > 100
        ) m_coverageInput->setString("100");
    });
    coverageInput->setLayoutOptions(
        AxisLayoutOptions::create()
            ->setNextGap(5.f)
    );
    m_coverageInput = coverageInput;

    auto coverageLabel = CCLabelBMFont::create("% cov.", "goldFont.fnt");

    bottomLayout->addChild(coverageInput);
    bottomLayout->addChild(coverageLabel);

    // "IGNORE LINKED" CHECKBOX

    // auto toggleLayout = CCMenu::create();
    // toggleLayout->setLayout(
    //     RowLayout::create()
    //         ->setGap(15.f)
    //         ->setGrowCrossAxis(true)
    // );
    // toggleLayout->setScale(0.5f);
    // toggleLayout->setContentWidth(280.f);

    auto offSpr = CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");
    auto onSpr = CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png");

    auto toggleBtn = CCMenuItemToggler::create(
        offSpr, onSpr, this, nullptr
    );
    toggleBtn->setLayoutOptions(
        AxisLayoutOptions::create()
            ->setNextGap(15.f)
    );
    bottomLayout->addChild(toggleBtn);

    auto toggleLabel = CCLabelBMFont::create("Ignore linked", "bigFont.fnt");
    bottomLayout->addChild(toggleLabel);

    m_toggleLinkedButton = toggleBtn;

    bottomLayout->updateLayout();
    m_mainLayer->addChild(bottomLayout);

    // APPLY BUTTON

    auto applySpr = ButtonSprite::create("Apply", "goldFont.fnt", "GJ_button_01.png", .9f);
    auto applyBtn = CCMenuItemSpriteExtra::create(
        applySpr, this, menu_selector(AddRandomGroupsPopup::onApply)
    );
    applyBtn->setPosition(winCenter + ccp(0, -95));
    m_buttonMenu->addChild(applyBtn);

    return true;
}

void AddRandomGroupsPopup::onChangeInput(CCObject* sender) {
    auto inputStr = m_groupInput->getString();
    auto inputValue = 0;
    if (inputStr != "") inputValue = nk::toInt(inputStr);

    int newValue = inputValue + sender->getTag();
    if (newValue < 0 || newValue > 9999) return;

    m_groupInput->setString(std::to_string(newValue));
}

void AddRandomGroupsPopup::onNextFree(CCObject* sender) {
    int nextFreeID = LevelEditorLayer::get()->getNextFreeGroupID(gd::unordered_set<int>());

    if (nextFreeID < 0 || nextFreeID > 9999) return;

    m_groupInput->setString(std::to_string(nextFreeID));
}

void AddRandomGroupsPopup::onAddGroup(CCObject* sender) {
    std::string str = m_groupInput->getString();
    if (str.empty()) return;

    std::stringstream ss(str);
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

            Result<short> minResult = numFromString<short>(minStr);
            Result<short> maxResult = numFromString<short>(maxStr);
            if (!minResult || !maxResult) continue;

            short minVal = minResult.unwrap();
            short maxVal = maxResult.unwrap();

            if (minVal > maxVal) {
                std::swap(minVal, maxVal);
            }

            for (short i = minVal; i <= maxVal; i++) {
                addGroupButton(i);
            }
        } else {
            Result<short> valResult = numFromString<short>(segment);
            if (!valResult) continue;

            short val = valResult.unwrap();
            addGroupButton(val);
        }
    }

    sort(m_groups.begin(), m_groups.end());
    onChangeGroups();
}

void AddRandomGroupsPopup::addGroupButton(short group) {
    if (ranges::contains(m_groups, group)) return;

    auto label = std::to_string(group);
    auto spr = ButtonSprite::create(label.c_str(), 30, true, "goldFont.fnt", "GJ_button_04.png", 20.f, 0.5f);
    
    auto btn = CCMenuItemSpriteExtra::create(
        spr, this, menu_selector(AddRandomGroupsPopup::onRemoveGroup)
    );
    btn->setTag(group);

    m_groups.push_back(group);

    auto upper = std::upper_bound(m_groups.begin(), m_groups.end(), group);
    
    if (upper != m_groups.end()) {
        auto after = static_cast<CCNode*>(m_layout->getChildren()->objectAtIndex(upper - m_groups.begin()));
        m_layout->insertBefore(btn, after);
    } else {
        m_layout->addChild(btn);
    }
}

void AddRandomGroupsPopup::onRemoveGroup(CCObject* sender) {
    auto group = sender->getTag();
    m_groups.erase(std::remove(m_groups.begin(), m_groups.end(), group), m_groups.end());
    
    auto btn = static_cast<CCMenuItemSpriteExtra*>(sender);
    m_layout->removeChild(btn, true);
    m_layout->updateLayout();

    onChangeGroups();
}

void AddRandomGroupsPopup::onChangeGroups() {
    auto numRows = static_cast<int>(ceil(m_groups.size() / 5.f));
    if (numRows <= 2) numRows = 2;

    auto winCenter = m_mainLayer->getContentSize() / 2;
    bool showScrollbar = m_groups.size() > 10;
    m_scrollbar->setPosition(winCenter + ccp(158 + (showScrollbar ? 0 : 300), 0));

    short BTN_HEIGHT = 20;
    short PAD_HEIGHT = 10;
    
    auto contentHeight = numRows * BTN_HEIGHT + (numRows - 1) * PAD_HEIGHT;

    m_scrollLayer->m_contentLayer->setContentSize({300, static_cast<float>(contentHeight) + 30});
    m_layout->setPosition({13, static_cast<float>(contentHeight) + 15});

    m_layout->updateLayout();
    m_scrollLayer->moveToTop();
}

void AddRandomGroupsPopup::assignGroups() {
    auto isIgnoreLinked = m_toggleLinkedButton->isToggled();
    auto coverageValue = nk::toInt(static_cast<std::string>(m_coverageInput->getString()));
    auto coveragePercent = coverageValue / 100.f;

    std::map<short, std::vector<GameObject*>> linkedObjectsMap;
    std::vector<std::vector<GameObject*>> linkedObjectGroups;

    for (GameObject* obj : CCArrayExt<GameObject*>(m_objects))
        linkedObjectsMap[obj->m_linkedGroup].push_back(obj);

    for (auto& pair : linkedObjectsMap) {
        if (pair.second[0]->m_linkedGroup == 0) {
            for (GameObject* object : pair.second) {
                linkedObjectGroups.push_back({object});
            }
        } else {
            linkedObjectGroups.push_back(pair.second);
        }
    };

    random::shuffle(linkedObjectGroups);

    // std::sort(linkedObjectGroups.begin(), linkedObjectGroups.end(), [](const std::vector<GameObject*>& a, const std::vector<GameObject*>& b) {
    //     float avgXa = 0, avgYa = 0, avgXb = 0, avgYb = 0;
    //     for (GameObject* obj : a) {
    //         avgXa += obj->m_positionX;
    //         avgYa += obj->m_positionY;
    //     }
    //     for (GameObject* obj : b) {
    //         avgXb += obj->m_positionX;
    //         avgYb += obj->m_positionY;
    //     }
    //     avgXa /= a.size();
    //     avgYa /= a.size();
    //     avgXb /= b.size();
    //     avgYb /= b.size();

    //     return avgXa < avgXb;
    // });

    short numToKeep = static_cast<int>(std::round(linkedObjectGroups.size() * coveragePercent));
    linkedObjectGroups.resize(numToKeep);

    short groupIndex = 0;

    for (auto& linkedObject : linkedObjectGroups) {
        if (isIgnoreLinked || linkedObject[0]->m_linkedGroup == 0) {
            random::shuffle(linkedObject);

            for (GameObject* object : linkedObject) {
                auto groupID = m_groups[groupIndex % m_groups.size()];
                object->addToGroup(groupID);

                groupIndex++;
            }
        } else {
            auto groupID = m_groups[groupIndex % m_groups.size()];
            for (GameObject* object : linkedObject) {
                object->addToGroup(groupID);
            }
            groupIndex++;
        }
    }
}

void AddRandomGroupsPopup::onApply(CCObject* sender) {
    if (m_groups.size() == 0) {
        FLAlertLayer::create(
            "Error",
            "You must add at least <cr>one group</c>.",
            "OK"
        )->show();
        return;
    } else if (m_coverageInput->getString() == "" || nk::toInt(m_coverageInput->getString()) == 0) {
        FLAlertLayer::create(
            "Error",
            "<cr>% coverage</c> must be defined.",
            "OK"
        )->show();
        return;
    }
    
    assignGroups();
    onClose(sender);
}

/* sort groups sequentially based on x and y position (will add later)

std::sort(linkedObjectGroups.begin(), linkedObjectGroups.end(), [](const std::vector<GameObject*>& a, const std::vector<GameObject*>& b) {
    float avgXa = 0, avgYa = 0, avgXb = 0, avgYb = 0;
    for (GameObject* obj : a) {
        avgXa += obj->m_realXPosition;
        avgYa += obj->m_realYPosition;
    }
    for (GameObject* obj : b) {
        avgXb += obj->m_realXPosition;
        avgYb += obj->m_realYPosition;
    }
    avgXa /= a.size();
    avgYa /= a.size();
    avgXb /= b.size();
    avgYb /= b.size();
    return std::tie(avgXa, avgYa) < std::tie(avgXb, avgYb);
}); */

/* originally wanted to put my icon in the info box, but i figured
it was a bit self-indulgent...here's the code if i ever need it

auto iconSpr = SimplePlayer::create(66);
auto primary = ccc3(255, 0, 125);
auto secondary = ccc3(0, 255, 255);
iconSpr->setColor(primary);
iconSpr->setSecondColor(secondary);
iconSpr->setGlowOutline(secondary);
iconSpr->updateColors();

iconSpr->setPosition({20, 20});
this->addChild(iconSpr);

auto iconLabel = CCLabelBMFont::create("By NinKaz", "bigFont.fnt");
iconLabel->setPosition({85, 12});
iconLabel->setScale(0.5f);
this->addChild(iconLabel); */