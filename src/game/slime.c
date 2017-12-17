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
    
    default: 
        break;
    }
}

/// Jump routine
/// < s Slime
/// < tm Time mul.
static bool jump_routine(SLIME* s, float tm)
{
    s->target.y = 2.0f;
    if(s->canJump)
    {
        s->spcTimer -= 1.0f * tm;
        if(s->spcTimer <= 0.0f)
        {
            float height = (float)(rand() % 100)/100.0f * 2.0f + 1.0f;
             s->speed.y = -height;
             s->spcTimer = (float)(rand() % 30 + 15 + 10 * s->id);
             return true;
        }
    }

    return false;
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

    switch(id)
    {
    case 1:
    case 0:
        s->speed.x = -get_global_speed();
        s->spcTimer = (float)(rand() % 60 + 30);
        s->health = 3;
        break;

    case 2:
        s->speed.x = -get_global_speed();
        s->target.y = 0.0f;
        s->health = 3;  
        s->speed.y = 0.0f;
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

    // Animate
    slime_animate(s,tm);

    float acc = 0.075f;

    // Update
    switch(s->id)
    {
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

    case 2:
        s->target.x *= 1.5f;
        acc = 0.05f;
        break;

    default: 
        break;
    }
    
    if(s->target.x > s->speed.x)
    {
        s->speed.x += acc * tm;
        if(s->speed.x > s->target.x)
            s->speed.x = s->target.x;
    }
    else if(s->target.x < s->speed.x)
    {
        s->speed.x -= acc * tm;
        if(s->speed.x < s->target.x)
            s->speed.x = s->target.x;
    }

    if(s->target.y > s->speed.y)
    {
        s->speed.y += acc * tm;
        if(s->speed.y > s->target.y)
            s->speed.y = s->target.y;
    }
    else if(s->target.x < s->speed.y)
    {
        s->speed.y -= acc * tm;
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