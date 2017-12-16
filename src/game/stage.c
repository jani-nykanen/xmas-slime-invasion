/// Stage (source)
/// (c) 2017 Jani Nykänen

#include "stage.h"

#include "../engine/graphics.h"
#include "../engine/assets.h"

/// Sky
static BITMAP* bmpSky;
/// Hills
static BITMAP* bmpHills;
/// Forest
static BITMAP* bmpForest;
/// Floor
static BITMAP* bmpFloor;

/// Background pos
static float bgPos;
/// Global speed
static float globalSpeed;

/// Initialize stage
void init_stage()
{
    bmpSky = get_bitmap("sky");
    bmpHills = get_bitmap("hills");
    bmpForest = get_bitmap("forest");
    bmpFloor = get_bitmap("floor");

    bgPos = 0.0f;
    globalSpeed = 1.0f;
}

/// Update stage
void update_stage(float tm)
{
    bgPos += globalSpeed * tm;
    if(bgPos >= 512.0f)
        bgPos -= 512.0f;
}

/// Draw stage
void draw_stage()
{
    int i = 0;

    draw_bitmap(bmpSky,0,0,0);

    // Hills
    int p = -((int) round(bgPos/4.0f)) % 128;
    for(i=0; i <2; i++)
    {
        draw_bitmap(bmpHills,p+i*128,24,0);
    }

    // Forest
    p = -((int) round(bgPos/2.0f)) % 64;
    for(i=0; i <3; i++)
    {
        draw_bitmap(bmpForest,p+i*64,96-12 - 24,0);
    }

    // Floor
    p = -((int)round(bgPos)) % 16;
    for(i=0; i < 9; i++)
    {
        draw_bitmap(bmpFloor,p + i*16,96-12,0);
    }
}

/// Get global speed
float get_global_speed()
{
    return globalSpeed;
}