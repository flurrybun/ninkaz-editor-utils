// this is supposed to bring back the "follow player" option from 2.1
// and while it works, it's pretty broken. some objects don't get affected by triggers properly, probably bc gd thinks they're offscreen

// future me: try reimplementing it using the same method as static camera triggers and ignore all attempts to change the camera position

/*
#include <Geode/Geode.hpp>
#include <Geode/modify/EditorPauseLayer.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>

using namespace geode::prelude;

class $modify(FPEditorPauseLayer, EditorPauseLayer) {
    $override
    bool init(LevelEditorLayer* levelEditorLayer) {
		if (!EditorPauseLayer::init(levelEditorLayer)) return false;

        auto offSpr = CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");
        auto onSpr = CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png");
        offSpr->setScale(0.55f);
        onSpr->setScale(0.55f);

        auto toggle = CCMenuItemToggler::create(offSpr, onSpr, this, menu_selector(FPEditorPauseLayer::onFollowPlayer));
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

		return true;
	}

    void onFollowPlayer(CCObject* sender) {
        // follow player uses game variable "0001" but when i try changing it, it always gets set to false!! not sure why but oh well

        bool isFollowPlayer = Mod::get()->getSavedValue<bool>("follow-player", true);
        Mod::get()->setSavedValue("follow-player", !isFollowPlayer);
    }
};

class $modify(GJBaseGameLayer) {
    $override
    void updateCamera(float dt) {
        bool followPlayer = Mod::get()->getSavedValue<bool>("follow-player", true);
        bool isEditor = typeinfo_cast<LevelEditorLayer*>(this);

        if (!followPlayer && isEditor) return;
        GJBaseGameLayer::updateCamera(dt);
    }
};

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
*/