#pragma once
#include "../renderer/my_math.h"
#include "assets/assets.h"
#include "input.h"

uint32_t constexpr MAX_LABELS = 100;
uint32_t constexpr MAX_UI_ELEMENTS = 100;

struct UIID
{
    uint32_t ID;
    uint32_t layer;
};


struct UIElement
{
    UIID ID;
    AssetTypeID assetTypeID;
    Rect rect;
    char* text;
    uint32_t animationIdx;
};


struct Label 
{
    char* text;
    Vec2 pos;
};

struct UIState
{
    uint32_t globalLayer;
    UIID active, hotThisFrame, hotLastFrame;
    
    uint32_t labelCount;
    Label labels[MAX_LABELS];

    uint32_t uiElementCount;
    UIElement uiElements[MAX_UI_ELEMENTS];
};

void update_ui(UIState *ui);
void do_text(UIState *ui, Vec2 pos, char* text);
bool do_button(UIState* ui, InputState* input, AssetTypeID assetTypeID, uint32_t ID, Rect rect, char* text = 0);