/// Stage (header)
/// (c) 2017 Jani Nykänen

#ifndef __STAGE__
#define __STAGE__

/// Initialize stage
void init_stage();

/// Update stage
/// < b Ball
/// < cam Camera
/// < tm Time mul.
void update_stage(float tm);

/// Draw stage
/// < cam Camera
void draw_stage();

/// Get global speed
/// > Global speed
float get_global_speed();

#endif // __STAGE__
