#include "AddRandomGroups.hpp"
#include <Geode/ui/TextInput.hpp>
#include <Geode/ui/Scrollbar.hpp>
#include <Geode/ui/ScrollLayer.hpp>

#include <Geode/Geode.hpp>
using namespace geode::prelude;

#include <algorithm>
#include <random>
#include <vector>
#include <regex>

bool AddRandomGroupsPopup::setup(CCArray* selectedObjects) {
    auto winSize = CCDirector::sharedDirector()->getWinSize();
    setTitle("Add Random Groups");

    m_groups = {};
    m_selectedObjects.initWithArray(as<CCArray*>(selectedObjects));

    // INFO BUTTON

    auto infoText = "<cg>Randomly distributes groups</c> between the <cy>selected objects</c>.\n"
        "Define a <cl>range of groups</c> with a hypen. (e.g. 1-10)\n"
        "By default, linked objects are treated as a single object. Use <cp>ignore linked</c> to prevent this.\n"
        "<cr>% coverage</c> controls the percentage of blocks that are assigned a group.";
    
    auto infoBtn = InfoAlertButton::create("Info", infoText, 0.7f);
    infoBtn->setPosition({158, 108});
    m_buttonMenu->addChild(infoBtn);

    // GROUP INPUT

    auto groupInput = TextInput::create(70.f, "Num", "bigFont.fnt");
    groupInput->setPosition({winSize.width / 2, winSize.height / 2 + 70});
    groupInput->setFilter("0123456789-");
    groupInput->setMaxCharCount(9);
    groupInput->setString("0");

    m_mainLayer->addChild(groupInput); 
    m_groupInput = groupInput;

    groupInput->setCallback([this](auto text) {
        if (text != "" && text.find('-') == std::string::npos && stoi(text) > 9999) m_groupInput->setString("9999");
    });

    // DECREMENT GROUP ARROW

    auto decArrowSpr = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    decArrowSpr->setScale(0.7f);
    auto decArrowBtn = CCMenuItemSpriteExtra::create(
        decArrowSpr, this, menu_selector(AddRandomGroupsPopup::onChangeInput)
    );
    decArrowBtn->setPosition({-55, 70});
    decArrowBtn->setTag(-1);
    m_buttonMenu->addChild(decArrowBtn);

    // INCREMENT GROUP ARROW

    auto incArrowSpr = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    incArrowSpr->setScale(0.7f);
    incArrowSpr->setFlipX(true);
    auto incArrowBtn = CCMenuItemSpriteExtra::create(
        incArrowSpr, this, menu_selector(AddRandomGroupsPopup::onChangeInput)
    );
    incArrowBtn->setPosition({55, 70});
    incArrowBtn->setTag(1);
    m_buttonMenu->addChild(incArrowBtn);

    // NEXT FREE BUTTON

    auto nextFreeSpr = ButtonSprite::create("Next Free", 50, true, "goldFont.fnt", "GJ_button_04.png", 25.f, 0.382f);
    auto nextFreeBtn = CCMenuItemSpriteExtra::create(
        nextFreeSpr, this, menu_selector(AddRandomGroupsPopup::onNextFree)
    );
    nextFreeBtn->setPosition({-120, 70});
    m_buttonMenu->addChild(nextFreeBtn);

    // ADD BUTTON

    auto addSpr = ButtonSprite::create("Add", 50, true, "goldFont.fnt", "GJ_button_04.png", 25.f, 0.7f);
    auto addBtn = CCMenuItemSpriteExtra::create(
        addSpr, this, menu_selector(AddRandomGroupsPopup::onAddGroup)
    );
    addBtn->setPosition(ccp(120, 70));
    m_buttonMenu->addChild(addBtn);

    // GROUPS SCROLLING PANEL

    auto scrollLayer = ScrollLayer::create({300, 80});
    scrollLayer->setAnchorPoint({0, 0});
    scrollLayer->setPosition(winSize / 2 - scrollLayer->getContentSize() / 2);

    auto scrollbar = Scrollbar::create(scrollLayer);
    scrollbar->setPosition(winSize / 2 + ccp(158, 0));
    // scrollbar->setVisible(false);

    auto groupBG = CCScale9Sprite::create("square02b_001.png", {0, 0, 80, 80});
    groupBG->setColor({ 0, 0, 0 });
    groupBG->setOpacity(50);
    groupBG->setPosition(winSize / 2);
    groupBG->setContentSize({ 300, 80 });
    m_mainLayer->addChild(groupBG);

    auto groupLayout = CCMenu::create();
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

    // "% COVERAGE" INPUT

    auto coverageLayout = CCMenu::create();
    coverageLayout->setLayout(
        RowLayout::create()
            ->setGap(5.f)
            ->setGrowCrossAxis(true)
    );
    coverageLayout->setScale(0.8f);
    coverageLayout->setContentWidth(160.f);

    auto coverageInput = TextInput::create(70.f, "Num", "bigFont.fnt");
    coverageInput->setMaxCharCount(3);
    coverageInput->setFilter("0123456789");
    coverageInput->setString("100");

    coverageLayout->addChild(coverageInput);

    auto coverageLabel = CCLabelBMFont::create("% cov.", "goldFont.fnt");
    coverageLayout->setScale(0.5f);
    coverageLayout->addChild(coverageLabel);

    coverageLayout->updateLayout();
    m_coverageInput = coverageInput;

    coverageInput->setCallback([this](auto text) {
        if (text != "" && stoi(text) > 100) m_coverageInput->setString("100");
    });

    // "IGNORE LINKED" CHECKBOX

    auto toggleLayout = CCMenu::create();
    toggleLayout->setLayout(
        RowLayout::create()
            ->setGap(15.f)
            ->setGrowCrossAxis(true)
    );
    toggleLayout->setScale(0.5f);
    toggleLayout->setContentWidth(280.f);

    auto offSpr = CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");
    auto onSpr = CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png");

    auto toggleBtn = CCMenuItemToggler::create(
        offSpr, onSpr, this, nullptr
    );
    toggleLayout->addChild(toggleBtn);

    auto toggleLabel = CCLabelBMFont::create("Ignore linked", "bigFont.fnt");
    toggleLayout->addChild(toggleLabel);

    toggleLayout->updateLayout();
    m_toggleLinkedButton = toggleBtn;

    // BOTTOM ROW LAYOUT

    auto bottomLayout = CCMenu::create();
    bottomLayout->setLayout(
        RowLayout::create()
            ->setGap(40.f)
    );
    bottomLayout->setPositionY(winSize.height / 2 - 60);
    bottomLayout->setScale(0.6f);

    bottomLayout->addChild(coverageLayout);
    bottomLayout->addChild(toggleLayout);

    bottomLayout->updateLayout();
    m_mainLayer->addChild(bottomLayout);

    // APPLY BUTTON

    auto applySpr = ButtonSprite::create("Apply", "goldFont.fnt", "GJ_button_01.png", .9f);
    auto applyBtn = CCMenuItemSpriteExtra::create(
        applySpr, this, menu_selector(AddRandomGroupsPopup::onApply)
    );
    applyBtn->setPosition({0, -95});
    m_buttonMenu->addChild(applyBtn);

    return true;
}

