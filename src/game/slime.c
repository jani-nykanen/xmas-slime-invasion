/// Slime (source)
/// (c) 2017 Jani Nykänen

#include "slime.h"

#include "../engine/graphics.h"
#include "../engine/assets.h"

#include "stage.h"

#include "time.h"
#include "math.h"
#include "stdlib.h"

/// Slime bitmap
static BITMAP* bmpSlime = NULL;

/// Animate a slime
/// < s Slime to animate
/// < tm Time mul.
static void slime_animate(SLIME* s, float tm)
{   
    switch(s->id)
    {
    case 8:
    case 4:
    case 1:
    case 0:
        if(s->canJump)
            s->spr.frame = 0;
        else
            s->spr.frame = s->speed.y < 0.0f ? 1 : 2;
        break;  

    case 2:
        spr_animate(&s->spr,s->id*3,0,3,5,tm);
        break;

    case 3:
        if(s->canJump)
        {
            spr_animate(&s->spr,s->id*3,0,3,5,tm);
        }
        else
            s->spr.frame = s->speed.y > 0.0f ? 5 : 4;
        break;

    case 9:
    case 7:
    case 6:
    case 5:
        spr_animate(&s->spr,s->id*3,0,3,3,tm);
        break;
    
    default: 
        break;
    }
}

/// Jump routine
/// < s Slime
/// < tm Time mul.
static bool jump_routine(SLIME* s, float tm)
{
    if(s->canJump)
    {
        s->spcTimer -= 1.0f * tm;
        if(s->spcTimer <= 0.0f)
        {
            float height = (float)(rand() % 100)/100.0f * 1.5f + 1.5f;
             s->speed.y = -height;
             s->spcTimer = s->id != 4 ? (float)(rand() % 30 + 15 + 10 * s->id) : 0;
             return true;
        }
    }

    return false;
}

/// Flying routine
/// < s Slime
/// < tm Time mul.
static void fly_routine(SLIME* s, float tm)
{
    s->spcTimer += 1.0f * tm;
    s->pos.y = s->startY + s->spcValue1 * sin(s->spcTimer / (s->spcValue2));
}

/// Create a new slime instance
SLIME create_slime()
{
    if(bmpSlime == NULL) bmpSlime = get_bitmap("slimes");

    SLIME s;
    s.dead = true;
    s.dying = false;
    s.spr = create_sprite(16,16);
    return s;
}

/// Put a slime to the screen
void put_slime(SLIME* s, VEC2 pos, int id)
{
    s->dead = false;
    s->dying = false;
    s->spr.row = id *3;
    s->pos = pos;
    s->hurtTimer = 0.0f;
    s->id = id;
    s->speed.y = 0.0f;
    s->startY = pos.y;
    s->canJump = false;

    s->health = 3; 
    s->speed.x = -get_global_speed();

    switch(id)
    {
    case 8:
        s->health = 1;
    case 4:
    case 1:
    case 0:
        s->spcTimer = id == 4 ? 0 : (float)(rand() % 60 + 30);
        break;

    case 3:
    case 2:
        s->spcTimer = (float)(rand() % 30 + 10);
        break;

    case 9:
        s->health = 1;
    case 5:
        s->spcValue1 = (float)(rand() % 12 + 8);
        s->spcValue2 = (float)(rand() % 32 + 8);
        break;

    case 7:

        s->spcValue1 = (float)(rand() % 100)/100.0f + 0.5f;
        s->spcValue2 = (float)(rand() % 100)/200.0f + 0.25f;
        
        break;

    default:
        break;
    }
}

