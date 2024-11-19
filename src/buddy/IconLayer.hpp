#pragma once

#include "DialogueLayer.hpp"
#include "DialogueLine.hpp"

#include <Geode/Geode.hpp>
using namespace geode::prelude;

class IconLayer : public CCLayerColor {
protected:
    CCSize m_winCenter;
    SimplePlayer* m_icon;
    // CCLayerColor* m_overlay;
    DialogueLayer* m_dialogueLayer;
    
    bool init();
    void spawnParticle(CCObject*);
    void startIntro();
    void afterIntro();

public:
    static IconLayer* create();
};