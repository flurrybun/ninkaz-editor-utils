#pragma once

#include "DialogueLine.hpp"

#include <Geode/Geode.hpp>
using namespace geode::prelude;

class DialogueLayer : public CCLayer {
protected:
    std::vector<DialogueLine> m_queuedLines;
    CCArray* m_characters;
    bool m_isDialogueActive = false;

    enum ActionTag {
        FadeIn = 0,
        Effect = 1,
    };
    
    bool init();
    void displayNextLine();
    void onDialogueEnd();
    virtual bool ccTouchBegan(CCTouch*, CCEvent*);

public:
    void queueLine(const std::string&);
    void startDialogue();
    
    static DialogueLayer* create();
};