#include "SpriteColor.hpp"

GJSpriteColor* nk::getBaseSpriteColor(GameObject* object) {
    GJSpriteColor* baseColor = object->m_baseColor;

    // for objects that have a detail color only, that color is stored in m_baseColor, NOT m_detailColor
    // m_customSpriteColor is only true if an object has a detail color only
    
    if (object->m_customSpriteColor) {
        baseColor = nullptr;
    }

    // in 2.2 you can change whether a single color sprite is base or detail, so we account for that here
    // base = 1, detail = 2, none = 0

    if (!object->m_detailColor) {
        if (object->m_customColorType == 1) {
            baseColor = object->m_baseColor;
        } else if (object->m_customColorType == 2) {
            baseColor = nullptr;
        }
    }

    return baseColor;
}

GJSpriteColor* nk::getDetailSpriteColor(GameObject* object) {
    GJSpriteColor* detailColor = object->m_detailColor;

    if (object->m_customSpriteColor) {
        detailColor = object->m_baseColor;
    }

    if (!object->m_detailColor) {
        if (object->m_customColorType == 1) {
            detailColor = nullptr;
        } else if (object->m_customColorType == 2) {
            detailColor = object->m_baseColor;
        }
    }

    return detailColor;
}

void nk::normalizeSpriteColor(GJSpriteColor* color) {
    if (!color) return;

    switch (color->m_defaultColorID) {
        case 1005: // P1
        case 1006: // P2
        case 1007: // LBG
            color->m_colorID = 1011; // set to white
    }
}
