/// Crystal (header)
/// (c) 2017 Jani Nykänen

#ifndef __CRYSTAL__
#define __CRYSTAL__

#include "../engine/vector.h"
#include "../engine/sprite.h"

#include "player.h"

/// Crystal type
typedef struct
{
    VEC2 pos;
    VEC2 speed;
    bool exist;
    SPRITE spr;
    bool vanishing;
}
CRYSTAL;

/// Create a new crystal instance (but does not
/// put a crystal to the screen)
/// > Crystal
CRYSTAL create_crystal();

/// Put a crystal to the screen
/// < c Crystal
/// < pos Position
/// < speed Speed
void put_crystal(CRYSTAL* c, VEC2 pos, VEC2 speed);

/// Update a crystal
/// < c Crystal
/// < pl Player
/// < tm Time mul.
void crystal_update(CRYSTAL* c, PLAYER* pl, float tm);

/// Draw a crystal
/// < c Crystal
void crystal_draw(CRYSTAL* c);

#endif // __CRYSTAL__