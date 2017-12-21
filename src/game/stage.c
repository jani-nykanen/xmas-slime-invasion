/// Stage (source)
/// (c) 2017 Jani Nykänen

#include "stage.h"

#include "../engine/graphics.h"
#include "../engine/assets.h"
#include "../engine/controls.h"
#include "../engine/app.h"

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
/// Black timer
static float blackTimer;

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
    blackTimer = 0.0f;
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
        if(get_key_state((int)SDL_SCANCODE_L) == PRESSED)
            {
                victoryPos = 0.0f;
                housePos = 8;
                globalSpeed = 0.0f;
            }

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
            else
            {
                blackTimer += 1.0f * tm;
                if(blackTimer >= 400.0f)
                {
                    app_swap_scene("title");
                    return;
                }
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

/// Post draw stage
void post_draw_stage()
{
    if(victoryPos < 0.0f || blackTimer < 120.0f) return;

    float t = blackTimer - 120.0f;
    if(blackTimer >= 330.0f) t -= 120.0f;
    if(blackTimer > 210.0f && blackTimer < 330.0f) 
        t = 90.0f;
    else if(t > 120.0f) 
        t = 120.0f;

    t /= 120.0f;

    float angle = 0.0f;
    float r = 128 * (1.0f - t);

    int x1,y1,x2,y2,x3,y3,x4,y4;

    int cx = 32;
    int cy = 96-20;

    float step = (M_PI*2)/20.0f;

    for(; angle <= M_PI * 2; angle += step)
    {
        x1 = cx + (int)(cos(angle) * r);
        y1 = cy + (int)(sin(angle) * r);

        x2 = cx + (int)(cos(angle+step) * r);
        y2 = cy + (int)(sin(angle+step) * r);

        x3 = cx + (int)(cos(angle+step) * (128) );
        y3 = cy + (int)(sin(angle+step) * (128) );

        x4 = cx + (int)(cos(angle) * (128) );
        y4 = cy + (int)(sin(angle) * (128) );

        draw_triangle(x1,y1,x2,y2,x3,y3,0);
        draw_triangle(x3,y3,x4,y4,x1,y1,0);
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