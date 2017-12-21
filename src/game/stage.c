/// Stage (source)
/// (c) 2017 Jani Nykänen

#include "stage.h"

#include "../engine/graphics.h"
#include "../engine/assets.h"

#include "game.h"

/// Sky
static BITMAP* bmpSky;
/// Hills
static BITMAP* bmpHills;
/// Forest
static BITMAP* bmpForest;
/// Floor
static BITMAP* bmpFloor;
/// House
static BITMAP* bmpHouse;
/// Victory
static BITMAP* bmpVictory;

/// Background pos
static float bgPos;
/// Global speed
static float globalSpeed;
/// Sky mode
static int skyMode;
/// Previous sky mode
static int prevSkyMode;
/// Ending sky changed
static bool endingSkyChanged;
/// Sky change limits
static int skyChangeLimits[] = 
{
    2,4,7,10
};
/// Sky change timer
static float skyChangeTimer;
/// House position
static float housePos;
/// Victory pos
static float victoryPos;

/// Initialize stage
void init_stage()
{
    bmpSky = get_bitmap("sky");
    bmpHills = get_bitmap("hills");
    bmpForest = get_bitmap("forest");
    bmpFloor = get_bitmap("floor");
    bmpHouse = get_bitmap("house");
    bmpVictory = get_bitmap("victory");

    bgPos = 0.0f;
    globalSpeed = 1.0f;
    skyMode = 0;
    prevSkyMode = 0;
    skyChangeTimer = 0.0f;
    housePos = 256.0f;
    victoryPos = -32.0f;
    endingSkyChanged = false;
}

/// Update stage
void update_stage(PLAYER* pl, float tm)
{
    prevSkyMode = skyMode;
    int i = 0;
    
    if(!is_victory())
    {
        skyMode = 0;
        for(; i < 4; i++)
        {
            if(get_kills() >= skyChangeLimits[i])
            {
                ++ skyMode;
            }
        }
        if(skyMode != prevSkyMode)
        {
            skyChangeTimer = 60.0f;
        }
    }
    else if(!endingSkyChanged)
    {
        if(skyMode > 0)
        {
            skyChangeTimer = 60.0f;
        }
        endingSkyChanged = true;
    }
    
    if(skyChangeTimer > 0.0f)
    {
        skyChangeTimer -= 1.0f * tm;
        if(skyChangeTimer <= 0.0f && is_victory())
        {
            skyMode = 0;
        }
    }

    if(!is_victory())
        globalSpeed = pl->dead ? 0.0f : 1.0f + skyMode*0.2f;
    else
    {

        if(housePos > 8)
        {
            if(globalSpeed > 0.5f)
            {
                globalSpeed -= 0.01f * tm;
                if(globalSpeed < 0.5f)
                {
                    globalSpeed = 0.5f;
                }
            }

            housePos -= globalSpeed * tm;
        }
        else
        {
            housePos = 8;
            globalSpeed = 0.0f;

            if(victoryPos < 0.0f)
            {
                victoryPos += 0.5f * tm;
                if(victoryPos > 0.0f)
                    victoryPos = 0.0f;
            }
        }
    }

    bgPos += globalSpeed * tm;
    if(bgPos >= 512.0f)
        bgPos -= 512.0f;
}

/// Draw stage
void draw_stage()
{
    int i = 0;

    draw_bitmap_region(bmpSky,0,skyMode*64,128,64,0,0,0);
        if(skyChangeTimer <= 0.0f)
        draw_bitmap_region(bmpSky,0,(bmpSky->h/5.0f)*skyMode,bmpSky->w,bmpSky->h/5,0,0,0);
    else
    {
        if(!is_victory())
        {
            draw_bitmap_region(bmpSky,0,(bmpSky->h/5.0f)*(skyMode-1),bmpSky->w,bmpSky->h/4,0,0,0);
            int skip = 6 - (int)floor(skyChangeTimer/10.0f);
            draw_skipped_bitmap_region(bmpSky,0,(bmpSky->h/5.0f)*(skyMode),bmpSky->w,bmpSky->h/5,0,0,skip,skip,0);
        }
        else
        {
            draw_bitmap_region(bmpSky,0,(bmpSky->h/5.0f)*(skyMode),bmpSky->w,bmpSky->h/4,0,0,0);
            int skip = 6 - (int)floor(skyChangeTimer/10.0f);
            draw_skipped_bitmap_region(bmpSky,0,0,bmpSky->w,bmpSky->h/5,0,0,skip,skip,0);
        }
    }

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

    // House
    if(is_victory())
    {
        draw_bitmap(bmpHouse,(int)round(housePos),96-12 - 64,0);

        if(housePos <= 8)
        {
            draw_bitmap(bmpVictory,0,victoryPos,0);
        }
    }
}

/// Get global speed
float get_global_speed()
{
    return globalSpeed;
}

/// Get the sky phase
int get_sky_phase()
{
    return skyMode;
}