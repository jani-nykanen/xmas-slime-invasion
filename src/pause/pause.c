/// Pause scene (source)
/// (c) 2017 Jani Nykänen

#include "pause.h"

#include "../engine/graphics.h"
#include "../engine/assets.h"
#include "../engine/app.h"

#include "../vpad.h"

#include "stdlib.h"
#include "stdbool.h"

/// Font bitmap
static BITMAP* bmpFont;
/// Cursor bitmap
static BITMAP* bmpCursor;

/// Cursor position
static bool cursorPos;
/// Stick "released"
static bool stickReleased;

/// Initialize pause scene
/// > Always 0
static int pause_init()
{
    bmpFont = get_bitmap("font");
    bmpCursor = get_bitmap("cursor");

    cursorPos = false;
    stickReleased = true;

    return 0;
}

/// Update pause scene
/// < tm Time multiplier
static void pause_update(float tm)
{
    if(vpad_get_button(4) == PRESSED || vpad_get_button(1) == PRESSED)
    {
        if(!cursorPos)
            app_swap_scene("game");
        else
        {
            app_swap_scene("title");
        }
    }

    float stick = vpad_get_stick().y * (cursorPos ? -1 : 1);
    if(stick > 0.25f )
        cursorPos = !cursorPos;
}

/// Draw pause scene
static void pause_draw()
{
    fill_rect(16,22,96,52,0b00010101);

    draw_text(bmpFont,(Uint8*)"GAME PAUSED",11,64,24,-1,0,true);
    draw_text(bmpFont,(Uint8*)"Resume",6,64,40,-1,0,true);
    draw_text(bmpFont,(Uint8*)"Quit",6,64,54,-1,0,true);

    int p = cursorPos ? 55 : 41;
    draw_bitmap(bmpCursor,26,p,0);

}

/// Scene swapped
static void pause_on_swap()
{
    cursorPos = 0;
}

/// Get pause scene
/// > Pause scene
SCENE get_pause_scene()
{
    SCENE s = (SCENE) {pause_init,pause_update,pause_draw, NULL, pause_on_swap};
    set_scene_name(&s,"pause");

    return s;
}
