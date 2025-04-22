#include "defines.h"
#include "assets/assets.h"
#include "logger.h"
#include "../renderer/my_math.h"
#include "input.h"
#include "renderer/shared_render_types.h"

uint32_t constexpr MAX_ENTITIES = 100;
uint32_t constexpr MAX_MATERIALS = 100;

enum Components
{
    COMPONENT_BALL = BIT(1),
    COMPONENT_LEFT_PADDLE = BIT(2),
    COMPONENT_RIGHT_PADDLE = BIT(3)
};

struct Entity {

    uint32_t compMask;
    uint32_t materialIdx;
    Vec2 origin;
    Vec2 vel;
    Vec2 spriteOffset;
    Rect boundingBox;
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

internal bool has_component(Entity* e, Components c)
{
   return e->compMask & c;
}

internal void add_component(Entity* e, Components c)
{
   e->compMask |= c;
}

internal void remove_component(Entity* e, Components c)
{
    e->compMask &= ~c;
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

internal Entity *create_entity(GameState *gameState,
                                Vec2 origin,
                                Vec2 spriteOffset,
                                Rect boundingBox)
{
    Entity *e = 0;

    if (gameState->entityCount < MAX_ENTITIES)
    {
        e = &gameState->entities[gameState->entityCount++];
        e->origin = origin;
        e->spriteOffset = spriteOffset;
        e->boundingBox = boundingBox;

    }
    else {
        ASSERT(0, "Reached Maximum amount of Entities!");
    }

    return e;
}

//#################################################################
//              Functions Related to the Game
//#################################################################

internal Rect get_bounding_box(Entity* e)
{
    ASSERT(e, "No Entity supplied");

    return Rect{e->origin + e->boundingBox.offset, e->boundingBox.size};
}

bool init_game(GameState*  gameState, InputState *input) 
{
    Vec2 paddleSize = get_texture_size(ASSET_SPRITE_PADDLE);
    Vec2 spriteOffsetPaddle = paddleSize / 2.0f * -1.0f;
    Vec2 ballSize = get_texture_size(ASSET_SPRITE_BALL);
    Vec2 spriteOffsetBall = ballSize / 2.0f * -1.0f;

    Entity *e = create_entity(gameState, {10.0f + paddleSize.x/2.0f, input->screenSize.y/2.0f}, spriteOffsetPaddle, {spriteOffsetPaddle, paddleSize});
    add_component(e, COMPONENT_LEFT_PADDLE);
    e->materialIdx = get_material(gameState, ASSET_SPRITE_PADDLE);

    e = create_entity(gameState, {input->screenSize.x - 10.0f - paddleSize.x/2.0f, input->screenSize.y/2.0f}, spriteOffsetPaddle, {spriteOffsetPaddle, paddleSize});
    add_component(e, COMPONENT_RIGHT_PADDLE);
    e->materialIdx = get_material(gameState, ASSET_SPRITE_PADDLE);

    e = create_entity(gameState, {input->screenSize/2.0f}, spriteOffsetBall, {spriteOffsetBall, ballSize});
    add_component(e, COMPONENT_BALL);
    e->materialIdx = get_material(gameState, ASSET_SPRITE_BALL);
    e->vel = {500.0f, 250.0f};
   

    return true;
}
void update_game(GameState* gameState, InputState *input, float dt) 
{
    float speed = 500.0f;

    for (uint32_t i = 0; i < gameState->entityCount; i++)
    {
        Entity* e = &gameState->entities[i];
        
        
        if(has_component(e, COMPONENT_LEFT_PADDLE))
        {
            e->vel = {};
            if (key_is_down(input, W_KEY))
            {
                e->vel.y = -speed;
            }
            if (key_is_down(input, S_KEY))
            {
                e->vel.y = speed;
            }

            //Confine the Paddle to the screen
            {
                e->origin.y = clamp(e->origin.y, 
                                    e->boundingBox.size.y / 2.0f, 
                                    input->screenSize.y - e->boundingBox.size.y / 2.0f);
            }

            e->origin = e->origin + e->vel *dt;
        }

        if(has_component(e, COMPONENT_RIGHT_PADDLE))
        {
            e->vel = {};
            
            Entity * ball = &gameState->entities[2];

            if(e->origin.y > ball->origin.y)
            {
                e->vel.y = -speed;

                float yHeading = e->origin.y + e->vel.y * dt;
                float distanceYToBall = ball->origin.y - e->origin.y;

                if(yHeading < distanceYToBall)
                {
                    e->origin.y = ball->origin.y;
                }
                else
                {
                    e->origin.y = yHeading;
                }
            }

            if(e->origin.y < ball->origin.y)
            {
                e->vel.x = speed;

                float yHeading = e->origin.y + e->vel.y * dt;
                float distanceYToBall = ball->origin.y - e->origin.y;

                if(yHeading > distanceYToBall)
                {
                    e->origin.y = ball->origin.y;
                }
                else
                {
                    e->origin.y = yHeading;
                }
            }
        }

        if(has_component(e, COMPONENT_BALL))
        {
            if(e->origin.x + e->boundingBox.offset.x + e->boundingBox.size.x > input->screenSize.x)
            {
                e->vel.x = -e->vel.x;
                e->origin.x -= 2* (e->origin.x + e->boundingBox.offset.x + e->boundingBox.size.x - input->screenSize.x);
            }

            if(e->origin.x + e->boundingBox.offset.x < 0.0f)
            {
                e->vel.x = -e->vel.x;
                e->origin.x -= 2*(e->origin.x + e->boundingBox.offset.x);
            }

            if(e->origin.y + e->boundingBox.offset.y + e->boundingBox.size.y > input->screenSize.y)
            {
                e->vel.y = -e->vel.y;
                e->origin.y -= 2* (e->origin.y + e->boundingBox.offset.y + e->boundingBox.size.y - input->screenSize.y);
            }

            if(e->origin.y + e->boundingBox.offset.y < 0.0f)
            {
                e->vel.y = -e->vel.y;
                e->origin.y -= 2*(e->origin.y + e->boundingBox.offset.y);
            }

            // Check for collision with paddles
            {
                Vec2 nextBallPos = e->origin + e->vel * dt;
                Entity *leftPaddle = &gameState->entities[0];
                Entity *rightPaddle = &gameState->entities[1];

                Rect leftPaddleBoundingBox = get_bounding_box(leftPaddle);
                Rect rightPaddleBoundingBox = get_bounding_box(rightPaddle);
                float leftBarrier = leftPaddleBoundingBox.pos.x + leftPaddleBoundingBox.size.x + e->boundingBox.size.x/2.0f;
                float rightBarrier = rightPaddleBoundingBox.pos.x - e->boundingBox.size.x/2.0f;
                if(e->origin.x > leftBarrier && e->origin.x < rightBarrier)
                {
                    // Left Barrier check
                    {
                        Vec2 collisionPoint = {};

                        if(line_intersection(e->origin, nextBallPos,
                                            Vec2{leftBarrier, leftPaddleBoundingBox.pos.y},
                                            Vec2{leftBarrier, leftPaddleBoundingBox.pos.y + leftPaddleBoundingBox.size.y},
                                            &collisionPoint))
                                            {
                                                e->vel.x = -e->vel.x;

                                                //Remove the Penetration
                                                nextBallPos.x += 2*(leftBarrier - nextBallPos.x);
                                            }
                    }

                     // Right
                     {
                        Vec2 collisionPoint = {};

                        if(line_intersection(e->origin, nextBallPos,
                                            Vec2{rightBarrier, rightPaddleBoundingBox.pos.y},
                                            Vec2{rightBarrier, rightPaddleBoundingBox.pos.y + rightPaddleBoundingBox.size.y},
                                            &collisionPoint))
                                            {
                                                e->vel.x = -e->vel.x;

                                                //Remove the Penetration
                                                nextBallPos.x -= 2*(nextBallPos.x - rightBarrier);
                                            }
                    }
                }
                
                e->origin = nextBallPos;
            } 

            // Increase the speed of the ball
            {
                float speedUp = 25.0f;

                if(e->vel.x < 0.0f)
                {
                    e->vel.x -= dt*speedUp;
                }
                else
                {
                    e->vel.x += dt*speedUp;
                }
                if(e->vel.y < 0.0f)
                {
                    e->vel.y -= dt*speedUp;
                }
                else
                {
                    e->vel.y += dt*speedUp;
                }
            }

               
        }
    }

};

