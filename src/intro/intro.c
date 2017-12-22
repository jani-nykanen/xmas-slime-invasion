/// Intro scene (source)
/// (c) 2017 Jani Nykänen

#include "intro.h"

#include "../engine/app.h"
#include "../engine/graphics.h"
#include "../engine/assets.h"

#include "../vpad.h"

#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include "time.h"

/// Creator bitmap
static BITMAP* bmpCreator;

/// Intro timer
static float introTimer;

/// Initialize intro scene
/// > 0 on success
static int intro_init()
{
    bmpCreator = get_bitmap("creator");
    introTimer = -15.0f;

    return 0;
} 

/// Update intro scene
/// < tm Time mul.
static void intro_update(float tm)
{
    introTimer += 1.0f * tm;
    if(introTimer >= 255.0f || vpad_get_button(4) == PRESSED)
    {
        app_swap_scene("title");
    }
}

/// Draw intro screen
static void intro_draw()
{
    clear_frame(0);
    if(introTimer < 0.0f || introTimer >= 240.0f) return;

    int sy = introTimer < 120.0f ? 0 : 1;

    draw_bitmap_region(bmpCreator,0,sy*96,128,96,0,0,0);

    float t = introTimer < 120.0f ? introTimer : (introTimer-120.0f);
    if(t < 30.0f)
    {
        int skip = (int)floor( (t-30.0f) / 5.0f);
        fill_skipped_rect(0,0,128,96,skip,skip,0);
    }
    else if(t >= 90.0f && t < 120.0f)
    {
        int skip = (int)floor( (t-90.0f) / 5.0f) +1;
        fill_skipped_rect(0,0,128,96,skip,skip,0);
    }
    
}

/// Get intro scene
SCENE get_intro_scene()
{
    SCENE s = (SCENE){intro_init,intro_update,intro_draw,NULL};

    set_scene_name(&s,"intro");

    return s;
}
