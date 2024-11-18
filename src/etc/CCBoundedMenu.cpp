#include "CCBoundedMenu.hpp"

#include <Geode/Geode.hpp>
using namespace geode::prelude;

bool CCBoundedMenu::ccTouchBegan(CCTouch* touch, CCEvent* event) {
    m_hasMovedOffScreen = false;
    CCPoint touchLocation = convertToWorldSpace(convertTouchToNodeSpace(touch));

    if (m_bounds.containsPoint(touchLocation)) return CCMenu::ccTouchBegan(touch, event);
    return false;
}

void CCBoundedMenu::ccTouchMoved(CCTouch* touch, CCEvent* event) {
    if (m_hasMovedOffScreen) return;
    CCPoint touchLocation = convertToWorldSpace(convertTouchToNodeSpace(touch));

    if (!m_bounds.containsPoint(touchLocation)) {
        touch->m_point = ccp(-100, -100);
        m_hasMovedOffScreen = true;
    }
    CCMenu::ccTouchMoved(touch, event);
}

CCBoundedMenu* CCBoundedMenu::create(CCRect rect) {
    auto ret = new CCBoundedMenu(rect);
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}
