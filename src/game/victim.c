/// Victim (source)
/// (c) 2017 Jani Nykänen

#include "victim.h"

#include "../engine/graphics.h"
#include "../engine/assets.h"

#include "stage.h"
#include "game.h"

/// Victim bitmap
static BITMAP* bmpVictim =NULL;

/// Die
/// < v Victim
static void victim_die(VICTIM* v)
{
    v->dead = true;
    v->dying = true;
    v->spr.frame = 0;
    v->spr.row = 3;

    create_blood_effect(vec2(v->pos.x,v->pos.y-8),rand() % 16 + 16,0b00100000);

    add_kill();
    shake_screen();
}

/// Create a new victim instance
VICTIM create_victim()
{
    if(bmpVictim == NULL) bmpVictim = get_bitmap("victim");

    VICTIM v;
    v.dead = true;
    v.dying = false;

    v.spr = create_sprite(12,16);
    v.sprWarning = create_sprite(16,16);

    return v;
}

/// Put a victim to the screen
void put_victim(VICTIM* v, VEC2 pos, int id)
{
    v->pos = pos;
    v->id = id;
    v->dead = false;
    v->dying = false;
    v->jumpTimer = (float)(rand() % 120 + 30);
}

/// Update a victim
void victim_update(VICTIM* v, float tm)
{
    if(v->dead)
    {
        if(v->dying)
        {
            spr_animate(&v->spr,3,0,6,3,tm);
            if(v->spr.frame == 6)
                v->dying = false;
        }
        return;
    }

    v->speed.x = v->id == 0 ? -get_global_speed() * 1.5f : (-get_global_speed() + 0.5f);
    v->speed.y += 0.075f * tm;
    if(v->speed.y > 2.0f)
        v->speed.y = 2.0f;

    v->pos.x += v->speed.x * tm;
    v->pos.y += v->speed.y * tm;

    if(v->speed.y > 0.0f && v->pos.y > 96-12)
    {
        v->pos.y = 96-12;
        v->canJump = true;
        v->speed.y = 0.0f;
    }

    if(v->canJump)
    {
        spr_animate(&v->spr,0,0,5,4 - v->id*1,tm);

        v->jumpTimer -= 1.0f * tm;
        if(v->jumpTimer <= 0.0f)
        {
            v->speed.y = -(float)(rand() % 100)/100 - 1.25f;
            v->jumpTimer = (float)(rand() % 120 + 60);
        }
    }
    else
    {
        int row = v->speed.y < 0.0f ? 2 : 1;
        spr_animate(&v->spr,row,0,5,4,tm);
    }

    if(v->pos.x < -6)
    {
        v->dead = true;
        v->dying = false;
    }

    spr_animate(&v->sprWarning,4,0,1,15,tm);
}

/// Get collision with game objects
void victim_collision(VICTIM* v, BULLET* bullets, int bulletLength)
{
    if(v->dead) return;

    // Bullet collision
    int i = 0;
    for(; i < bulletLength; i++)
    {
        BULLET b = bullets[i];
        if(b.exist == false) continue;

        if(b.pos.x+10 > v->pos.x-6 && b.pos.x < v->pos.x+6
            && b.pos.y-1 > v->pos.y-14 && b.pos.y+1 < v->pos.y)
        {
            bullets[i].exist = false;
            bullets[i].dying = true;
            victim_die(v);
        }
    }
}

/// Draw a victim
void victim_draw(VICTIM* v)
{
    if(v->dead && !v->dying) return;

    spr_draw(&v->sprWarning,bmpVictim,(int)round(v->pos.x-8),(int)round(v->pos.y-15 - 17), 0);
    spr_draw(&v->spr,bmpVictim,(int)round(v->pos.x-6),(int)round(v->pos.y-15), v->id);
}