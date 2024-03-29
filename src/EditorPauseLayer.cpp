#include <Geode/Geode.hpp>
#include <Geode/modify/EditorPauseLayer.hpp>
#include "features/AddRandomGroups.hpp"

using namespace geode::prelude;

class $modify(NKEditorPauseLayer, EditorPauseLayer) {
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
    
    bool init (LevelEditorLayer* levelEditorLayer) {
		if (!EditorPauseLayer::init(levelEditorLayer)) return false;

		auto spr = ButtonSprite::create("Random\nGroups", 30, true, "bigFont.fnt", "GJ_button_04.png", 30.f, 0.3f);
        spr->setScale(0.8f);
        
        auto btn = CCMenuItemSpriteExtra::create(
            spr, this, menu_selector(NKEditorPauseLayer::onAddRandomGroups)
        );

        auto menu = getChildByIDRecursive("small-actions-menu");
        auto beforeBtn = static_cast<CCMenuItemSpriteExtra*>(menu->getChildren()->objectAtIndex(0));
        menu->insertBefore(btn, beforeBtn);
        menu->updateLayout();

		return true;
	}
};