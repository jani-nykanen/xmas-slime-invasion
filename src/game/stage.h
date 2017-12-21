/// Stage (header)
/// (c) 2017 Jani Nykänen

#ifndef __STAGE__
#define __STAGE__

#include "player.h"

/// Initialize stage
void init_stage();

/// Update stage
/// < pl Player
/// < cam Camera
/// < tm Time mul.
void update_stage(PLAYER* pl, float tm);

/// Draw stage
/// < cam Camera
void draw_stage();

/// Get global speed
/// > Global speed
float get_global_speed();

/// Get the sky phase
/// > Sky phase
int get_sky_phase();

#endif // __STAGE__
