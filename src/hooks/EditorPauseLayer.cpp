#include <Geode/Geode.hpp>
#include <Geode/modify/EditorPauseLayer.hpp>
#include "../additions/AddRandomGroups.hpp"

using namespace geode::prelude;

class $modify(NKEditorPauseLayer, EditorPauseLayer) {
    $override
    bool init(LevelEditorLayer* levelEditorLayer) {
		if (!EditorPauseLayer::init(levelEditorLayer)) return false;

        createFollowPlayerButton();
		createAddRandomGroupsButton();

		return true;
	}

    void createFollowPlayerButton() {
        auto offSpr = CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");
        auto onSpr = CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png");
        offSpr->setScale(0.55f);
        onSpr->setScale(0.55f);

        auto toggle = CCMenuItemToggler::create(offSpr, onSpr, this, menu_selector(NKEditorPauseLayer::onFollowPlayer));
        toggle->setID("follow-player-toggle"_spr);

        auto label = CCLabelBMFont::create("Follow Player", "bigFont.fnt");
        label->setID("follow-player-label"_spr);
        label->setLayoutOptions(
            AxisLayoutOptions::create()
                ->setSameLine(true)
                ->setBreakLine(true)
                ->setPrevGap(5.f)
                ->setScaleLimits(.1f, .35f)
                ->setScalePriority(1)
        );

        // copied from node ids

        auto off = toggle->m_offButton;
        auto on = toggle->m_onButton;

        float maxWidth = (std::max)(off->getContentSize().width, on->getContentSize().width);
        float maxHeight = (std::max)(off->getContentSize().height, on->getContentSize().height);

        toggle->setContentSize({maxWidth, maxHeight});
        off->setContentSize({maxWidth, maxHeight});
        on->setContentSize({maxWidth, maxHeight});

        off->setPosition({maxWidth/2, maxHeight/2});
        on->setPosition({maxWidth/2, maxHeight/2});

        off->setPosition({maxWidth/2, maxHeight/2});
        on->setPosition({maxWidth/2, maxHeight/2});

        offSpr->setPosition({maxWidth/2, maxHeight/2});
        onSpr->setPosition({maxWidth/2, maxHeight/2});

        auto optionMenu = getChildByID("options-menu");
        float oldHeight = optionMenu->getContentSize().height;

        // devs might hate me for changing a node ids layout, but if i don't do this the menu gets all fucked up
        // the labels become super tiny and updating the layout causes noticable lag

        if (auto layout = typeinfo_cast<AxisLayout*>(optionMenu->getLayout())) layout->setCrossAxisOverflow(true);
        auto afterNode = typeinfo_cast<CCMenuItemToggler*>(optionMenu->getChildByID("ignore-damage-toggle"));

        optionMenu->insertBefore(toggle, afterNode);
        optionMenu->insertBefore(label, afterNode);
        optionMenu->updateLayout();

        float width = optionMenu->getContentSize().width;
        float newHeight = optionMenu->getContentSize().height;
        float heightDiff = newHeight - oldHeight;
        float scale = oldHeight / newHeight;
        optionMenu->setPositionY(optionMenu->getPositionY() + heightDiff / 2);
        optionMenu->setScale(optionMenu->getScale() * scale);
        optionMenu->setPositionX(optionMenu->getPositionX() - width * (1 - scale) / 2);
        optionMenu->setPositionY(optionMenu->getPositionY() - newHeight * (1 - scale) / 2);

        toggle->toggle(Mod::get()->getSavedValue<bool>("follow-player", true));
    }

    void onFollowPlayer(CCObject* sender) {
        // follow player uses game variable "0001" but when i try changing it, it always gets set to false!! not sure why but oh well
        
        bool isFollowPlayer = Mod::get()->getSavedValue<bool>("follow-player", true);
        Mod::get()->setSavedValue("follow-player", !isFollowPlayer);
    }

    void createAddRandomGroupsButton() {
        auto spr = ButtonSprite::create("Random\nGroups", 30, true, "bigFont.fnt", "GJ_button_04.png", 30.f, 0.3f);
        spr->setScale(0.8f);

        auto btn = CCMenuItemSpriteExtra::create(
            spr, this, menu_selector(NKEditorPauseLayer::onAddRandomGroups)
        );
        btn->setID("random-groups-button"_spr);

        auto menu = getChildByID("small-actions-menu");
        auto afterNode = static_cast<CCNode*>(menu->getChildren()->objectAtIndex(0));
        menu->insertBefore(btn, afterNode);
        menu->updateLayout();
    }

    void onAddRandomGroups(CCObject* sender) {
        CCArray* selectedObjects = m_editorLayer->m_editorUI->getSelectedObjects();
        
        if (selectedObjects->count() == 0) {
            FLAlertLayer::create(
                "Add Random Groups",
                "You must have at least <cr>one object</c> selected to add groups to.",
                "OK"
            )->show();

            return;
        }

        AddRandomGroupsPopup::create(selectedObjects)->show();
    }
};

#include <Geode/modify/GJBaseGameLayer.hpp>
class $modify(GJBaseGameLayer) {
    $override
    void updateCamera(float dt) {
        bool followPlayer = Mod::get()->getSavedValue<bool>("follow-player", true);
        bool isEditor = typeinfo_cast<LevelEditorLayer*>(this);

        if (!followPlayer && isEditor) return;
        GJBaseGameLayer::updateCamera(dt);
    }
};

#include <Geode/modify/LevelEditorLayer.hpp>
class $modify(LevelEditorLayer) {
    struct Fields {
        bool shouldUpdateVisibility = true;
    };

    $override
    virtual void updateVisibility(float dt) {
        if (m_playbackMode == PlaybackMode::Not) m_fields->shouldUpdateVisibility = true;

        if (!m_fields->shouldUpdateVisibility) return;
        LevelEditorLayer::updateVisibility(dt);
    }

    $override
    void onPlaytest() {
        auto followPlayer = Mod::get()->getSavedValue<bool>("follow-player", true);
        if (!followPlayer) m_fields->shouldUpdateVisibility = true;
        LevelEditorLayer::onPlaytest();
        if (!followPlayer) m_fields->shouldUpdateVisibility = false;
    }
};
