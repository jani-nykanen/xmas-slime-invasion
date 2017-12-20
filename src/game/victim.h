/// Victim (header)
/// (c) 2017 Jani Nykänen

#ifndef __VICTIM__
#define __VICTIM__

#include "../engine/vector.h"
#include "../engine/sprite.h"

#include "stdbool.h"

#include "bullet.h"
#include "slime.h"

/// Victim type
typedef struct
{
    VEC2 pos;
    VEC2 speed;
    SPRITE spr;
    SPRITE sprWarning;
    bool dead;
    bool dying;
    bool canJump;
    float jumpTimer;
    int id;
}
VICTIM;

/// Create a new victim instance
VICTIM create_victim();

/// Put a victim to the screen
/// < v Victim
/// < pos Position
/// < id Victim ID
void put_victim(VICTIM* v, VEC2 pos, int id);

/// Update a victim
/// < v Victim
/// < tm Time mul.
void victim_update(VICTIM* v, float tm);

/// Get victim's collision with game objects
/// < pl Player
/// < v Victim
/// < bullets Bullets in an array
/// < bulletLength The size of the bullet array
void victim_collision(VICTIM* v, PLAYER* pl, BULLET* bullets, int bulletLength);

/// Get collision with victims & slimes
/// < v Victim
/// < slimes Slimes in an array
/// < slimeLength The size of the slime array
void victim_slime_collision(VICTIM* v, SLIME* slimes, int slimeLength);

/// Draw a victim
/// < v Victim
void victim_draw(VICTIM* v);

#endif // __VICTIM__