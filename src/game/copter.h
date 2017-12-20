/// Copter (header)
/// (c) 2017 Jani Nykänen

#ifndef __COPTER__
#define __COPTER__

#include "../engine/vector.h"
#include "../engine/sprite.h"

#include "stdbool.h"

#include "bullet.h"
#include "player.h"

/// Copter type
typedef struct
{
    VEC2 pos;
    float startY;
    float waveTimer;
    SPRITE spr;
    SPRITE sprPower;
    bool dead;
    bool dying;
}
COPTER;

/// Create a new copter instance
COPTER create_copter();

/// Put a copter to the screen
/// < c Copter
/// < pos Position
void put_copter(COPTER* c, VEC2 pos);

/// Update a copter
/// < c Copter
/// < tm Time mul.
void copter_update(COPTER* c, float tm);

/// Get copter's collision with game objects
/// < c Copter
/// < pl Player
/// < bullets Bullets in an array
/// < bulletLength The size of the bullet array
void copter_collision(COPTER* c, PLAYER* pl, BULLET* bullets, int bulletLength);

/// Draw a copter
/// < c Copter
void copter_draw(COPTER* c);

#endif // __COPTER__