void AddRandomGroupsPopup::onChangeInput(CCObject* sender) {
    auto inputStr = m_groupInput->getString();
    auto inputValue = 0;
    if (inputStr != "") inputValue = stoi(inputStr);

    int newValue = inputValue + sender->getTag();
    if (newValue < 0 || newValue > 9999) return;

    m_groupInput->setString(std::to_string(newValue));
}

void AddRandomGroupsPopup::onNextFree(CCObject* sender) {
    int nextFreeID = GameManager::sharedState()->getEditorLayer()->getNextFreeGroupID({});

    if (nextFreeID < 0 || nextFreeID > 9999) return;

    m_groupInput->setString(std::to_string(nextFreeID));
}

void AddRandomGroupsPopup::onAddGroup(CCObject* sender) {
    auto inputStr = m_groupInput->getString();
    if (std::regex_match(inputStr, std::regex("^\\d+-\\d+$"))) {
        auto rangeStart = stoi(inputStr.substr(0, inputStr.find("-")));
        auto rangeEnd = stoi(inputStr.substr(inputStr.find("-") + 1));

        if (rangeStart < 1 || rangeStart > 9999 || rangeEnd < 1 || rangeEnd > 9999) return;
        if (rangeStart == rangeEnd) return;
        
        if (rangeEnd < rangeStart) std::swap(rangeStart, rangeEnd);

        for (int i = rangeStart; i <= rangeEnd; i++) {
            if (std::find(m_groups.begin(), m_groups.end(), i) == m_groups.end()) {
                addGroupButton(i);
            }
        }
    } else {
        auto inputValue = stoi(inputStr);
        if (inputValue < 1 || inputValue > 9999) return;
        if (std::find(m_groups.begin(), m_groups.end(), inputValue) != m_groups.end()) return;

        addGroupButton(inputValue);
    }

    sort(m_groups.begin(), m_groups.end());
    onChangeGroups();
}

void AddRandomGroupsPopup::addGroupButton(short group) {
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

    // m_scrollbar->setVisible(m_groups.size() > 10);

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
    auto coverageValue = stoi(static_cast<std::string>(m_coverageInput->getString()));
    auto coveragePercent = coverageValue / 100.f;

    auto linkedObjectsMap = std::make_unique<std::map<short, std::vector<GameObject*>>>();
    auto linkedObjectGroups = std::make_unique<std::vector<std::vector<GameObject*>>>();
    auto objects = as<CCArrayExt<GameObject*>>(GameManager::sharedState()->getEditorLayer()->m_editorUI->getSelectedObjects());

    for (GameObject* obj : objects)
        (*linkedObjectsMap)[obj->m_linkedGroup].push_back(obj);

    for (auto& pair : *linkedObjectsMap) {
        if (pair.second[0]->m_linkedGroup == 0) {
            for (GameObject* object : pair.second) {
                linkedObjectGroups->push_back({object});
            }
        } else {
            linkedObjectGroups->push_back(pair.second);
        }
    };

    auto rd = std::random_device {};
    auto rng = std::default_random_engine { rd() };
    std::shuffle(linkedObjectGroups->begin(), linkedObjectGroups->end(), rng);

    short numToKeep = static_cast<int>(std::round(linkedObjectGroups->size() * coveragePercent));
    linkedObjectGroups->resize(numToKeep);

    short groupIndex = 0;

    for (std::vector<GameObject*>& linkedObject : *linkedObjectGroups) {
        if (isIgnoreLinked || linkedObject[0]->m_linkedGroup == 0) {
            std::shuffle(linkedObject.begin(), linkedObject.end(), rng);

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
    } else if (m_coverageInput->getString() == "" || stoi(static_cast<std::string>(m_coverageInput->getString())) == 0) {
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

AddRandomGroupsPopup* AddRandomGroupsPopup::create(CCArray* selectedObjects) {
    auto ret = new AddRandomGroupsPopup();
    if (ret && ret->init(350.f, 250.f, selectedObjects)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
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