#include <Geode/modify/SetupTriggerPopup.hpp>
#include <Geode/modify/SetupPickupTriggerPopup.hpp>
#include <Geode/modify/SetupInteractObjectPopup.hpp>
#include <Geode/modify/SetupObjectOptions2Popup.hpp>
#include "multi-edit/MultiEditContext.hpp"

#include <Geode/Geode.hpp>
using namespace geode::prelude;

enum class PropertyType {
    GroupID,
    ItemID,
    BlockID,
    TimerID,
    ControlID,
    MaterialID,
    Other
};

int getNextFreeID(const std::set<short>& ids) {
    short expected = 0;

    for (short id : ids) {
        if (id != expected) {
            return expected;
        }
        expected++;
    }

    return expected;
}

int getNextFreeGroupID() {
    auto objects = LevelEditorLayer::get()->m_objects;
    auto ids = std::set<short>();

    // robtop's next free group id function also checks for item ids????

    for (auto object : CCArrayExt<GameObject*>(objects)) {
        for (short i = 0; i < object->m_groupCount; i += 1) {
            ids.insert(object->m_groups->at(i));
        }

        if (object->m_classType != GameObjectClassType::Effect) continue;

        auto ego = static_cast<EffectGameObject*>(object);
        ids.insert(ego->m_centerGroupID);
        ids.insert(ego->m_targetGroupID);
    }

    return std::min(getNextFreeID(ids), 9999);
}

int getNextFreeItemID(bool timer) {
    auto objects = LevelEditorLayer::get()->m_objects;
    auto ids = std::set<short>();

    // robtop's next free item id function only checks for pickup triggers????

    for (int i = 0; i < objects->count(); i++) {
        auto object = static_cast<GameObject*>(objects->objectAtIndex(i));

        if (auto label = typeinfo_cast<LabelGameObject*>(object)) {
            if (timer != label->m_isTimeCounter) continue;

            ids.insert(label->m_itemID);
            continue;
        }

        if (auto timeObject = typeinfo_cast<ItemTriggerGameObject*>(object)) {
            if (timer != timeObject->m_timer) continue;

            ids.insert(timeObject->m_itemID);
            continue;
        }

        auto ego = static_cast<EffectGameObject*>(object);

        if (object->m_objectType == GameObjectType::Collectible) {
            if (!ego->m_collectibleIsPickupItem) continue;

            ids.insert(ego->m_itemID);
            continue;
        }

        switch (object->m_objectID) {
            case 1611:
            case 1811:
            case 1817: {
                if (timer) continue;

                ids.insert(ego->m_itemID);
                continue;
            }
            case 3614:
            case 3615:
            case 3617: {
                if (!timer) continue;

                ids.insert(ego->m_itemID);
                continue;
            }
        }
    }

    return getNextFreeID(ids);
}

int getNextFreeControlID() {
    auto objects = LevelEditorLayer::get()->m_objects;
    auto ids = std::set<short>();

    for (int i = 0; i < objects->count(); i++) {
        auto object = static_cast<EffectGameObject*>(objects->objectAtIndex(i));
        if (object->m_classType != GameObjectClassType::Effect) continue;

        ids.insert(object->m_controlID);

        if (object->m_targetControlID) {
            ids.insert(object->m_targetGroupID);
        }
    }

    return getNextFreeID(ids);
}

int getNextFreeMaterialID() {
    auto objects = LevelEditorLayer::get()->m_objects;
    auto ids = std::set<short>();

    for (int i = 0; i < objects->count(); i++) {
        auto object = static_cast<GameObject*>(objects->objectAtIndex(i));
        ids.insert(object->m_objectMaterial);
    }

    return getNextFreeID(ids);
}

bool usesTimerID(EffectGameObject* object) {
    if (auto label = typeinfo_cast<LabelGameObject*>(object)) {
        return label->m_isTimeCounter;
    }

    if (auto timeObject = typeinfo_cast<ItemTriggerGameObject*>(object)) {
        return timeObject->m_timer;
    }

    switch (object->m_objectID) {
        case 3614:
        case 3615:
        case 3617:
            return true;
        default:
            return false;
    }
}

bool usesBlockID(EffectGameObject* object) {
    switch (object->m_objectID) {
        case 1816:
        case 1815:
        case 3609:
            return true;
        default:
            return false;
    }
}

void setNextFreeButtonPosition(CCLabelBMFont* label, CCMenuItemSpriteExtra* button) {
    constexpr float GAP = 3;

    button->getNormalImage()->setScale(label->getScale() * 1.5f);
    button->updateSprite();

    float centerX = label->getPositionX();
    float labelModX = -(button->getScaledContentWidth() + GAP) / 2;
    float btnModX = (label->getScaledContentWidth() + GAP) / 2;
    float btnY = label->getPositionY() + label->getScaledContentHeight() * (0.5f - label->getAnchorPoint().y);

    button->setPosition(ccp(centerX + btnModX, btnY) - button->getParent()->getPosition());
    label->setPositionX(centerX + labelModX);
}

