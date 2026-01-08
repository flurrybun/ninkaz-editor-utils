#include <Geode/modify/SetupTriggerPopup.hpp>
#include <Geode/modify/SetupPickupTriggerPopup.hpp>
#include <Geode/modify/SetupInteractObjectPopup.hpp>
#include "multi-edit/MultiEditContext.hpp"

#include <Geode/Geode.hpp>
using namespace geode::prelude;

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

class $modify(TNFSetupTriggerPopup, SetupTriggerPopup) {
    $override
    bool init(EffectGameObject* trigger, CCArray* triggers, float width, float height, int unkEnum) {
        if (!SetupTriggerPopup::init(trigger, triggers, width, height, unkEnum)) return false;
        if (!MultiEditContext::isTriggerPopup(this)) return true;

        auto ctx = MultiEditContext::get(this);
        if (!ctx) return true;

        queueInMainThread([this, ctx]() {
            for (auto [property, input] : ctx->getInputs()) {
                if (property != 51 && property != 71 && property != 80 && property != 95) continue;

                CCLabelBMFont* label = ctx->getInputLabel(property);
                if (!label) continue;

                std::string labelStr = label->getString();

                if (label->getPositionY() <= input->getPositionY()) continue;
                if (labelStr == "EffectID:") continue;

                // shorten some especially long labels

                if (labelStr == "Target Group ID:") label->setString("Target Group:");
                else if (labelStr == "Center Group ID:") label->setString("Center Group:");
                else if (labelStr == "TargetPos Group ID:") label->setString("TargetPos Group:");

                bool isAreaTrigger = typeinfo_cast<SetupAreaMoveTriggerPopup*>(this) != nullptr;

                auto plusSpr = CCSprite::createWithSpriteFrameName("GJ_plus2Btn_001.png");
                plusSpr->setScale(
                    isAreaTrigger ? 0.8 : (label->getScale() * 1.5f)
                );

                auto plusBtn = CCMenuItemSpriteExtra::create(plusSpr, this, menu_selector(TNFSetupTriggerPopup::onNextFree));
                plusBtn->setTag(property);
                plusBtn->setID("next-free-btn"_spr);

                for (const auto& group : CCArrayExt<CCArray>(m_groupContainers)) {
                    if (group->containsObject(input)) continue;

                    group->addObject(plusBtn);
                    break;
                }

                for (const auto& page : CCArrayExt<CCArray>(m_pageContainers)) {
                    if (!page->containsObject(input)) continue;

                    page->addObject(plusBtn);
                    break;
                }

                if (isAreaTrigger) {
                    plusBtn->setPosition({145, input->getPositionY() - m_buttonMenu->getPositionY()});
                } else {
                    constexpr float GAP = 3;
                    float centerX = label->getPositionX();
                    float labelModX = -(plusBtn->getScaledContentWidth() + GAP) / 2;
                    float btnModX = (label->getScaledContentWidth() + GAP) / 2;
                    float btnY = label->getPositionY() + label->getScaledContentHeight() * (0.5f - label->getAnchorPoint().y);

                    plusBtn->setPosition(ccp(centerX + btnModX, btnY) - m_buttonMenu->getPosition());
                    label->setPositionX(centerX + labelModX);
                }

                m_buttonMenu->addChild(plusBtn);
                plusBtn->setVisible(label->isVisible());
                ctx->addButton(plusBtn, property);
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

        EffectGameObject* trigger = m_gameObject
            ? static_cast<EffectGameObject*>(m_gameObject)
            : static_cast<EffectGameObject*>(m_gameObjects->firstObject());

        if (property == 51) {
            bool targetControlID = m_gameObject
                ? static_cast<EffectGameObject*>(m_gameObject)->m_targetControlID
                : static_cast<EffectGameObject*>(m_gameObjects->firstObject())->m_targetControlID;

            if (targetControlID) {
                nextFree = getNextFreeControlID();
            } else {
                nextFree = getNextFreeGroupID();
            }
        } else if (property == 71) {
            nextFree = getNextFreeGroupID();
        } else if (property == 80 || property == 95) {
            // item id, collision block id, and timer id all use the same property id

            if (usesBlockID(trigger)) {
                nextFree = lel->getNextFreeBlockID(nullptr);
            } else if (usesTimerID(trigger)) {
                nextFree = getNextFreeItemID(true);
            } else {
                nextFree = getNextFreeItemID(false);
            }
        } else {
            return;
        }

        input->setString(std::to_string(nextFree));
        input->m_delegate->textChanged(input);
    }
};

class $modify(SetupPickupTriggerPopup) {
    $override
    bool init(EffectGameObject* object, CCArray* objects) {
        if (!SetupPickupTriggerPopup::init(object, objects)) return false;

        // get your UGLY ASS robtop ass next free button out of here

        m_buttonMenu->getChildByType<CCMenuItemSpriteExtra*>(4)->removeFromParent();

        return true;
    }
};

class $modify(SetupInteractObjectPopup) {
    $override
    bool init(EffectGameObject* object, CCArray* objects) {
        if (!SetupInteractObjectPopup::init(object, objects)) return false;

        // get your UGLY ASS robtop ass next free button out of here

        m_buttonMenu->getChildByType<CCMenuItemSpriteExtra*>(4)->removeFromParent();

        return true;
    }

    $override
    void updateItems() {
        SetupInteractObjectPopup::updateItems();

        // yet another popup that doesn't use robtop's group and page system

        auto ctx = MultiEditContext::get(this);
        if (!ctx) return;

        bool groupVisible =  ctx->getInputLabel(51)->isVisible();
        bool itemVisible = ctx->getInputLabel(80)->isVisible();

        for (auto button : ctx->getButtonsForProperty(51)) {
            button->setVisible(groupVisible);
        }

        for (auto button : ctx->getButtonsForProperty(80)) {
            button->setVisible(itemVisible);
        }
    }
};
