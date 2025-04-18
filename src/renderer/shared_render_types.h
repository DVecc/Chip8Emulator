#pragma once

struct GlobalData{
    int screenSizeX, screenSizeY;
};

struct Transform
{
    float xPos, yPos, sizeX, sizeY;
};

struct PushData {
    int transformIdx;
};