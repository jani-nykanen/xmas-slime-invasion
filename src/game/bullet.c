/// Bullet (source)
/// (c) 2017 Jani Nykänen

#include "bullet.h"

#include "../engine/graphics.h"
#include "../engine/assets.h"

/// Bullet bitmap
static BITMAP* bmpBullet =NULL;

/// Create a new bullet instance (but does not
BULLET create_bullet()
{
    if(bmpBullet == NULL)
        bmpBullet = get_bitmap("bullet");

    BULLET b;
    b.exist = false;
    b.spr = create_sprite(12,12);
    b.dying = false;
    return b;
}

/// Put a bullet to the screen
void put_bullet(BULLET* b, VEC2 pos, float speed, int id)
{
    b->exist = true;
    b->dying = false;

    b->pos = pos;
    b->speed = speed;
    b->spr.frame = 0;
    b->id = id;
}

/// Update a bullet
void bullet_update(BULLET* b, float tm)
{
    if(b->exist == false)
    {   
        if(b->dying)
        {
            spr_animate(&b->spr,b->id*2 + 1,0,5,2,tm);
            if(b->spr.frame == 5)
            {
                b->dying = false;
            }
        }

        return;
    }

    b->pos.x += b->speed * tm;
    if(b->pos.x > 128)
    {
        b->exist = false;
    }
    
    spr_animate(&b->spr,b->id*2,0,1 + b->id*2,3,tm);
}

/// Draw a bullet
void bullet_draw(BULLET* b)
{
    if(b->exist == false)
    {
        if(b->dying)
        {
            spr_draw(&b->spr,bmpBullet,(int)round(b->pos.x+5),(int)round(b->pos.y) -5, 0);
        }
        return;
    }

    spr_draw(&b->spr,bmpBullet,(int)round(b->pos.x),(int)round(b->pos.y) -6, 0);
}
