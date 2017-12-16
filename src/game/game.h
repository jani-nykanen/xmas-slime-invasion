/// Game scene (header)
/// (c) 2017 Jani Nykänen

#ifndef __GAME_SCENE__
#define __GAME_SCENE__

#include "../engine/scene.h"

#include "bullet.h"

/// Get the next nonexistent bullet in the array
/// > Bullet pointer
BULLET* get_next_bullet();

/// Get game scene
/// > Game scene
SCENE get_game_scene();

/// Draw game
void game_draw();

#endif // __GAME_SCENE__