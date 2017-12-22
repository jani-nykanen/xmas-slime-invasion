/// Cotper (source)
/// (c) 2017 Jani Nykänen

#include "copter.h"

#include "../engine/graphics.h"
#include "../engine/assets.h"

#include "stage.h"
#include "game.h"

/// Copter bitmap
static BITMAP* bmpCopter =NULL;

/// Power up sample
static SAMPLE* smpPowerup;

/// Copter dying
/// < c Copter
/// < pl Player
static void copter_die(COPTER* c, PLAYER* pl)
{
    if(c->sprPower.frame < 2)
    {
        pl->powerUpTimer = 60.0f;
        pl->powerUpId = c->sprPower.frame;
    }
    else if(pl->health < 3)
    {
        ++ pl->health; 
    }
}

/// Create a new copter instance
COPTER create_copter()
{
    if(bmpCopter == NULL)
    {
        bmpCopter = get_bitmap("powerup");
        smpPowerup = get_sample("powerup");
    }

    COPTER c;
    c.dead = true;
    c.dying = false;

    c.spr = create_sprite(24,24);
    c.sprPower = create_sprite(12,12);

    return c;
}

/// Put a copter to the screen
void put_copter(COPTER* c, VEC2 pos)
{
    c->pos = pos;
    c->startY = pos.y;
    c->dead = false;
    c->dying = false;
    c->waveTimer = 0.0f;
}

/// Update a copter
void copter_update(COPTER* c, float tm)
{
    if(c->dead)
    {
        if(c->dying)
        {
            spr_animate(&c->spr,2,0,7,c->spr.frame == 6 ? 30 : 3,tm);
            if(c->spr.frame == 7)
                c->dying = false;
        }
        return;
    }

    spr_animate(&c->spr,1,0,3,4,tm);
    spr_animate(&c->sprPower,0,0,2,30,tm);

    c->pos.x += -get_global_speed() * tm;

    c->waveTimer += 0.05f *tm;
    c->pos.y = c->startY + 16.0f * sin(c->waveTimer);

    if(c->pos.x < -12)
    {
        c->dead = true;
        c->dying = false;
    }
}

/// Get collision with game objects
void copter_collision(COPTER* c, PLAYER* pl, BULLET* bullets, int bulletLength)
{
    if(c->dead) return;

    // Bullet collision
    int i = 0;
    for(; i < bulletLength; i++)
    {
        BULLET b = bullets[i];
        if(b.exist == false) continue;

        if(b.pos.x+10 > c->pos.x-12 && b.pos.x < c->pos.x+12
            && b.pos.y-1 > c->pos.y-8 && b.pos.y+1 < c->pos.y+12)
        {
            bullets[i].exist = false;
            bullets[i].dying = true;
            c->dead = true;
            c->dying = true;
            c->spr.frame = 0;
            c->spr.row = 2;

            copter_die(c,pl);
            play_sample(smpPowerup,0.65f);
        }
    }
}

/// Draw a copter
void copter_draw(COPTER* c)
{
    if(c->dead && !c->dying) return;

    spr_draw(&c->spr,bmpCopter,(int)round(c->pos.x-12),(int)round(c->pos.y-12), 0);
    spr_draw(&c->sprPower,bmpCopter,(int)round(c->pos.x-6),(int)round(c->pos.y-3), 0);
}