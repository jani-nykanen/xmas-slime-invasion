/// Game scene (header)
/// (c) 2017 Jani Nykänen

#ifndef __GAME_SCENE__
#define __GAME_SCENE__

#include "../engine/scene.h"

#include "bullet.h"

/// Draw game
void game_draw();

/// Get the next nonexistent bullet in the array
/// > Bullet pointer
BULLET* get_next_bullet();

/// Create crystals
/// < pos Position
/// < count Crystal count
void create_crystals(VEC2 pos, int count);

/// Add percentage
/// < amount Amount
void add_percentage(int amount);

/// Add a kill
void add_kill();

/// Get the amount of kills
/// > Kills
int get_kills();

/// Shake screen
void shake_screen();

/// Is the game won
/// > True, if... well, true!
bool is_victory();

/// Create some nasty blood
/// < pos Position
/// < amount Amount
/// < color Blood color
void create_blood_effect(VEC2 pos, int amount, Uint8 color);

/// Get game scene
/// > Game scene
SCENE get_game_scene();

#endif // __GAME_SCENE__