#pragma once

#include <Geode/Geode.hpp>
using namespace geode::prelude;

class CCBoundedMenu : public CCMenu {
private:
    CCRect m_bounds;
    bool m_hasMovedOffScreen = false;

    CCBoundedMenu(CCRect bounds) : m_bounds(bounds) {};

    bool ccTouchBegan(CCTouch*, CCEvent*) override;
    void ccTouchMoved(CCTouch*, CCEvent*) override;
public:
	static CCBoundedMenu* create(CCRect);
};