class $modify(TNFSetupTriggerPopup, SetupTriggerPopup) {
    $override
    bool init(EffectGameObject* trigger, CCArray* triggers, float width, float height, int unkEnum) {
        if (!SetupTriggerPopup::init(trigger, triggers, width, height, unkEnum)) return false;
        if (!MultiEditContext::isTriggerPopup(this)) return true;

        auto ctx = MultiEditContext::get(this);
        if (!ctx) return true;

        queueInMainThread([this, ctx]() {
            for (auto [property, input] : ctx->getInputs()) {
                if (getPropertyType(property) == PropertyType::Other) continue;

                CCLabelBMFont* label = ctx->getInputLabel(property);
                if (!label) continue;

                std::string labelStr = label->getString();

                if (label->getPositionY() <= input->getPositionY()) continue;
                if (labelStr == "EffectID:") continue;

                // shorten some especially long labels

                if (labelStr == "Target Group ID:") label->setString("Target Group:");
                else if (labelStr == "Center Group ID:") label->setString("Center Group:");
                else if (labelStr == "TargetPos Group ID:") label->setString("TargetPos Group:");
                else if (labelStr == "Target\nGroup ID") label->setString("Target\nGroup");
                else if (labelStr == "Follow\nGroup ID") label->setString("Follow\nGroup");

                auto plusSpr = CCSprite::createWithSpriteFrameName("GJ_plus2Btn_001.png");
                plusSpr->setScale(0.8);

                auto plusBtn = CCMenuItemSpriteExtra::create(plusSpr, this, menu_selector(TNFSetupTriggerPopup::onNextFree));
                plusBtn->setTag(property);
                plusBtn->setID("next-free-btn"_spr);

                ctx->addToGroup(plusBtn, input);
                ctx->addToPage(plusBtn, input);

                m_buttonMenu->addChild(plusBtn);
                plusBtn->setVisible(label->isVisible());
                ctx->addButton(plusBtn, property);

                bool isAreaTrigger = typeinfo_cast<SetupAreaMoveTriggerPopup*>(this) != nullptr
                    && typeinfo_cast<SetupAreaAnimTriggerPopup*>(this) == nullptr;

                if (isAreaTrigger) {
                    plusBtn->setPosition({145, input->getPositionY() - m_buttonMenu->getPositionY()});
                } else {
                    setNextFreeButtonPosition(label, plusBtn);
                }
            }
        });

        return true;
    }

    void onNextFree(CCObject* sender) {
        auto ctx = MultiEditContext::get(this);
        if (!ctx) return;

        auto property = sender->getTag();
        auto input = ctx->getInput(property);
        if (!input) return;

        auto lel = LevelEditorLayer::get();
        int nextFree;

        switch (getPropertyType(property)) {
            case PropertyType::GroupID:
                nextFree = getNextFreeGroupID();
                break;
            case PropertyType::ItemID:
                nextFree = getNextFreeItemID(false);
                break;
            case PropertyType::TimerID:
                nextFree = getNextFreeItemID(true);
                break;
            case PropertyType::BlockID:
                nextFree = lel->getNextFreeBlockID(gd::unordered_set<int>());
                break;
            case PropertyType::ControlID:
                nextFree = getNextFreeControlID();
                break;
            case PropertyType::MaterialID:
                nextFree = getNextFreeMaterialID();
                break;
            case PropertyType::Other:
                return;
        }

        input->setString(fmt::to_string(nextFree));
        input->m_delegate->textChanged(input);
    }

    PropertyType getPropertyType(int property) {
        EffectGameObject* trigger = m_gameObject
            ? static_cast<EffectGameObject*>(m_gameObject)
            : static_cast<EffectGameObject*>(m_gameObjects->firstObject());
        
        if (trigger->m_classType != GameObjectClassType::Effect) {
            trigger = nullptr;
        }

        switch (property) {
            case 51:
                if (trigger && trigger->m_targetControlID) {
                    return PropertyType::ControlID;
                } else {
                    return PropertyType::GroupID;
                }
            case 71:
            case 395:
            case 516:
            case 517:
            case 518:
            case 519:
            case 457:
            case 448:
                return PropertyType::GroupID;
            case 80:
            case 95:
                if (!trigger) return PropertyType::ItemID;

                if (usesBlockID(trigger)) {
                    return PropertyType::BlockID;
                } else if (usesTimerID(trigger)) {
                    return PropertyType::TimerID;
                } else {
                    return PropertyType::ItemID;
                }
            case 446:
                return PropertyType::MaterialID;
            case 534:
                return PropertyType::ControlID;
            default:
                return PropertyType::Other;
        }
    }
};

class $modify(SetupPickupTriggerPopup) {
    static void onModify(auto& self) {
        (void)self.setHookPriority("SetupPickupTriggerPopup::init", Priority::VeryEarlyPost);
    }

    $override
    bool init(EffectGameObject* object, CCArray* objects) {
        if (!SetupPickupTriggerPopup::init(object, objects)) return false;

        // get your UGLY ASS robtop ass next free button out of here

        m_buttonMenu->getChildByType<CCMenuItemSpriteExtra*>(4)->removeFromParent();

        return true;
    }
};

class $modify(SetupInteractObjectPopup) {
    static void onModify(auto& self) {
        (void)self.setHookPriority("SetupInteractObjectPopup::init", Priority::VeryEarlyPost);
    }

    $override
    bool init(EffectGameObject* object, CCArray* objects) {
        if (!SetupInteractObjectPopup::init(object, objects)) return false;

        // get your UGLY ASS robtop ass next free button out of here

        m_buttonMenu->getChildByType<CCMenuItemSpriteExtra*>(4)->removeFromParent();

        return true;
    }
};

class $modify(SetupObjectOptions2Popup) {
    static void onModify(auto& self) {
        (void)self.setHookPriority("SetupObjectOptions2Popup::init", Priority::VeryEarlyPost);
    }

    bool init(GameObject* object, CCArray* objects) {
        if (!SetupObjectOptions2Popup::init(object, objects)) return false;

        // ok you can stay but you're on thin ice

        setNextFreeButtonPosition(
            m_mainLayer->getChildByType<CCLabelBMFont*>(1),
            m_buttonMenu->getChildByType<CCMenuItemSpriteExtra*>(4)
        );

        return true;
    }
};
