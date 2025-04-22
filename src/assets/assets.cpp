#include "assets.h"
#include "logger.h"
#include "platform.h"
#include "../src/renderer/my_math.h"

const char* get_asset(AssetTypeID assetTypeID)
{
    switch (assetTypeID)
    {
        case ASSET_SPRITE_WHITE:
        {
            char * white = new char[4];
            white[0] = 255;
            white[1] = 255;
            white[2] = 255;
            white[3] = 255;

            return (const char *)white;
        } break;

        case ASSET_SPRITE_CAKEZ:
        {
            uint32_t fileSize;
            const char* data = platform_read_file("assets/textures/cakez.DDS", &fileSize);
            return data;
        }break;

        case ASSET_SPRITE_BALL:
        {
            uint32_t fileSize;
            const char* data = platform_read_file("assets/textures/ball.DDS", &fileSize);
            return data;
        }break;

        case ASSET_SPRITE_PADDLE:
        {
            uint32_t fileSize;
            const char* data = platform_read_file("assets/textures/paddle.DDS", &fileSize);
            return data;
        }break;

        case ASSET_SPRITE_ISABELLE:
        {
            uint32_t fileSize;
            const char* data = platform_read_file("assets/textures/Left_side_solo_square-removebg-preview.DDS", &fileSize);
            return data;
        }
        break;

        case ASSET_SPRITE_FONT_ATLAS:
        {
            uint32_t fileSize;
            const char* data = platform_read_file("assets/textures/Font_Atlas_mono_5_10.dds", &fileSize);
            return data;
        }

        default:
            ASSERT(0, "Unrecognized AssetTypeID: %d", assetTypeID);
            
        }

        return 0;
}

Texture get_texture(AssetTypeID AssetTypeID)
{
    Texture texture = {};
    switch (AssetTypeID)
    {
        case ASSET_SPRITE_WHITE:
        {
            texture.size = {1.0f, 1.0f};
            texture.subSize = texture.size;
            
        }
        break;

        case ASSET_SPRITE_BALL:
        {
           texture.size = {50.0f, 50.0f};   
           texture.subSize = texture.size; 
        }
        break;

        case ASSET_SPRITE_PADDLE:
        {
           texture.size = {50.0f, 100.0f};
           texture.subSize = texture.size;
        }break;

        case ASSET_SPRITE_ISABELLE:
        {
            texture.size = {50.0f, 50.0f};
            texture.subSize = texture.size;
        }
        break;

        case ASSET_SPRITE_FONT_ATLAS:
        {
            texture.size = {240.0f, 240.0f};
            texture.subSize = {15.0f, 30.0f};
        }
        break;
        default:
        {
            ASSERT(0, "Unrecognized AssetTypeID: %d", AssetTypeID);
            
        }
    }

    return texture;
}