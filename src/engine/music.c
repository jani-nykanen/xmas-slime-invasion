/// Music (source)
/// (c) 2017 Jani Nykänen

#include "music.h"

#include "SDL2/SDL.h"

#include "stdbool.h"
#include "time.h"
#include "stdlib.h"
#include "math.h"

/// Global music volume
static float globalMusicVol;

/// Loop length
static int loopLength;

/// Start time
static long startTime;

/// Is music playing
static bool playing;

/// Position in ms
static int posMs;

/// Init music
int init_music()
{
    globalMusicVol = 1.0f;
    playing = false;
    loopLength = 0;
    startTime = 0;
    posMs = 0;

    // Init formats
    int flags = MIX_INIT_OGG;
    int initted = Mix_Init(flags);
    if( (initted&flags) != flags) 
    {
        SDL_ShowSimpleMessageBox( SDL_MESSAGEBOX_ERROR,"Error!","Failed to init audio!\n",NULL);
        return 0;
    }

    // Open audio
    if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024)==-1) 
    {
        SDL_ShowSimpleMessageBox( SDL_MESSAGEBOX_ERROR,"Error!","Failed to open audio!\n",NULL);
        return 1;
    }

    return 0;
}   

/// Load music
MUSIC* load_music(const char* path)
{
    MUSIC* m = (MUSIC*)malloc(sizeof(MUSIC));
    if(m == NULL)
    {
        SDL_ShowSimpleMessageBox( SDL_MESSAGEBOX_ERROR,"Error!","Memory allocation error!\n",NULL);
        return NULL;
    }

    m->data = Mix_LoadMUS(path);
    if(m->data == NULL)
    {
        char err [64];
        snprintf(err,64,"Failed to load a file in %s!",path);

        SDL_ShowSimpleMessageBox( SDL_MESSAGEBOX_ERROR,"Error!",err,NULL);
        return NULL;
    }
    return m;
}

/// Play music
void play_music(MUSIC* mus, float vol)
{
    int v = (int)(128 * vol * globalMusicVol);
    if(v > 128) v = 128;
    if(v < 0) v = 0;

    Mix_HaltMusic();
    Mix_VolumeMusic(v);
    Mix_PlayMusic(mus->data, -1);

    startTime = (long)time(NULL);
    posMs = 0;
}

/// Update music
void update_music(float tm)
{   
    long t = (long)time(NULL);
    if(t - startTime > loopLength)
    {
        posMs -= loopLength;
        startTime = t;
    }
}

/// Set approximated music length in milliseconds
void set_music_length(int ms)
{
    loopLength = ms;
}

/// Swap music but keep the position
void swap_music(MUSIC* m, float vol)
{
    play_music(m,vol);
    Mix_SetMusicPosition( (double)posMs / 1000.0f );
}   