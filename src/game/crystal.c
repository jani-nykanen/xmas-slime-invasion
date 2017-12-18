/// Crystal (source)
/// (c) 2017 Jani Nykänen

#include "crystal.h"

#include "../engine/graphics.h"
#include "../engine/assets.h"

#include "stage.h"

/// Crystal bitmap
static BITMAP* bmpCrystal =NULL;

/// Create a new crystal instance
CRYSTAL create_crystal()
{
    if(bmpCrystal == NULL) bmpCrystal = get_bitmap("crystal");

    CRYSTAL c;
    c.exist = false;
    c.vanishing = false;

    c.spr = create_sprite(12,12);

    return c;
}

/// Put a crystal to the screen
void put_crystal(CRYSTAL* c, VEC2 pos, VEC2 speed)
{
    c->exist = true;
    c->pos = pos;
    c->speed = speed;
    c->spr.frame = 0;
    c->spr.row = 0;
}

/// Update a crystal
void crystal_update(CRYSTAL* c, PLAYER* pl, float tm)
{
    if(!c->exist)
    {
        if(c->vanishing)
        {
            spr_animate(&c->spr,1,0,5,5,tm);
            if(c->spr.frame == 5)
                c->vanishing = false;
        }
        return;
    }

    if(c->speed.y < 2.0f)
    {
        c->speed.y += 0.075f;
        if(c->speed.y > 2.0f)
            c->speed.y = 2.0f;
    }

    float targetX = -get_global_speed();
    float accX = 0.05f;
    if(c->speed.x > targetX)
    {
        c->speed.x -= accX * tm;
        if(c->speed.x < targetX)
            c->speed.x = targetX;
    }
    else if(c->speed.x < targetX)
    {
        c->speed.x += accX * tm;
        if(c->speed.x > targetX)
            c->speed.x = targetX;
    }

    c->pos.x += c->speed.x *tm;
    c->pos.y += c->speed.y *tm;

    if(c->pos.x < -6)
    {
        c->exist = false;
    }

    if(c->speed.y >= 0.0f && c->pos.y > 96-12)
    {
        c->speed.y /= -1.5f;
        c->pos.y = 96-12;
    }

    spr_animate(&c->spr,0,0,3,8,tm);

    if(pl->pos.x+6 >= c->pos.x-4 && pl->pos.x-6 <= c->pos.x-4
     && pl->pos.y >= c->pos.y-12 && pl->pos.y-12 <= c->pos.y)
    {
        c->exist = false;
        c->vanishing = true;
        c->spr.frame = 0;
        c->spr.row = 1;
        if(pl->crystals < 99)
            pl->crystals ++;
        
    }
}

/// Draw a crystal
void crystal_draw(CRYSTAL* c)
{
    if(!c->exist && !c->vanishing) return;

    spr_draw(&c->spr,bmpCrystal,(int)round(c->pos.x)-6,(int)round(c->pos.y)-11,0);
}