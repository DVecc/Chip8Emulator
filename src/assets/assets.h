#pragma once
#include "../src/renderer/my_math.h"

enum AssetTypeID
{
    ASSET_SPRITE_WHITE,
    ASSET_SPRITE_CAKEZ,
    ASSET_SPRITE_ISABELLE,
    ASSET_SPRITE_BALL,
    ASSET_SPRITE_PADDLE,
    ASSET_SPRITE_FONT_ATLAS,

    ASSET_COUNT
};

struct Texture
{
    Vec2 size;
    Vec2 subSize;
};

const char* get_asset(AssetTypeID AssetTypeID);

Texture get_texture(AssetTypeID AssetTypeID);