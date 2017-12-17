/// Player (header)
/// (c) 2017 Jani Nykänen

#ifndef __PLAYER__
#define __PLAYER__

#include "../engine/vector.h"
#include "../engine/sprite.h"

#include "stdbool.h"

/// Player type
typedef struct
{
    VEC2 pos;
    VEC2 speed;
    VEC2 target;

    bool canJump;
    bool doubleJump;
    bool spinning;
    bool shooting;
    bool teleporting;
    bool released;

    int crystals;
    
    SPRITE spr;
    SPRITE sprArm;

}PLAYER;

/// Create a player object
/// > Player object
PLAYER create_player();

/// Update player
/// < pl Player to update
/// < tm Time mul.
void pl_update(PLAYER* pl, float tm);

/// Draw player
/// < pl Player
void pl_draw(PLAYER* pl);

/// Make him/her suffer
/// < pl Player
/// < p Position
/// < d Dimension
void pl_hurt(PLAYER* pl, VEC2 p, VEC2 d);

#endif // __PLAYER__