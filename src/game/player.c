/// Player (source)
/// (c) 2017 Jani Nykänen

#include "player.h"

#include "../engine/assets.h"
#include "../engine/graphics.h"

#include "../vpad.h"

#include "stage.h"
#include "game.h"

#include "math.h"

/// Player bitmap
static BITMAP* bmpPlayer = NULL;

/// Control
static void pl_control(PLAYER*pl)
{
    pl->shooting = (vpad_get_button(0) == DOWN || pl->sprArm.frame > 0);

    pl->target.x = vpad_get_stick().x;

    if(!pl->teleporting && vpad_get_button(2) == PRESSED)
    {
        pl->teleporting = true;
        pl->spr.frame = 0;
        pl->spr.row = 6;
        pl->released = false;
        pl->speed.x = pl->target.x;
    }

    if(pl->teleporting)
    {
        if(vpad_get_button(2) == UP) pl->released = true;

        pl->shooting = false;
        return;
    }

    if(vpad_get_button(1) == PRESSED)
    {
        if(pl->canJump)
        {
            pl->speed.y = -2.45f;
            pl->doubleJump = true;
            pl->canJump = false;
        }
        else if(pl->doubleJump)
        {
            pl->speed.y = -1.95f;
            pl->doubleJump = false;
        }
    }

    pl->spinning = (vpad_get_button(1) == DOWN && !pl->doubleJump && pl->speed.y > 0.0f && !pl->canJump);
    
    if(!pl->canJump && vpad_get_button(1) == RELEASED && pl->speed.y < 0.0f)
    {
        pl->speed.y /= 1.75f;
    }
}

/// Animate
static void pl_animate(PLAYER*pl, float tm)
{
    if(pl->teleporting)
    {
        spr_animate(&pl->spr,6,0,8,!pl->released ? 5 : 3,tm);
        if(pl->spr.frame == 8)
        {
            pl->teleporting = false;
            pl->spr.row = 0;
            pl->spr.frame = 0;
        }
        else
        {
            return;
        }
    }

    if(pl->shooting)
    {
        int lastFrame = pl->sprArm.frame;
        spr_animate(&pl->sprArm,pl->spinning ? 5 : 4,0,5,2,tm);

        if(pl->sprArm.frame == 1 && lastFrame == 0)
        {
            BULLET* b = get_next_bullet();
            put_bullet(b,vec2(pl->pos.x + (pl->spinning ? -1 : 1) +7,pl->pos.y-16 +8),4.0f);
        }
    }

    if(pl->canJump)
    {
        float speed = get_global_speed() + pl->speed.x * 0.5f;
        spr_animate(&pl->spr,0,0,5,5.0f-speed,tm);
    }
    else
    {
        if(pl->spinning)
        {
            spr_animate(&pl->spr,3,0,3,3,tm);
        }
        else
        {

            if(pl->doubleJump || pl->speed.y > 0.0f)
            {
                int frame = fabs(pl->speed.y) > 0.25f ? (pl->speed.y < 0.0f ? 0 : 2) : 1;
                spr_animate(&pl->spr,1,frame,frame,0,tm);
            }
            else 
            {
                spr_animate(&pl->spr,2,0,3,3,tm);
            }

        }
    }
}

/// Move
static void pl_move(PLAYER* pl, float tm)
{
    const float ACC_X = 0.075f;
    const float ACC_Y = 0.075f;

    pl->speed.y += ACC_Y * tm;
    if(pl->speed.y > 2.0f)
        pl->speed.y = 2.0f;
    if(pl->spinning)
        pl->speed.y = 0.25f;
    if(pl->teleporting)
        pl->speed.y = 0.0f;
    
    if(pl->target.x > pl->speed.x)
    {
        pl->speed.x += ACC_X * tm;
        if(pl->speed.x > pl->target.x)
            pl->speed.x = pl->target.x;
    }
    else if(pl->target.x < pl->speed.x)
    {
        pl->speed.x -= ACC_X * tm;
        if(pl->speed.x < pl->target.x)
            pl->speed.x = pl->target.x;
    }

    pl->pos.x += pl->speed.x * tm;
    pl->pos.y += pl->speed.y * tm;

    // Floor collision
    pl->canJump = false;
    if(pl->pos.y > 96-12)
    {
        pl->pos.y = 96-12;
        pl->canJump = true;
        pl->speed.y = 0.0f;
        pl->doubleJump = false;
        pl->spinning = false;
    }
    // Wall collisions
    if(pl->pos.x < 5 && pl->speed.x < 0.0f)
    {
        pl->pos.x = 5;
        pl->speed.x = 0.0f;
    }
    else if(pl->pos.x > 128-5 && pl->speed.x > 0.0f)
    {
        pl->pos.x = 128-5;
        pl->speed.x = 0.0f;
    }
}

/// Create a player object
PLAYER create_player()
{
    if(bmpPlayer == NULL)
        bmpPlayer = get_bitmap("figure");

    PLAYER pl;
    pl.pos = vec2(32.0f,96-12);
    pl.speed = vec2(0.0f,0.0f);
    pl.target = pl.speed;
    pl.canJump = true;
    pl.doubleJump = false;
    pl.spr = create_sprite(16,16);
    pl.sprArm = create_sprite(16,16);
    pl.shooting = false;
    pl.teleporting = false;
    pl.crystals = 0;

    return pl;
}

/// Update player
void pl_update(PLAYER* pl, float tm)
{
    pl_control(pl);
    pl_move(pl,tm);
    pl_animate(pl,tm);
}

/// Draw player
void pl_draw(PLAYER* pl)
{
    if(pl->shooting)
        spr_draw(&pl->sprArm,bmpPlayer,round(pl->pos.x) + (pl->spinning ? -1 : 1),round(pl->pos.y)-16,0);

    spr_draw(&pl->spr,bmpPlayer,round(pl->pos.x)-8,round(pl->pos.y)-16,0);
}

/// Make him/her suffer
void pl_hurt(PLAYER* pl, VEC2 p, VEC2 d)
{
    // Kill the player here
}