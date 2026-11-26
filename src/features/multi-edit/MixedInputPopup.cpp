#include "MixedInputPopup.hpp"
#include "../../ui/NumberInput.hpp"
#include "../../ui/SequenceInput.hpp"
#include "../../ui/Checkbox.hpp"
#include "Geode/utils/random.hpp"
#include "ccTypes.h"
#include <ranges>

#include <Geode/Geode.hpp>
using namespace geode::prelude;

namespace me {

bool MixedInputPopup::init(MultiEditContext* context, int property) {
    if (!Popup::init(380.f, 280.f)) return false;

    const float CONTROL_WIDTH = m_size.width - 50.f;
    const float PREVIEW_WIDTH = m_size.width - 30.f;

    m_context = context;
    m_property = property;

    std::vector<GameObject*> objects;
    std::vector<float> initialValues;

    for (auto object : context->getGameObjects()) {
        if (!context->hasProperty(object, property)) continue;

        objects.push_back(object);
        initialValues.push_back(context->getProperty(object, property));
    }

    std::ranges::sort(std::views::zip(initialValues, objects));

    m_decimalPlaces = context->getPropertyDecimalPlaces(property);
    m_propertyBounds = context->getPropertyBounds(property);

    m_calculation = Calculation::create();
    m_calculation->m_decimalPlaces = m_decimalPlaces;
    m_calculation->setValueBounds(m_propertyBounds.min, m_propertyBounds.max);
    m_calculation->setInitialValues(std::views::zip(objects, initialValues) | std::ranges::to<std::unordered_map>());

    // if all property values are equal, that means the user manually entered this menu
    // rather than simply selecting a mixed input. so they most likely want to do direction

    if (
        !initialValues.empty() &&
        std::equal(initialValues.begin() + 1, initialValues.end(), initialValues.begin())
    ) {
        m_calculation->m_direction = Direction::Right;
        m_calculation->m_initial = initialValues[0];
        m_calculation->m_modifier = 1.0;
    }

    m_closeBtn->removeFromParent();

    // INFO BUTTON

    auto infoText = "Use the <cg>operators</c> to override or modify each object's current value by the <cy>modifier value</c>.\n"
        "With a <cl>direction</c>, the operation compounds from the <cp>initial value</c> in order, based on the object's position.\n"
        "<cp>Cycle</c> hands out values from a sequence, e.g. 1-5,10,20-30 (ranges are supported).\n"
        "Define a <cl>range with a hyphen</c> (1-5) and a <cp>series with commas</c> (1,2,3). These can be combined (1-3,5,10).\n"
        "<cp>Preserve Sign</c> applies operations to each value's magnitude and keeps its original sign.\n";

    auto infoBtn = InfoAlertButton::create("Info", infoText, 0.7f);
    m_buttonMenu->addChildAtPosition(infoBtn, Anchor::TopLeft, { 18.f, -18.f });

    auto mainColumn = CCNode::create();
    mainColumn->setAnchorPoint({ 0.5f, 0.5f });
    mainColumn->setLayout(
        SimpleColumnLayout::create()
            ->setGap(7.f)
            ->setMainAxisScaling(AxisScaling::Fit)
            ->setCrossAxisScaling(AxisScaling::Fit)
    );

    // PREVIEW WINDOW

    auto topInfoRow = CCNode::create();
    topInfoRow->setLayout(
        SimpleRowLayout::create()
            ->setGap(5.f)
            ->setMainAxisAlignment(MainAxisAlignment::Between)
            ->setCrossAxisScaling(AxisScaling::Fit)
    );
    topInfoRow->setContentWidth(CONTROL_WIDTH);

    auto objCountLabel = Label::create(
        fmt::format("{} object{}", objects.size(), objects.size() != 1 ? "s" : ""),
        "goldFont.fnt"
    );
    objCountLabel->setScale(0.5f);

    auto objFrequencyRow = this->createObjectFrequencyRow(objects, 7);
    objFrequencyRow->setScale(0.4f);

    topInfoRow->addChild(objCountLabel);
    topInfoRow->addChild(objFrequencyRow);
    topInfoRow->updateLayout();

    auto previewFrame = CCLayerColor::create({ 191, 114, 62, 255 });
    previewFrame->setContentSize({ PREVIEW_WIDTH, 120.f });

    auto previewBorder = ListBorders::create();
    previewBorder->setZOrder(1);

    auto previewNode = CCNode::create();
    previewNode->setAnchorPoint({ 0.5f, 0.5f });
    previewNode->setContentHeight(110.f);
    previewNode->setLayout(
        SimpleColumnLayout::create()
            ->setGap(5.f)
            ->setMainAxisAlignment(MainAxisAlignment::Between)
            ->setCrossAxisScaling(AxisScaling::Fit)
    );
    m_previewFrame = previewNode;

    previewFrame->addChild(previewBorder);
    previewFrame->addChildAtPosition(previewNode, Anchor::Center);
    previewFrame->setLayout(
        CopySizeLayout::create()
            ->add(previewBorder)
    );

    // CONTROL BUTTONS

    auto makeBase = [](bool active) -> CCNode* {
        auto base = NineSlice::create(
            active ? "GJ_button_02.png" : "GJ_button_04.png",
            { 0, 0, 40, 40 }
        );
        base->setContentSize({ 35, 35 });
        base->setScale(0.8f);
        base->setCascadeColorEnabled(true);
        base->setCascadeOpacityEnabled(true);
        return base;
    };

    std::array<std::string, 6> operationFrames = {
        "equal-btn.png"_spr, "plus-btn.png"_spr, "minus-btn.png"_spr,
        "multiply-btn.png"_spr, "divide-btn.png"_spr, "cycle-btn.png"_spr, 
    };

    m_operationGroup = ToggleGroup<Operation>::create(
        { Operation::Set, Operation::Add, Operation::Subtract, Operation::Multiply, Operation::Divide, Operation::Cycle },
        makeBase,
        [operationFrames](Operation operation) {
            size_t idx = static_cast<size_t>(operation);
            auto spr = CCSprite::createWithSpriteFrameName(operationFrames[idx].c_str());
            spr->setScale(0.8f);

            return spr;
        },
        [this](Operation operation) {
            m_calculation->m_operation = operation;
            m_calculation->queueRecalculate();
            this->updateInputVisibility();
        },
        5.f
    );

    std::array<std::string, 6> directionFrames = {
        "no-direction-btn.png"_spr, "direction-btn.png"_spr, "direction-btn.png"_spr,
        "direction-btn.png"_spr, "direction-btn.png"_spr, getRandomFrameName(),
    };
    std::array<float, 6> directionRotations = {
        0.f, -90.f, 90.f, 180.f, 0.f, 0.f
    };

    m_directionGroup = ToggleGroup<Direction>::create(
        { Direction::None, Direction::Up, Direction::Down, Direction::Left, Direction::Right, Direction::Random },
        makeBase,
        [directionFrames, directionRotations](Direction direction) {
            size_t idx = static_cast<size_t>(direction);
            auto spr = CCSprite::createWithSpriteFrameName(directionFrames[idx].c_str());
            spr->setRotation(directionRotations[idx]);
            spr->setScale(0.8f);

            return spr;
        },
        [this](Direction direction) {
            m_operationGroup->setEnabled(Operation::Set, direction == Direction::None);
            m_operationGroup->setEnabled(Operation::Cycle, direction != Direction::None);

            if (
                direction != Direction::None && m_calculation->m_modifier == 0 &&
                m_calculation->m_operation != Operation::Set && m_calculation->m_operation != Operation::Cycle
            ) {
                m_calculation->m_modifier = 1;
                m_modifierInput->setValue(1.0);
            }

            if (direction == Direction::Random) {
                // whatever man
                std::string frameName = getRandomFrameName();
                auto frame = CCSpriteFrameCache::get()->spriteFrameByName(frameName.c_str());
                auto btn = m_directionGroup->getButton(Direction::Random);
                btn->m_onButton->getNormalImage()->getChildByIndex<CCSprite>(0)->setDisplayFrame(frame);
                btn->m_offButton->getNormalImage()->getChildByIndex<CCSprite>(0)->setDisplayFrame(frame);
            }

            m_calculation->m_direction = direction;
            m_calculation->queueRecalculate();
            this->updateInputVisibility();
        },
        5.f
    );

    m_operationGroup->setEnabled(Operation::Cycle, false);

    auto buttonColumn = CCNode::create();
    buttonColumn->setLayout(
        SimpleColumnLayout::create()
            ->setGap(5.f)
            ->setMainAxisScaling(AxisScaling::Fit)
            ->setCrossAxisScaling(AxisScaling::Fit)
    );

    buttonColumn->addChild(m_operationGroup);
    buttonColumn->addChild(m_directionGroup);
    buttonColumn->updateLayout();

    // CONTROL INPUTS

    int decimalPlaces = context->getPropertyDecimalPlaces(property);

    auto modifierInput = NumberInput::create(
        [this](double value) {
            m_calculation->m_modifier = value;
            m_calculation->queueRecalculate();
        },
        {
            .initialValue = m_calculation->m_modifier,
            .decimalPlaces = decimalPlaces,
            .label = "Modifier:"
        }
    );
    m_modifierInput = modifierInput;

    auto initialInput = NumberInput::create(
        [this](double value) {
            m_calculation->m_initial = value;
            m_calculation->queueRecalculate();
        },
        {
            .initialValue = m_calculation->m_initial,
            .decimalPlaces = decimalPlaces,
            .label = "Initial:"
        }
    );
    m_initialInput = initialInput;

    auto inputColumn = CCNode::create();
    m_inputColumn = inputColumn;
    inputColumn->setLayout(
        SimpleColumnLayout::create()
            ->setGap(3.5f)
            ->setMainAxisScaling(AxisScaling::Fit)
            ->setCrossAxisScaling(AxisScaling::Fit)
            ->setCrossAxisAlignment(CrossAxisAlignment::End)
    );

    inputColumn->addChild(modifierInput);
    inputColumn->addChild(initialInput);
    inputColumn->updateLayout();

    auto sequenceInput = SequenceInput::create(
        [this](const std::string& text) {
            m_calculation->m_cycleText = text;
            m_calculation->queueRecalculate();
        },
        {
            .initialValue = m_calculation->m_cycleText,
            // .label = "Range:"
            .label = "Sequence:"
        }
    );

    auto stepInput = NumberInput::create(
        [this](double value) {
            m_calculation->m_cycleStep = value;
            m_calculation->queueRecalculate();
        },
        {
            .initialValue = m_calculation->m_cycleStep,
            .decimalPlaces = decimalPlaces,
            .label = "Step:"
        }
    );

    auto rangeColumn = CCNode::create();
    m_rangeColumn = rangeColumn;
    rangeColumn->setLayout(
        SimpleColumnLayout::create()
            ->setGap(3.5f)
            ->setMainAxisScaling(AxisScaling::Fit)
            ->setCrossAxisScaling(AxisScaling::Fit)
            ->setCrossAxisAlignment(CrossAxisAlignment::End)
    );

    rangeColumn->addChild(sequenceInput);
    rangeColumn->addChild(stepInput);
    rangeColumn->updateLayout();

    auto controlRow = CCNode::create();
    controlRow->setLayout(
        SimpleRowLayout::create()
            ->setMainAxisAlignment(MainAxisAlignment::Between)
            ->setMainAxisScaling(AxisScaling::None)
            ->setCrossAxisScaling(AxisScaling::Fit)
    );
    controlRow->setScale(0.8f);
    controlRow->setScaledContentSize({ CONTROL_WIDTH, 0 });

    controlRow->addChild(buttonColumn);
    controlRow->addChild(inputColumn);
    controlRow->addChild(rangeColumn);
    rangeColumn->setVisible(false);
    controlRow->updateLayout();

    // CHECKBOXES

    m_calculation->m_combineLinked = true;

    auto combineCheckbox = Checkbox::create(
        [this](bool enabled) {
            m_calculation->m_combineLinked = !enabled;
            m_calculation->queueRecalculate();
        },
        {
            .initialValue = !m_calculation->m_combineLinked,
            .label = "Ignore Linked"
        }
    );

    auto checkboxRow = CCNode::create();
    checkboxRow->setLayout(
        SimpleRowLayout::create()
            ->setGap(15.f)
            ->setMainAxisScaling(AxisScaling::Fit)
            ->setCrossAxisScaling(AxisScaling::Fit)
            ->setPadding({ 0.f, 2.f, 0.f, 2.f })
    );
    auto preserveSignCheckbox = Checkbox::create(
        [this](bool enabled) {
            m_calculation->m_preserveSign = enabled;
            m_calculation->queueRecalculate();
        },
        {
            .initialValue = m_calculation->m_preserveSign,
            .label = "Preserve Sign"
        }
    );

    checkboxRow->addChild(combineCheckbox);
    checkboxRow->addChild(preserveSignCheckbox);
    checkboxRow->updateLayout();

    // APPLY, CANCEL, & OPTIONS BUTTONS

    auto popupButtonRow = CCNode::create();
    popupButtonRow->setLayout(
        SimpleRowLayout::create()
            ->setGap(10.f)
            ->setMainAxisScaling(AxisScaling::Fit)
            ->setCrossAxisScaling(AxisScaling::Fit)
    );

    auto cancelBtn = Button::createWithNode(
        ButtonSprite::create("Cancel", "goldFont.fnt", "GJ_button_01.png", 0.9f),
        [this](CCObject* sender) {
            onClose(sender);
        }
    );

    auto applyBtn = Button::createWithNode(
        ButtonSprite::create("Apply", "goldFont.fnt", "GJ_button_01.png", 0.9f),
        [this](CCObject* sender) {
            this->onApply();
        }
    );

    popupButtonRow->addChild(cancelBtn);
    popupButtonRow->addChild(applyBtn);
    popupButtonRow->updateLayout();

    mainColumn->addChild(topInfoRow);
    mainColumn->addChild(AxisGap::create(4.f));
    mainColumn->addChild(previewFrame);
    mainColumn->addChild(controlRow);
    mainColumn->addChild(checkboxRow);
    mainColumn->addChild(popupButtonRow);
    mainColumn->updateLayout();
    m_mainLayer->addChildAtPosition(mainColumn, Anchor::Center);

    this->updateInputVisibility();

    m_calculation->setObjects(objects);
    m_calculation->queueRecalculate();
    m_calculation->setRecalculateCallback([this](const std::vector<Calculation::Unit>& units) {
        this->createCalculationPreview(units);
    });

    previewFrame->setZOrder(-1);
    previewBorder->setContentSize(previewBorder->getContentSize() + ccp(5.f, 5.f));

    return true;
}

void MixedInputPopup::createCalculationPreview(const std::vector<Calculation::Unit>& units) {
    m_previewFrame->removeAllChildren();

    auto unitGroups = m_calculation->makeUnitGroups(units);

    if (unitGroups.empty()) {
        auto emptyLabel = Label::create("Nothing to preview", "chatFont.fnt");
        emptyLabel->setScale(0.5f);
        emptyLabel->setColor(ccBLACK);
        emptyLabel->setOpacity(120);
        m_previewFrame->addChildAtPosition(emptyLabel, Anchor::Center);
        return;
    }

    bool hasExcess = unitGroups.size() > 10;
    if (hasExcess) unitGroups.erase(unitGroups.begin() + 5, unitGroups.end() - 5);

    auto bottomRow = NineSlice::create("square02_small.png");
    bottomRow->setOpacity(50);
    bottomRow->setContentSize({ 340, 20 });
    bottomRow->setLayout(
        SimpleRowLayout::create()
            ->setMainAxisAlignment(MainAxisAlignment::Between)
            ->setMainAxisScaling(AxisScaling::None)
            ->setPadding(Padding::horizontal(10.f))
    );

    auto description = Label::create("chatFont.fnt");
    description->setScale(0.6f);
    m_selectedObjectRow = CCNode::create();

    bottomRow->addChild(description);
    bottomRow->addChild(m_selectedObjectRow);
    bottomRow->updateLayout();

    auto makeBase = [](bool active) -> CCNode* {
        auto base = NineSlice::create(
            active ? "GJ_button_02.png" : "GJ_button_04.png",
            { 0, 0, 40, 40 }
        );
        base->setContentSize({ 60, 40 });
        return base;
    };

    auto group = ToggleGroup<Calculation::UnitGroup>::create(
        unitGroups,
        makeBase,
        [this](const Calculation::UnitGroup& unitGroup) {
            auto frame = CCNode::create();
            frame->setLayout(
                SimpleColumnLayout::create()
                    ->setGap(3.f)
                    ->setMainAxisScaling(AxisScaling::Fit)
                    ->setCrossAxisScaling(AxisScaling::Fit)
            );
            frame->setAnchorPoint({ 0.5f, 0.5f });
            frame->setScale(0.6f);

            auto objFrame = NineSlice::create("square02_small.png");
            objFrame->setContentSize({ 78, 30 });
            objFrame->setOpacity(50);
            auto objRow = this->createObjectFrequencyRow(unitGroup.objects, 3);
            objRow->setScale(0.6f);
            objRow->setContentWidth(120.f);
            static_cast<SimpleAxisLayout*>(objRow->getLayout())->setMainAxisScaling(AxisScaling::ScaleDown);
            objRow->updateLayout();
            objFrame->addChildAtPosition(objRow, Anchor::Center);

            auto label = Label::create(unitGroup.label, "chatFont.fnt");
            label->setColor(ccBLACK);
            label->setOpacity(150);
            label->setScale(0.7f);

            frame->addChild(objFrame);
            frame->addChild(label);
            frame->updateLayout();

            return frame;
        },
        [this, bottomRow, description](const Calculation::UnitGroup& unitGroup) {
            description->setText(unitGroup.description);

            m_selectedObjectRow->removeFromParent();
            m_selectedObjectRow = this->createObjectFrequencyRow(unitGroup.objects, 7);
            m_selectedObjectRow->setScale(0.4f);
            bottomRow->addChild(m_selectedObjectRow);
            bottomRow->updateLayout();
        },
        5.f
    );

    // simpleaxislayout doesn't support wrapping :(
    // axislayout doesn't support having a fixed height with a wrapped row :((

    group->setContentWidth(320.f);
    group->setLayout(
        AxisLayout::create()
            ->setAxisAlignment(AxisAlignment::Start)
            ->setAutoScale(false)
            ->setGrowCrossAxis(true)
    );
    group->updateLayout();
    group->runCallback();

    m_previewFrame->addChild(group);
    m_previewFrame->addChild(bottomRow);
    m_previewFrame->updateLayout();

    if (hasExcess) {
        // show cut off buttons to imply there are more than just 10

        auto base1 = NineSlice::create("GJ_button_04.png", { 0, 0, 40, 40 });
        base1->setContentSize({ 15, 40 });
        base1->setAnchorPoint({ 0, 0.5 });
        auto base2 = NineSlice::create("GJ_button_04.png", { 0, 0, 40, 40 });
        base2->setContentSize({ 15, 40 });
        base2->setAnchorPoint({ 1, 0.5 });

        base1->setPosition(group->getChildByIndex(4)->getPosition() + ccp(35.f, 0.f));
        base2->setPosition(group->getChildByIndex(5)->getPosition() - ccp(35.f, 0.f));
        group->addChild(base1);
        group->addChild(base2);
    }
}

void MixedInputPopup::onApply() {
    auto& units = m_calculation->getUnits();

    for (const auto& unit : units) {
        float value = static_cast<float>(unit.value);

        for (GameObject* object : unit.objects) {
            m_context->setProperty(object, m_property, value);
        }
    }

    if (!units.empty()) {
        float first = static_cast<float>(units.front().value);
        bool allSame = std::ranges::all_of(units, [&](const Calculation::Unit& unit) {
            return static_cast<float>(unit.value) == first;
        });

        std::optional<float> common = allSame ? std::optional<float>(first) : std::nullopt;
        m_context->onMixedInputApplied(m_property, common);
    }

    this->onClose(nullptr);
}

void MixedInputPopup::updateInputVisibility() {
    bool isCycle = m_calculation->m_operation == Operation::Cycle;
    bool isSet = m_calculation->m_operation == Operation::Set;
    bool isDirectional = m_calculation->m_direction != Direction::None;

    m_rangeColumn->setVisible(isCycle);
    m_inputColumn->setVisible(!isCycle);
    m_initialInput->setEnabled(!isSet && isDirectional);

    if (auto parent = m_inputColumn->getParent()) parent->updateLayout();
}

std::string MixedInputPopup::getRandomFrameName() {
    static int prevNum = -1, prevNum2 = -1;
    int num;

    do {
        num = random::generate<int>(0, 6);
    } while (num == prevNum || num == prevNum2);

    prevNum2 = prevNum;
    prevNum = num;
    return fmt::format("random-btn_{}.png"_spr, num);
}

CCNode* MixedInputPopup::createSpriteForObjectID(int id) {
    // based on EditorUI::getCreateBtn

    GameObject* obj;

    if (id == 914 || id == 1615) {
        // @geode-ignore(unknown-resource)
        auto texture = CCTextureCache::get()->addImage("bigFont.png", false);
        obj = TextGameObject::create(texture);
    } else {
        obj = GameObject::createWithKey(id);
    }

    std::string frame = ObjectToolbox::sharedState()->intKeyToFrame(id);

    obj->customSetup();
    obj->addColorSprite(frame);
    obj->setupCustomSprites(frame);

    switch (id) {
        case 1049:
            static_cast<EffectGameObject*>(obj)->updateSpecialColor();
            break;
        case 914:
            static_cast<TextGameObject*>(obj)->updateTextObject("A", true);
            break;
        case 1615:
            static_cast<TextGameObject*>(obj)->updateTextObject("0", true);
            break;
    }

    if (
        obj->m_classType == GameObjectClassType::Effect &&
        obj->isColorObject() &&
        static_cast<EffectGameObject*>(obj)->m_shouldPreview
    ) {
        obj->setColor({ 200, 200, 255 });
    }

    if (obj->m_opacityMod2 > 0.f) {
        obj->setOpacity(255);
    }

    CCRect rect = LevelEditorLayer::get()->getObjectRect(obj, false, false);
    float fitScale = 32.f / std::max(rect.size.width, rect.size.height);

    obj->setScale(fitScale);

    auto node = CCNode::create();
    node->setAnchorPoint({ 0.5f, 0.5f });
    node->setContentSize(obj->getScaledContentSize());
    node->addChild(obj);
    obj->setPosition(node->getContentSize() / 2.f);

    if (obj->m_colorSprite && !obj->m_unk28c) {
        obj->addChild(obj->m_colorSprite, obj->m_colorZLayerRelated ? 1 : -1);

        obj->m_colorSprite->setPosition(obj->getContentSize() / 2.f);
        obj->m_colorSprite->setColor({ 200, 200, 255 });
        obj->m_colorSprite->setScale(1.f);
    }

    return node;
}

std::vector<ObjectIDFrequency> MixedInputPopup::getObjectIDFrequencies(const std::vector<GameObject*>& objects) {
    std::unordered_map<int, int> frequencyMap;
    for (GameObject* object : objects) {
        frequencyMap[object->m_objectID]++;
    }

    std::vector<ObjectIDFrequency> result;
    result.reserve(frequencyMap.size());
    
    for (const auto& [val, cnt] : frequencyMap) {
        result.push_back({val, cnt});
    }

    std::sort(result.begin(), result.end(), [](const ObjectIDFrequency& a, const ObjectIDFrequency& b) {
        return a.count > b.count; 
    });

    return result;
}

CCNode* MixedInputPopup::createObjectFrequencyRow(const std::vector<GameObject*>& objects, float max) {
    auto row = CCNode::create();
    row->setAnchorPoint({ 0.5f, 0.5f });
    row->setLayout(
        SimpleRowLayout::create()
            ->setGap(5.f)
            ->setMainAxisScaling(AxisScaling::Fit)
            ->setCrossAxisScaling(AxisScaling::Fit)
    );

    std::vector<ObjectIDFrequency> frequencies = getObjectIDFrequencies(objects);
    int i = max > 0 ? max : INT_MAX;
    bool notFirst = false;

    for (const auto& [objectID, count] : frequencies) {
        if (i == 0) {
            auto label = Label::create("...", "chatFont.fnt");
            label->setColor(ccBLACK);
            label->setOpacity(150);
            row->addChild(label);

            break;
        }

        i--;

        auto object = createSpriteForObjectID(objectID);
        auto label = Label::create(
            fmt::format("{}x", count),
            "chatFont.fnt"
        );
        label->setColor(ccBLACK);
        label->setOpacity(150);

        if (notFirst) row->addChild(AxisGap::create(15.f));
        notFirst = true;

        row->addChild(object);
        row->addChild(label);
    }

    row->updateLayout();
    return row;
}

} // namespace me
