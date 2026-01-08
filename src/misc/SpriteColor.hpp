#pragma once

#include <Geode/binding/GJSpriteColor.hpp>
#include <Geode/binding/GameObject.hpp>

namespace nk {
    GJSpriteColor* getBaseSpriteColor(GameObject* object);
    GJSpriteColor* getDetailSpriteColor(GameObject* object);
    void normalizeSpriteColor(GJSpriteColor* color);
}