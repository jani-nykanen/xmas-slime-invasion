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
    pl->shooting = (vpad_get_button(0) == DOWN || (vpad_get_button(3) == DOWN && pl->crystals > 0) || pl->sprArm.frame > 0);

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
            bool mode = vpad_get_button(3) == DOWN;

            if(mode &&  pl->crystals <= 0) mode = false;
            
            if(mode)
                 pl->crystals --;

            int type = mode ? 1 : 0;
            float speed = mode ? 2.0f : 4.0f;

            int loop = (pl->powerUpId == 0 && pl->powerUpTimer > 0.0f) ? 2: 1;
            int deltaY = loop == 1 ? 0 : -2;
            if(!mode && pl->powerUpId == 1 && pl->powerUpTimer > 0.0f)
                 loop = 0;

            int i = 0;
            for(; i < loop; ++ i)
            {
                BULLET* b = get_next_bullet();
                put_bullet(b,vec2(pl->pos.x + (pl->spinning ? -1 : 1) +7,pl->pos.y-16 +8 + deltaY + i * 4),speed,type);
            }
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

/// Die
/// < pl Player
/// < tm Time mul.
static void pl_die(PLAYER* pl, float tm)
{
    int oldFrame = pl->spr.frame;

    float speed = 4.0f;
    if(pl->spr.frame == 0) speed = 30.0f;
    else if(pl->spr.frame == 7) speed = 60.0f;

    pl->hurtTimer = 0.0f;
    spr_animate(&pl->spr,7,0,8,speed,tm);

    if(oldFrame == 0 && pl->spr.frame > 0)
    {
        create_blood_effect(vec2(pl->pos.x,pl->pos.y-8),64,0b00010000);
    }

    if(pl->spr.frame == 8)
    {
        pl->dying = false;
    }
}

/// Draw laser power up
/// < pl Player
void pl_draw_laser(PLAYER* pl)
{
    const int colors[] = {0b0011011, 0b0101111, 63, 0b0101111};

    if(!pl->shooting) return;

    int index = (int)floor(pl->powerUpTimer) % 4;

    fill_rect(round(pl->pos.x) + (pl->spinning ? -1 : 1) + 8,round(pl->pos.y)-9,128,3 , colors[index]);
    fill_rect(round(pl->pos.x) + (pl->spinning ? -1 : 1) + 8,round(pl->pos.y)-9 +1,128,1 , colors[ (index + 1) % 4 ] );
}

/// Create a player object
PLAYER create_player()
{
    if(bmpPlayer == NULL)
        bmpPlayer = get_bitmap("figure");

    PLAYER pl;
    pl.pos = vec2(-8.0f,96-12);
    pl.speed = vec2(0.0f,0.0f);
    pl.target = pl.speed;
    pl.canJump = true;
    pl.doubleJump = false;
    pl.spr = create_sprite(16,16);
    pl.sprArm = create_sprite(16,16);
    pl.shooting = false;
    pl.teleporting = false;
    pl.crystals = 0;
    pl.health = 3;
    pl.hurtTimer = 0.0f;
    pl.dead = false;
    pl.dying = false;
    pl.powerUpId = 0;
    pl.powerUpTimer = 0.0f;
    pl.startPosReached = false;

    return pl;
}

/// Update player
void pl_update(PLAYER* pl, float tm)
{
    if(pl->dead)
    {
        if(pl->dying)
            pl_die(pl,tm);

        return;
    }

    if(pl->startPosReached)
    {
        pl_control(pl);
        pl_move(pl,tm);
    }
    else
    {
        pl->pos.x += 0.5f * tm;
        if(pl->pos.x > 24.0f)
        {
            pl->pos.x = 24.0f;
            pl->startPosReached = true;
        }
    }
    pl_animate(pl,tm);

    if(pl->hurtTimer > 0.0f)
        pl->hurtTimer -= 1.0f * tm;

    if(pl->powerUpTimer > 0.0f)
        pl->powerUpTimer -= (0.15f + 0.05f*pl->powerUpId) * tm;

    if(pl->health <= 0)
    {
        pl->dead = true;
        pl->dying = true;
        pl->spr.frame = 0;
        pl->spr.row = 7;
        pl->spr.count = 0.0f;
    }
}

/// Draw player
void pl_draw(PLAYER* pl)
{
    if(pl->dead && !pl->dying) return;

    bool cond = pl->hurtTimer > 0.0f && (int)floor(pl->hurtTimer/4) % 2 == 0;
    if(cond)
    {
        pl->spr.frame += 8;
        pl->sprArm.frame += 16;
    }

    if(pl->shooting && !pl->dying)
        spr_draw(&pl->sprArm,bmpPlayer,round(pl->pos.x) + (pl->spinning ? -1 : 1),round(pl->pos.y)-16,0);

    spr_draw(&pl->spr,bmpPlayer,round(pl->pos.x)-8,round(pl->pos.y)-16,0);

    if(!pl->dead && pl->powerUpId == 1 && pl->powerUpTimer > 0.0f)
        pl_draw_laser(pl);

    if(cond)
    {
        pl->spr.frame -= 8;
        pl->sprArm.frame -= 16;
    }
}

/// Make him/her suffer
void pl_hurt(PLAYER* pl, VEC2 p, VEC2 d)
{
    if(pl->hurtTimer > 0.0f || pl->teleporting) return;

    if(pl->pos.x+4 > p.x && pl->pos.x-4 < p.x+d.x
        && pl->pos.y-2 > p.y && pl->pos.y-12 < p.y+d.y)
    {
        pl->hurtTimer = 60.0f;
        pl->health --;
    }
}