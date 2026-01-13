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

std::string nk::getColorName(int colorID) {
    switch (colorID) {
        case 1000: return "BG";
        case 1001: return "G1";
        case 1009: return "G2";
        case 1013: return "MG";
        case 1014: return "MG2";
        case 1002: return "Line";
        case 1003: return "3DL";
        case 1004: return "OBJ";
        case 1005: return "P1";
        case 1006: return "P2";
        case 1007: return "LBG";
        case 1010: return "Black";
        case 1011: return "White";
        case 1012: return "Lighter";
        default: return fmt::to_string(colorID);
    }
}

Result<int> nk::colorIDFromString(const std::string& name) {
    std::string str = string::toLower(name);

    if (str == "bg") return Ok(1000);
    if (str == "g1") return Ok(1001);
    if (str == "g2") return Ok(1009);
    if (str == "mg") return Ok(1013);
    if (str == "mg2") return Ok(1014);
    if (str == "line") return Ok(1002);
    if (str == "3dl") return Ok(1003);
    if (str == "obj") return Ok(1004);
    if (str == "p1") return Ok(1005);
    if (str == "p2") return Ok(1006);
    if (str == "lbg") return Ok(1007);
    if (str == "black") return Ok(1010);
    if (str == "white") return Ok(1011);
    if (str == "lighter") return Ok(1012);

    return numFromString<int>(str);
}
