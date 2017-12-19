/// Blood (source)
/// (c) 2017 Jani Nykänen

#include "blood.h"

#include "../engine/graphics.h"

/// Create a new blood instance (but does not
BLOOD create_blood()
{
    BLOOD b;
    b.exist = false;
    return b;
}

/// Put a blood to the screen
void put_blood(BLOOD* b, VEC2 pos, VEC2 speed, Uint8 color)
{
    const float MAX_TIMER = 60.0f;

    b->pos = pos;
    b->speed = speed;
    b->exist = true;
    b->timer = MAX_TIMER;
    b->radius = rand() % 2 + 1;
    b->color = color;
}

/// Update "a" blood
void blood_update(BLOOD* b, float tm)
{
    if(!b->exist) return;

    b->speed.y += 0.05f * tm;
    if(b->speed.y >= 1.5f)
        b->speed.y = 1.5f;

    b->pos.x += b->speed.x * tm;
    b->pos.y += b->speed.y * tm;

    b->timer -= 1.0f * tm;
    if(b->timer <= 0.0f) b->exist = false;
}

/// Draw a blood
void blood_draw(BLOOD* b)
{
    if(!b->exist) return;

    fill_rect(b->pos.x,b->pos.y,b->radius,b->radius,b->color);
}