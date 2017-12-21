/// Title scene (source)
/// (c) 2017 Jani Nykänen

#include "title.h"

#include "../engine/app.h"
#include "../engine/graphics.h"
#include "../engine/bitmap.h"
#include "../engine/controls.h"
#include "../engine/assets.h"
#include "../engine/transform.h"

#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include "time.h"

#include "../vpad.h"

#include "../game/game.h"

/// Background frame
static FRAME* frameBg;
/// Logo bitmap
static BITMAP* bmpLogo;
/// Font bitmap
static BITMAP* bmpFont;
/// Is the frame drawn
static bool frameDrawn;
/// Float timer
static float floatTimer;
/// Title phase
static int titlePhase;
/// Title timer
static float titleTimer;

/// Swap scene event
static void title_on_swap()
{
    titlePhase = 0;
    floatTimer = 0.0f;
    titleTimer = 0.0f;
}

/// Initialize title scene
/// > 0 on success
static int title_init()
{
    frameBg = frame_create(128,96);
    if(frameBg == NULL)
    {
        return 1;
    }
    frameDrawn = false;

    bmpLogo = get_bitmap("logo");
    bmpFont = get_bitmap("font2");

    title_on_swap();

    return 0;
} 

/// Update title scene
/// < tm Time mul.
static void title_update(float tm)
{
    

    switch(titlePhase)
    {
    case 0:
    {
        titleTimer += 1.0f * tm;
        if(titleTimer >= 60.0f)
        {
            titlePhase ++;
            titleTimer = 60.0f;
        }
    }
    break;
    
    case 1:
        floatTimer += 0.033f * tm;
        if(any_pressed())
        {
            titlePhase ++;
            
        }
        break;

    case 2:
    {
        if(vpad_get_button(4) == PRESSED || vpad_get_button(1) == PRESSED)
        {
            titlePhase ++;
            floatTimer = 0.0f;
        }
    }
    break;

    case 3:
    {
        titleTimer -= 1.0f * tm;
        if(titleTimer <= 00.0f)
        {
            invert_frame(frameBg);
            copy_frame(frameBg,get_gameover_frame());
            invert_frame(frameBg);

            game_recreate();
            
            app_swap_scene("game");
        }
    }
    break;

    default:
        break;
    }
}

/// Draw title screen
static void title_draw()
{
    float t = titleTimer / 60.0f;

    if(!frameDrawn)
    {
        bind_frame(frameBg);
        game_draw();
        bind_frame(app_get_canvas());
        frameDrawn = true;
    }

    clear_frame(0b00101010);

    draw_inverted_bitmap((BITMAP*)frameBg,0,0,0);

    float scale = 1.0f + sin(floatTimer*1.5f)*0.05f;
    int y = 2 + (int)(4*sin(floatTimer));
    if(titlePhase == 0 || titlePhase == 3)
    {
        y = -56 + 58*t;
    }
    draw_scaled_bitmap_region(bmpLogo,0,0,128,96,-128*(scale-1.0f)/2,y -96*(scale-1.0f)/2,128*scale,96*scale);

    y = titlePhase == 1 ? 72 : (int)(96 - 24*t);
    if(titlePhase == 1 && (int)floor(floatTimer) %2 == 0)
    {
        draw_text(bmpFont,(Uint8*)"Press Any Key",13,64,y,-1,0,true);
    }
}

/// Get title scene
SCENE get_title_scene()
{
    SCENE s = (SCENE){title_init,title_update,title_draw,NULL,title_on_swap};

    set_scene_name(&s,"title");

    return s;
}
