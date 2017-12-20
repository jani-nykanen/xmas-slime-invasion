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

/// Initialize pause scene
/// > Always 0
static int pause_init()
{
    bmpFont = get_bitmap("font");

    return 0;
}

/// Update pause scene
/// < tm Time multiplier
static void pause_update(float tm)
{
    if(vpad_get_button(4) == PRESSED)
    {
        app_swap_scene("game");
    }
}

/// Draw pause scene
static void pause_draw()
{
    fill_rect(16,32,96,32,0);

    draw_text(bmpFont,(Uint8*)"GAME PAUSED",11,64,36,-1,0,true);
}

/// Get pause scene
/// > Pause scene
SCENE get_pause_scene()
{
    SCENE s = (SCENE) {pause_init,pause_update,pause_draw, NULL};
    set_scene_name(&s,"pause");

    return s;
}
