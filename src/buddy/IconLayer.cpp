#include "IconLayer.hpp"

#include <Geode/Geode.hpp>
using namespace geode::prelude;

IconLayer* IconLayer::create() {
    auto ret = new IconLayer();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool IconLayer::init() {
    if (!CCLayerColor::initWithColor({ 0, 0, 0, 150 })) return false;

    setPosition({0, 0});
    setContentSize(CCDirector::sharedDirector()->getWinSize());

    setTouchEnabled(true);
    setTouchPriority(-10000000);
    
    m_winCenter = CCDirector::sharedDirector()->getWinSize() / 2;

    // auto overlay = CCLayerColor::create({ 0, 0, 0, 150 });
    // overlay->setOpacity(0);
    // overlay->setZOrder(-100);
    // addChild(overlay);

    // PLEASE REPLACE SIMPLEPLAYER WITH A CCSPRITE SO ANCHOR POINTS AND POSITIONING ISNT STUPID ANYMORE

    auto icon = SimplePlayer::create(346);
    icon->setColor(ccc3(100, 150, 0));
    icon->setSecondColor(ccc3(253, 224, 160));
    icon->setGlowOutline(ccc3(255, 250, 127));
    icon->updateColors();

    icon->setPosition({m_winCenter.width + 60, -50});
    icon->setScale(3);
    addChild(icon);

    auto dialogueLayer = DialogueLayer::create();
    dialogueLayer->setPosition(m_winCenter + ccp(0, 80));
    addChild(dialogueLayer);

    auto infoBtn = InfoAlertButton::create("Info", "suck it nerd", 0.7f);
    infoBtn->setPosition(m_winCenter + ccp(0, -100));
    addChild(infoBtn);

    m_icon = icon;
    // m_overlay = overlay;
    m_dialogueLayer = dialogueLayer;

    startIntro();

    return true;
}

void IconLayer::spawnParticle(CCObject* node) {
    auto particle = typeinfo_cast<CCParticleSystemQuad*>(node);
    if (!particle) {
        log::error("IconLayer::spawnParticle - node is not a valid particle");
        return;
    }
    
    particle->setAutoRemoveOnFinish(true);
    this->addChild(particle);
}

void IconLayer::startIntro() {
    // m_overlay->runAction(CCEaseSineInOut::create(CCFadeTo::create(0.4, 185)));
    // runAction(CCEaseSineInOut::create(CCFadeTo::create(0.4, 185)));

    // auto playerLandParticle = GameToolbox::particleFromString("10a0.02a0a0.6a-1a90a60a150a25a10a0a0a-500a0a0a0a0a5a3a0a0a1a0.1a1a0.1a1a0.1a1a0.5a0a0a0a0a0a0a0a0a0a0a1a0a0a0a0a0a100a0a0a0a0a0a0a2a1a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0", nullptr, false);
    auto playerLandParticle = GameToolbox::particleFromString("30a0.02a0.3a0.4a-1a90a60a40a10a30a2a0a-100a0a0a0a0a5a3a0a0a1a0.1a1a0.1a1a0.1a1a0.5a0a0a0a0a0a0a0a0a0a0a1a0a0a0a0a0a100a0a0a0a0a0a0a2a1a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0", nullptr, false);
    playerLandParticle->setStartColor(ccc4f(100, 150, 0, 255));
    playerLandParticle->setPosition(m_winCenter + ccp(60, -83));
    playerLandParticle->setScale(2);
    // playerLandParticle->setZOrder(100);

    m_icon->runAction(CCSequence::create(
        CCDelayTime::create(0.6),
        CCEaseSineOut::create(CCMoveTo::create(0.4, m_winCenter + ccp(60, 30))),
        CCEaseSineIn::create(CCMoveTo::create(0.2, m_winCenter + ccp(60, -60 + 24))),
        CCCallFuncO::create(this, callfuncO_selector(IconLayer::spawnParticle), playerLandParticle),
        CCDelayTime::create(0.4),
        CCCallFunc::create(this, callfunc_selector(IconLayer::afterIntro)),
        nullptr
    ));

    m_icon->setScaleX(1.8);
    m_icon->setScaleY(2.2);

    m_icon->runAction(CCSequence::create(
        CCDelayTime::create(0.6),
        CCEaseSineOut::create(CCScaleTo::create(0.4, 2, 2)),
        CCEaseSineIn::create(CCScaleTo::create(0.2, 1.9, 2.1)),
        CCScaleTo::create(0, 2.2, 1.8),
        CCEaseSineOut::create(CCScaleTo::create(0.4, 2, 2)),
        nullptr
    ));
}

void IconLayer::afterIntro() {
    m_dialogueLayer->queueLine("Hello and good morning, world!");
    m_dialogueLayer->queueLine("I am a test dialogue line.");
    m_dialogueLayer->queueLine("I really hope this works! >_<");

    m_dialogueLayer->startDialogue();
}