#include "DialogueLayer.hpp"

#include <Geode/Geode.hpp>
using namespace geode::prelude;

DialogueLayer* DialogueLayer::create() {
    auto ret = new DialogueLayer();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool DialogueLayer::init() {
    if (!CCLayer::init()) return false;

    setTouchEnabled(true);
    setTouchPriority(-10000);

    // auto infoBtn = InfoAlertButton::create("Info", "suck it nerd", 0.7f);
    // addChild(infoBtn);

    return true;
}

void DialogueLayer::displayNextLine() {
    auto& line = m_queuedLines.front();
    m_queuedLines.erase(m_queuedLines.begin());
    m_isDialogueActive = true;

    auto str = line.text.c_str();

    auto text = CCLabelBMFont::create(str, "chatFont.fnt");
    auto characters = text->getChildren();
    m_characters = characters;
    addChild(text);

    float delay = 0;

    for (size_t i = 0; i < characters->count(); i++) {
        auto character = typeinfo_cast<CCSprite*>(characters->objectAtIndex(i));
        character->setOpacity(0);

        auto fadeInAction = CCSequence::create(
            CCDelayTime::create(delay),
            CCFadeIn::create(0.05),
            nullptr
        );
        fadeInAction->setTag(ActionTag::FadeIn);
        character->runAction(fadeInAction);

        if (str[i] == ',') delay += 0.15;
        else delay += 0.02;
    }

    runAction(CCSequence::create(
        CCDelayTime::create(delay),
        CCCallFunc::create(this, callfunc_selector(DialogueLayer::onDialogueEnd)),
        nullptr
    ));
}

void DialogueLayer::onDialogueEnd() {
    m_isDialogueActive = false;
}

void DialogueLayer::queueLine(const std::string& line) {
    m_queuedLines.emplace_back(line);
}

void DialogueLayer::startDialogue() {
    if (!m_queuedLines.empty()) {
        displayNextLine();
    }
}

bool DialogueLayer::ccTouchBegan(CCTouch* touch, CCEvent* event) {
    log::info("DialogueLayer::ccTouchBegan");
    
    if (m_isDialogueActive) {
        m_isDialogueActive = false;

        CCArrayExt<CCSprite*> characters = m_characters;

        for (auto character : characters) {
            character->stopActionByTag(ActionTag::FadeIn);
            character->setOpacity(255);
        }
    } else {
        if (!m_queuedLines.empty()) {
            displayNextLine();
        }
    }
    
    return true;
}