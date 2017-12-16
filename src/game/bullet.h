/// Bullet (header)
/// (c) 2017 Jani Nykänen

#ifndef __BULLET__
#define __BULLET__

#include "../engine/vector.h"
#include "../engine/sprite.h"

#include "stdbool.h"

/// Bullet type
typedef struct
{
    VEC2 pos;
    float speed;
    bool exist;
    bool dying;
    SPRITE spr;
    SPRITE sprDeath;
}
BULLET;

/// Create a new bullet instance (but does not
/// put a bullet to the screen)
/// > Bullet
BULLET create_bullet();

/// Put a bullet to the screen
/// < b Bullet
/// < pos Position
/// < speed Spped
void put_bullet(BULLET* b, VEC2 pos, float speed);

/// Update a bullet
/// < b BUllet
/// < tm Time mul.
void bullet_update(BULLET* b, float tm);

/// Draw a bullet
/// < b Bullet
void bullet_draw(BULLET* b);

#endif // __BULLET__