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
    ASSET_SPRITE_BUTTON_64_16,
    ASSET_SPRITE_PONG_FROM_SCRATCH_LOGO,

    ASSET_COUNT
};

struct Texture
{
    Vec2 size;
    Vec2 subSize;
};

const char* get_asset(AssetTypeID AssetTypeID);

Texture get_texture(AssetTypeID AssetTypeID);