#include "defines.h"
#include "assets/assets.h"
#include "logger.h"
#include "../renderer/my_math.h"
#include "input.h"
#include "renderer/shared_render_types.h"

uint32_t constexpr MAX_ENTITIES = 100;
uint32_t constexpr MAX_MATERIALS = 100;

struct Entity {
    Transform transform;
};

struct Material
{
    AssetTypeID assetTypeID;
    MaterialData materialData;
};

struct GameState {
    uint32_t entityCount;
    Entity entities[MAX_ENTITIES];

    uint32_t materialCount;
    Material materials[MAX_MATERIALS];
};
internal Entity *create_entity(GameState *gameState, Transform transform)
{
    Entity *e = 0;

    if (gameState->entityCount < MAX_ENTITIES)
    {
        e = &gameState->entities[gameState->entityCount++];
        e->transform = transform;
    }
    else {
        ASSERT(0, "Reached Maximum amount of Entities!");
    }

    return e;
}

internal uint32_t create_material(GameState *GameState, AssetTypeID assetTypeID,
                                  Vec4 color = {1.0f, 1.0f, 1.0f, 1.0f})
{

    uint32_t materialIdx = INVALID_IDX;

    if(GameState->materialCount < MAX_MATERIALS)
    {
        materialIdx = GameState->materialCount;
        Material *m = &GameState->materials[GameState->materialCount++];
        m->assetTypeID = assetTypeID;
        m->materialData.color = color;
    }
    else
    {
        ASSERT(0, "Reached maximum amount of Materials");
    }

    return materialIdx;
}

internal uint32_t get_material(GameState *gameState, AssetTypeID assetTypeID,
                                Vec4 color = {1.0f, 1.0f, 1.0f, 1.0f})
{
    uint32_t materialIdx = INVALID_IDX;
    for(uint32_t i = 0; i < gameState->materialCount; i++)
    {
        Material *m = &gameState->materials[i];
        if(m->assetTypeID == assetTypeID && m->materialData.color == color)
        {
            materialIdx = i;
            break;
        }
    }
    if(materialIdx == INVALID_IDX)
    {
        materialIdx = create_material(gameState, assetTypeID, color);
    }
    return materialIdx;
}

internal Material *get_material(GameState *gameState, uint32_t materialIdx)
{
    Material *m = 0;

    if(materialIdx < MAX_MATERIALS)
    {
        m = &gameState->materials[materialIdx];
    }

    return m;
}

bool init_game(GameState*  gameState) {

    uint32_t counter = 0;
    for (uint32_t i = 0; i < 10; i++)
    {
        for (uint32_t j = 0; j < 10; j++)
        {
            float r = (float)counter / 100.f;
            float g = r;
            float b = 1.0f - r;
            float a = r;

           Entity* e = create_entity(gameState, {i* 120.0f, j * 60.0f, 70.0f, 70.0f});
        e->transform.materialIdx = create_material(gameState, ASSET_SPRITE_CAKEZ, {r,g,b,a});
            counter++;
        }
    }
    

    return true;
}

void update_game(GameState* gameState, InputState *input) 
{
    float xVel = 0.0f;
    float yVel = 0.0f;

    if (key_is_down(input, A_KEY))
    {
        xVel = -0.1f;
    }
    if (key_is_down(input, D_KEY))
    {
        xVel = 0.1f;
    }
    if(key_pressed_this_frame(input, W_KEY))
    {
        yVel = -10.0f;
    }
    if(key_pressed_this_frame(input, S_KEY))
    {
        yVel = 10.0f;
    }

    for (uint32_t i = 0; i < gameState->entityCount; i++)
    {
        Entity* e = &gameState->entities[i];
        e->transform.xPos += xVel;
        e->transform.yPos += yVel;
    }

};