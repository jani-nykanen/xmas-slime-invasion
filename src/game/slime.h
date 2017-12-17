/// Slime (header)
/// (c) 2017 Jani Nykänen

#ifndef __SLIME__
#define __SLIME__

#include "../engine/vector.h"
#include "../engine/sprite.h"

#include "stdbool.h"

#include "player.h"
#include "bullet.h"

/// Slime type
typedef struct
{
    VEC2 pos;
    float startY;
    VEC2 speed;
    VEC2 target;
    SPRITE spr;
    bool dead;
    bool dying;
    bool canJump;
    float spcTimer;
    int id;
    int health;
    float hurtTimer;

    float spcValue1;
    float spcValue2;
}
SLIME;

/// Create a new slime instance
SLIME create_slime();

/// Put a slime to the screen
/// < s Slime
/// < pos Position
/// < id Slime ID
void put_slime(SLIME* s, VEC2 pos, int id);

/// Update a slime
/// < s Slime
/// < tm Time mul.
void slime_update(SLIME* s, float tm);

/// Get collision with game objects
/// < s Slime
/// < pl Player
/// < bullets Bullets in an array
/// < bulletLength The size of the bullet array
void slime_collision(SLIME* s, PLAYER* pl, BULLET* bullets, int bulletLength);

/// Slime to slime collision
/// < s "This" slime
/// < o The other slime
void slime_to_slime_collision(SLIME* s, SLIME* o);

/// Draw a slime
/// < s Slime
void slime_draw(SLIME* s);

#endif // __SLIME__