/// Update a slime
void slime_update(SLIME* s, float tm)
{
    if(s->dead)
    {
        if(s->dying)
        {
            spr_animate(&s->spr,2 + s->id*3,0,6,3,tm);
            if(s->spr.frame == 5)
                s->dying = false;
        }
        return;
    }

    s->target.x = -get_global_speed();
    s->target.y = 2.0f;

    // Animate
    slime_animate(s,tm);

    float accX = 0.075f;
    float accY = 0.075f;

    // Update
    switch(s->id)
    {
    case 8:
    case 0:
        jump_routine(s,tm);
        break;

    case 1:
        if(jump_routine(s,tm))
        {
            s->speed.x = get_global_speed() / 10.0f;
        }

        if(!s->canJump)
            s->target.x = get_global_speed() / 10.0f;
        break;

    case 4:
        jump_routine(s,tm);
        s->target.x *= 1.3f;
        break;

    case 3:
        jump_routine(s,tm);
    case 2:
        s->target.x *= 1.6f;
        accX = 0.035f;
        break;

    case 6:
        s->target.x *= 1.6f;
        accX = 0.035f;
        s->target.y = 0.0f;
        s->speed.y = 0.0f;
        break;

    case 9:
    case 5:
        fly_routine(s,tm);
        s->speed.y = 0.0f;
        s->target.y = 0.0f;
        break;

    case 7:

        if(s->canJump)
        {
            s->id = 0;
            s->spr.row = 0;
            s->spr.frame = 0;
        }

        s->target.y = s->spcValue1;
        s->speed.y = s->target.y;
        s->target.x = s->spcValue2 * -get_global_speed();
        break;

    default: 
        break;
    }
    
    if(s->target.x > s->speed.x)
    {
        s->speed.x += accX * tm;
        if(s->speed.x > s->target.x)
            s->speed.x = s->target.x;
    }
    else if(s->target.x < s->speed.x)
    {
        s->speed.x -= accX * tm;
        if(s->speed.x < s->target.x)
            s->speed.x = s->target.x;
    }

    if(s->target.y > s->speed.y)
    {
        s->speed.y += accY * tm;
        if(s->speed.y > s->target.y)
            s->speed.y = s->target.y;
    }
    else if(s->target.x < s->speed.y)
    {
        s->speed.y -= accY * tm;
        if(s->speed.y < s->target.y)
            s->speed.y = s->target.y;
    }

    s->pos.x += s->speed.x * tm;
    s->pos.y += s->speed.y * tm;

    s->canJump = false;
    if(s->pos.y > 96-12 && s->speed.y > 0.0f)
    {
        s->pos.y = 96-12;
        s->speed.y = 0.0f;
        s->canJump = true;
    }

    if(s->pos.x < -8)
    {
        s->dead = true;
        s->dying = false;
    }

    if(s->hurtTimer > 0.0f)
        s->hurtTimer -= 1.0f * tm;
}

/// Get collision with game objects
void slime_collision(SLIME* s, PLAYER* pl, BULLET* bullets, int bulletLength)
{
    if(s->dead)
        return;

    // Bullet collision
    int i = 0;
    for(; i < bulletLength; i++)
    {
        BULLET b = bullets[i];
        if(b.exist == false) continue;

        if(b.pos.x+10 > s->pos.x-6 && b.pos.x < s->pos.x+6
            && b.pos.y-1 > s->pos.y-14 && b.pos.y+1 < s->pos.y)
        {
            bullets[i].exist = false;
            bullets[i].dying = true;
            s->hurtTimer = 30.0f;
            s->health --;
            s->speed.x = 1.0f;
            if(s->health <= 0)
            {
                s->dead = true;
                s->dying = true;
                s->spr.frame = 0;
                s->spr.row = s->id*3 +2;
            }
        }
    }
}

/// Slime to slime collision
void slime_to_slime_collision(SLIME* s, SLIME* o)
{
    if(s->dead || o->dead) return;

    float dist = (float)hypot(s->pos.x-o->pos.x,s->pos.y-o->pos.y);
    if(dist < 8)
    {
        float angle = (float)atan2(s->pos.y-o->pos.y,s->pos.x-o->pos.x);
        o->pos.x -= cos(angle) * (8-dist)* 0.5f;
        o->pos.y -= sin(angle) * (8-dist)* 0.5f;

        if(o->pos.y > 96-12) o->pos.y = 96-12;

        s->pos.x += cos(angle) * (8-dist) * 0.5f;
        s->pos.y += sin(angle) * (8-dist) * 0.5f;

        if(s->pos.y > 96-12) s->pos.y = 96-12;
    }
}

/// Draw a slime
void slime_draw(SLIME* s)
{
    if(s->dead && !s->dying) return;

    int row = s->spr.row;

    if(s->hurtTimer > 0.0f && !s->dead && (int)floor(s->hurtTimer/4) % 2 == 0)
        s->spr.row ++;

    spr_draw(&s->spr,bmpSlime,(int)round(s->pos.x-8),(int)round(s->pos.y-15),0);
    s->spr.row = row;
}