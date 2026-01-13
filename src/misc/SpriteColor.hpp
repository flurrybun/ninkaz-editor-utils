#pragma once

#include <Geode/Geode.hpp>
using namespace geode::prelude;

namespace nk {
    GJSpriteColor* getBaseSpriteColor(GameObject* object);
    GJSpriteColor* getDetailSpriteColor(GameObject* object);
    void normalizeSpriteColor(GJSpriteColor* color);

    std::string getColorName(int colorID);
    Result<int> colorIDFromString(const std::string& name);
}