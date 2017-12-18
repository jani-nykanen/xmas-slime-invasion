/// Blood (header)
/// (c) 2017 Jani Nykänen

#ifndef __BLOOD__
#define __BLOOD__

#include "../engine/vector.h"

#include "stdbool.h"

/// Blood type (ehehhehehe)
typedef struct
{
    VEC2 pos;
    VEC2 speed;
    bool exist;
    float timer;
    int radius;
}
BLOOD;

/// Create a new blood instance (but does not
/// put a blood to the screen)
/// > Blood
BLOOD create_blood();

/// Put a blood to the screen
/// < b Blood
/// < pos Position
/// < speed Speed
void put_blood(BLOOD* b, VEC2 pos, VEC2 speed);

/// Update "a" blood
/// < b blood
/// < tm Time mul.
void blood_update(BLOOD* b, float tm);

/// Draw a blood
/// < b Blood
void blood_draw(BLOOD* b);

#endif // __BLOOD__