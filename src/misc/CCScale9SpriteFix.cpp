#include "CCScale9SpriteFix.hpp"

void nk::fixCCScale9Sprite(CCScale9Sprite* spr) {
    // this fix and happy textures work fine together, but i'm just assuming the implementation in
    // happy textures is more compatible with texture packs than mine.

    if (Loader::get()->isModLoaded("alphalaneous.happy_textures")) {
        auto ht = Loader::get()->getLoadedMod("alphalaneous.happy_textures");
        if (ht->getSettingValue<bool>("ccscale9sprite-fix")) return;
    }

    CCSprite* sprites[3][3] = {
        {spr->_topLeft, spr->_top, spr->_topRight},
        {spr->_left, spr->_centre, spr->_right},
        {spr->_bottomLeft, spr->_bottom, spr->_bottomRight}
    };

    float outer = spr->m_spriteRect.size.height * 0.15;
    float center = spr->m_spriteRect.size.height * 0.7;

    float positions[3] = {0, outer, outer + center};
    float sizes[3] = {outer, center, outer};

    for (short row = 0; row < 3; row++) {
        for (short col = 0; col < 3; col++) {
            auto sprite = sprites[row][col];
            sprite->setTextureRect({positions[col], positions[row], sizes[col], sizes[row]});
        }
    }
}
