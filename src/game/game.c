/// Game scene (source)
/// (c) 2017 Jani Nykänen

#include "game.h"

#include "../engine/app.h"
#include "../engine/graphics.h"
#include "../engine/bitmap.h"
#include "../engine/controls.h"
#include "../engine/assets.h"
#include "../engine/transform.h"

#include "../vpad.h"

#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include "time.h"

#include "stage.h"
#include "player.h"
#include "slime.h"
#include "crystal.h"
#include "blood.h"
#include "victim.h"
#include "copter.h"

/// Bitmap font
static BITMAP* bmpFont;
/// Bitmap font 2 (yellow)
static BITMAP* bmpFont2;
/// Power-up bitmap
static BITMAP* bmpPowerUp;
/// Game Over! logo
static BITMAP* bmpGameOver;
/// Cursor bitmap
static BITMAP* bmpCursor;
/// Warning bitmap
static BITMAP* bmpWarning;
/// Controls bitmap
static BITMAP* bmpControls;

/// Warning sample
static SAMPLE* smpWarning;
/// Choose sample
static SAMPLE* smpChoose;
/// Select sample
static SAMPLE* smpSelect;
/// Start sample
static SAMPLE* smpStart;

/// Kills
static int kills;
/// Percentage
static int percentage;

/// Player
static PLAYER player;
/// Bullet count
#define BULLET_COUNT 32
/// Bullets
static BULLET bullets[BULLET_COUNT];
/// Slime count
#define SLIME_COUNT 32
/// Slimes
static SLIME slimes[32];
/// Slime timer count
#define SLIME_TIMER_COUNT 4
/// Slime timers
static float slimeTimer[SLIME_TIMER_COUNT];
/// Double slime timer
static float doubleSlimeTimer;
/// Crystal count
#define CRYSTAL_COUNT 32
/// Crystals
static CRYSTAL crystals[32];
/// Bomb counter
static int bombCount;
/// Blood count
#define BLOOD_COUNT 128
/// Blood
static BLOOD blood[BLOOD_COUNT];
/// Victim count
#define VICTIM_COUNT 16
/// Victimes
static VICTIM victims[VICTIM_COUNT];
/// Victim counter
static int vicCount;
/// Copter
static COPTER copter;
/// Copter counter
static int copterCount;

/// Screen shake timer
static float shakeTimer;
/// Shake amount
static int shake;

/// Is the game won
static bool victory;

/// Is the game over
static bool gameOver;
/// Game over timer
static float goverTimer;
/// Game Over! frame
static FRAME* goFrame;
/// Draw HUD
static bool drawHud;
/// Fade game over timer
static float fadeGoTimer;
/// Cursor pos in game over screen
static bool cursorPos;

/// Health change timer
static float healthChange;
/// Warning timer
static float warnTimer;

/// Controls shown 
static bool controlsShown;
/// Controls screen timer
static float contrTimer;
/// Controls screen enabled
static bool ctrScreen;
/// Controls screen mode
static int ctrMode;

/// Draw controls
static void draw_controls()
{
    float pos;
    if(ctrMode != 1)
        pos = -96 + 96/60.0f * (ctrMode == 2 ? (60.0f-contrTimer) : contrTimer);
    else
        pos = 0;

    draw_bitmap(bmpControls,0,pos,0);
}

/// Draw game over
/// TODO: This should actually be an external scene,
/// just like pause, but I was lazy
static void draw_gameover()
{
    draw_inverted_bitmap((BITMAP*)goFrame,0,0,0);

    if(goverTimer > 0.0f)
    {
        int skip = (int)floor(goverTimer / 5.0f) +1;
        draw_skipped_bitmap_region((BITMAP*)goFrame,0,0,128,96,0,0,skip,skip,0);

        // skip = 5.0f - ((int)floor(goverTimer / 5.0f));
        // draw_skipped_bitmap_region((BITMAP*)bmpGameOver,0,0,128,16,0,24,skip,skip,0);
    }
    else
    {   
        draw_bitmap(bmpGameOver,0,24,0);

        draw_text(bmpFont2,(Uint8*)"Retry",5,64,64,-1,0,true);
        draw_text(bmpFont2,(Uint8*)"Quit",5,64,78,-1,0,true);

        int p = cursorPos ? 79 : 65;
        draw_bitmap(bmpCursor,30,p,0);
    }
}

/// Update game over
static void update_gameover(float tm)
{
    if(goverTimer > 0.0f)
        goverTimer -= 1.0f * tm;

    if(goverTimer <= 0.0f)
    {
        float stick = vpad_get_stick().y * (cursorPos ? -1 : 1);
        if(stick > 0.25f )
        {
            cursorPos = !cursorPos;
            play_sample(smpSelect,0.70f);
        }

        if(vpad_get_button(4) == PRESSED || vpad_get_button(1) == PRESSED)
        {
            play_sample(smpChoose,0.70f);
            if(!cursorPos)
            {
                gameOver = false;
                copy_frame(app_get_canvas(),goFrame);
                game_recreate();
                fadeGoTimer = 30.0f;
            }
            else
            {
                app_swap_scene("title");
            }
        }

        if(vpad_get_button(5) == PRESSED)
        {
            play_sample(smpChoose,0.70f);
            app_swap_scene("title");
        }
    }
    
}

/// Draw power up timer & icon
static void draw_powerup()
{
    float p = player.powerUpTimer / 60.0f;

    int x = 16;
    int y = 96-9;
    int w = 32;
    int h = 8;

    fill_rect(x,y,w,h,63);
    fill_rect(x+1,y+1,w-2,h-2,0);

    float mw = (w-4) * p;
    fill_rect(x+2,y+2,(int)mw,h-4,0b00101010);
    fill_rect(x+2,y+2,(int)mw -1,h-5,63);
    fill_rect(x+2 + (int)mw +1,y+2,28 - (int)mw -1,h-4,0b00010101);

    draw_bitmap_region(bmpPowerUp,12*player.powerUpId,0,12,12,2,96-12,0);
}

/// Draw HUD
static void draw_hud()
{
    // Draw percentage
    int hundreds = (int)floor(percentage / 10);
    int decimals = percentage - hundreds*10;

    char prcStr[32];
    snprintf(prcStr,32,"%d.%d ",hundreds,decimals);
    int dpos = (int)floor(hundreds/100) > 0 ? 5 : ( (int)floor(hundreds/10) > 0 ? 4 : 3);
    prcStr[dpos] = '%';

    draw_text(bmpFont,(Uint8*)prcStr,dpos+1,64,1,-1,0,true);

    // Draw kills
    char num[3];
    snprintf(num,3,"%d",kills);

    Uint8 kills[32];
    kills[0] = 2;
    kills[1] = 3;
    kills[2] = 4;
    kills[3] = num[0];
    kills[4] = num[1];
    kills[5] = 0;
    
    draw_text(bmpFont,kills,32,2,1,0,0,false);

    // Draw crystals
    snprintf(num,3,"%d",player.crystals);

    Uint8 crystals[32];
    crystals[0] = 5;
    crystals[1] = 6;
    crystals[2] = 4;
    crystals[3] = num[0];
    crystals[4] = num[1];
    crystals[5] = 0;
    
    draw_text(bmpFont,crystals,32,87,1,0,0,false);

    // Draw hearts
    int i = 0;
    Uint8 h[] = {7,8};
    for(; i < player.health; i++)
    {
        draw_text(bmpFont,h,2,-1 + i*12,14,0,0,false);
    }
    if(healthChange > 0.0f)
    {
        int pos = (int)(24.0f - 24.0f/60.0f * healthChange);
        draw_text(bmpFont,h,2,-1 + i*12,14 - pos,0,0,false);
    }

    // Power up
    if(player.powerUpTimer > 0.0f) 
        draw_powerup();

    // Warning
    if(warnTimer > 0.0f && (warnTimer < 45.0f || (int)floor(warnTimer/4) % 2 == 0) )
    {
        draw_bitmap(bmpWarning,128-24,96-36,0);
    }
}

/// Push slime to the screen
/// < index Timer index
static void push_slime(int index)
{
    SLIME * s = &slimes[0];
    int i = 0;
    for(; i < SLIME_COUNT; i++)
    {
        if(slimes[i].dead == true && slimes[i].dying == false)
        {
            s = &slimes[i];
            break;
        }
    }

    int id =  rand() % 8 + index;
    if(id >= 8) id -= 8;

    bombCount --;
    if(bombCount <= 0)
    {
        id = rand() %2 + 8;
        bombCount = rand() % 6 + 4;
    }

    VEC2 pos;
    switch(id)
    {
    case 8:
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
        
        pos.x = 128+8;
        pos.y = 96-12;
        
        break;

    case 9:
    case 6:
    case 5:
        pos.x = 128+8;
        pos.y = 16 + (float)(rand() % 56) - 8;
        break;

    case 7:
        pos.x = 128+8;
        pos.y = -8;
        break;

    default:
        break;
    }
    put_slime(s,pos,id);
}

/// Push victim to the screen
static void push_victim()
{
    VICTIM * v = &victims[0];
    int i = 0;
    for(; i < VICTIM_COUNT; i++)
    {
        if(victims[i].dead == true && victims[i].dying == false)
        {
            v = &victims[i];
            break;
        }
    }

    int id = rand() % 2;
    put_victim(v,vec2(id == 0 ? 288 : 208,96-12),id);
}

/// Push copter to the screen
static void push_copter()
{
    if(!copter.dead || copter.dying) return;

    int x = 128 + 12;
    int y = 16 + (rand() % (96-44));
    put_copter(&copter,vec2(x,y));
    
    copterCount = 16 + rand() % 16;
}

/// Recreate game
void game_recreate()
{
    srand(time(NULL));

    init_stage();

    percentage = 0;
    kills = 0;

    // Create objects
    player = create_player();
    int i = 0;
    for(; i < BULLET_COUNT; i++)
    {
        bullets[i] = create_bullet();
    }
    for(i=0; i < SLIME_COUNT; i++)
    {
        slimes[i] = create_slime();
    }
    for(i=0; i < SLIME_TIMER_COUNT; i++)
    {
        slimeTimer[i] = 90 + i*300;
    }
    for(i=0; i < CRYSTAL_COUNT; i++)
    {
        crystals[i] = create_crystal();
    }
    for(i=0; i < BLOOD_COUNT; i++)
    {
        blood[i] = create_blood();
    }
    for(i=0; i < VICTIM_COUNT; i++)
    {
        victims[i] = create_victim();
    }
    copter = create_copter();

    bombCount = rand() % 4 + 4;
    vicCount = rand() % 4 + 4;
    healthChange = 0.0f;
    shakeTimer = 0.0f;
    shake = 0;
    copterCount = 16 + rand() % 8;
    doubleSlimeTimer = 0.0f;
    gameOver = false;
    drawHud = true;
    victory = false;
    goverTimer = 0.0f;
    warnTimer = 0.0f;
}

/// Init game
static int game_init()
{
    bmpFont = get_bitmap("font");
    bmpFont2 = get_bitmap("font2");
    bmpPowerUp = get_bitmap("powerup");
    bmpGameOver = get_bitmap("gameover");
    bmpCursor = get_bitmap("cursor");
    bmpWarning = get_bitmap("warning");
    bmpControls = get_bitmap("controls");

    smpWarning = get_sample("warning");
    smpChoose = get_sample("choose");
    smpSelect = get_sample("select");
    smpStart = get_sample("start");

    controlsShown = false;
    contrTimer = 0.0f;
    ctrScreen = false;

    goFrame = frame_create(128,96);
    if(goFrame == NULL) return 1;

    fadeGoTimer = 0.0f;

    game_recreate();
    drawHud = false;

    return 0;
}

/// Update game
/// tm Time multiplier
static void game_update(float tm)
{
    if(ctrScreen)
    {
        if(ctrMode == 0 || ctrMode == 2)
        {
            contrTimer += 2.0f * tm;
            if(contrTimer >= 60.0f)
            {
                contrTimer = 0.0f;
                if(ctrMode == 0)
                    ctrMode = 1;
                else
                    ctrScreen = false;
            }
        }

        if(ctrMode == 1 && any_pressed())
        {
            ctrMode = 2;
            play_sample(smpChoose,0.70f);
        }
            
        return;
    }

    if(gameOver)
    {
        update_gameover(tm);
        return;
    }
    else if(fadeGoTimer > 0.0f)
    {
        fadeGoTimer -= 1.0f * tm;
        if(fadeGoTimer <= 0.0f && !controlsShown)
        {
            ctrScreen = true;
            controlsShown = true;
            contrTimer = 0.0f;
            return;
        }
    }

    if(warnTimer > 0.0f)
    {
        warnTimer -= 1.0f * tm;
    }

    if(!player.dead && !victory && (vpad_get_button(4) == PRESSED || vpad_get_button(5) == PRESSED) )
    {
        play_sample(smpStart,0.70f);
        app_swap_scene("pause");
        return;
    }

    int oldHealth = player.health;

    // Update stage
    update_stage(&player,tm);
    // Update bullets
    int i = 0;
    int i2 = 0;
    for(; i < BULLET_COUNT; i++)
    {
        bullet_update(&bullets[i],tm);
    }
    // Update slimes
    for(i=0; i < SLIME_COUNT; i++)
    {
        slime_update(&slimes[i],tm);
        slime_collision(&slimes[i],&player,bullets,BULLET_COUNT);
        for(i2 = 0; i2 < SLIME_COUNT; i2++)
        {
            if(i == i2) continue;
            slime_to_slime_collision(&slimes[i],&slimes[i2]);
        }
    }
    // Update victims
    for(i=0; i < VICTIM_COUNT; i++)
    {
        victim_update(&victims[i],tm);
        victim_collision(&victims[i],&player,bullets,BULLET_COUNT);
        victim_slime_collision(&victims[i],slimes,SLIME_COUNT);
    }
    // Update crystals
    for(i=0; i < CRYSTAL_COUNT; i++)
    {
        crystal_update(&crystals[i],&player,tm);
    }
    // Update copter
    copter_update(&copter,tm);
    copter_collision(&copter,&player,bullets,BULLET_COUNT);
    // Update player
    pl_update(&player,tm);

    // Update blood
    for(i=0; i < BLOOD_COUNT; i++)
    {
        blood_update(&blood[i],tm);
    }

    // Update timers
    float speed = get_global_speed();
    bool victimCreated = false;

    if(!victory)
    {

        for(i=0; i < SLIME_TIMER_COUNT; i++)
        {
            slimeTimer[i] -= 1.0f * speed * tm;
            if(slimeTimer[i] <= 0.0f)
            {
                int loop = doubleSlimeTimer >= 300 + 300*i ? 2 : 1;
                for(i2=0; i2 < loop; i2 ++)
                    push_slime(i);

                slimeTimer[i] = (float)(rand() % 120 + 60);

                vicCount --;
                if(!victimCreated && vicCount <= 0)
                {
                    play_sample(smpWarning,0.70f);
                    warnTimer = 90.0f;

                    push_victim();
                    victimCreated = true;

                    vicCount = rand() % 8 + 8;
                }

                copterCount --;
                if(copterCount <= 0)
                    push_copter();
            }
        }
        
        if(percentage >= 1000)
        {
            victory = true;
            swap_music(get_music("theme1"),0.5f);
            for(i=0; i < SLIME_COUNT; i++)
            {
                if(slimes[i].dead) continue;
                slimes[i].dead = true;
                slimes[i].dying = true;
            }
            player.dead = false;
            player.dying = false;
            player.health = 3;
        }
    }

    // Update health change timer
    if(oldHealth > player.health && player.health > -1)
        healthChange = 60.0f;
    if(healthChange > 0.0f)
        healthChange -= 2.0f * tm;

    // Update shake timer
    if(shakeTimer > 0.0f)
        shakeTimer -= 1.0f * tm;

    // Game over!
    if(player.dead && !player.dying)
    {
        drawHud = false;
        game_draw();
        copy_frame(get_current_frame(),goFrame);
        drawHud = true;
        goverTimer = 30.0f;
        gameOver = true;
        play_music(get_music("theme1"),0.50f);
    }
}

/// Draw game
void game_draw()
{
    clear_frame(0);

    if(gameOver)
    {
        draw_gameover();
        return;
    }

    if(shakeTimer <= 0.0f)
        set_translation(0,0);
    else
        set_translation(rand() % (shake*2 +1) - shake,rand() % (shake*2 +1) - shake );

    int i = 0;
    // Draw stage
    draw_stage();
    // Draw slimes
    for(i=0; i < SLIME_COUNT; i++)
    {
        slime_draw(&slimes[i]);
    }
    // Post-draw slimes
    for(i=0; i < SLIME_COUNT; i++)
    {
        slime_post_draw(&slimes[i]);
    }
    // Draw copter
    copter_draw(&copter);
    // Draw victims
    for(i=0; i < VICTIM_COUNT; i++)
    {
        victim_draw(&victims[i]);
    }
    // Draw blood
    for(i=0; i < BLOOD_COUNT; i++)
    {
        blood_draw(&blood[i]);
    }
    // Draw crystals
    for(i=0; i < CRYSTAL_COUNT; i++)
    {
        crystal_draw(&crystals[i]);
    }

    // Draw player
    pl_draw(&player);
    // Draw bullets
    for(i=0; i < BULLET_COUNT; i++)
    {
        bullet_draw(&bullets[i]);
    }
    // "Post draw" stage (for ending)
    post_draw_stage();

    set_translation(0,0);
    // Draw HUD
    if(drawHud && !victory)
        draw_hud();

    // Draw fading game over screen
    if(fadeGoTimer > 0.0f)
    {
        int skip = (int)floor(fadeGoTimer / 5.0f) +1;
        draw_skipped_bitmap_region((BITMAP*)goFrame,0,0,128,96,0,0,skip,skip,0);
    }

    // Draw controls
    if(ctrScreen)
    {
        draw_controls();
    }
}

/// Destroy game
static void game_destroy()
{
    destroy_assets();
}

/// Scene swapped
static void game_on_swap()
{
    drawHud = true;
}

/// Get the next nonexistent bullet in the array
BULLET* get_next_bullet()
{
    int i = 0;
    for(; i < BULLET_COUNT; i++)
    {
        if(bullets[i].exist == false && bullets[i].dying == false)
            return &bullets[i];
    }
    return &bullets[0];
}

/// Create crystals
void create_crystals(VEC2 pos, int count)
{
    VEC2 speed;

    int loop = 0;
    int i;
    for(; loop < count; loop++)
    {
        for(i=0; i < CRYSTAL_COUNT; i++)
        {
            if(crystals[i].exist || crystals[i].vanishing) continue;

            speed.x = -0.5f + (float)(rand() % 200) / 100.0f;
            speed.y =  0.25f -(float)(rand() % 200 ) / 100.0f;

            put_crystal(&crystals[i],pos,speed);


            break;
        }
    }
}

/// Add percentage
void add_percentage(int amount)
{
    int p = get_sky_phase();
    amount = (int)((float)amount * (0.8f + p*0.1f));

    percentage += amount;
    if(percentage >= 1000)
    {
        percentage = 1000;
        // Trigger final boss event
    }
}

/// Add a kill
void add_kill()
{
    ++ kills;
}

/// Get the amount of kills
int get_kills()
{
    return kills;
}

/// Shake screen
void shake_screen()
{
    shakeTimer = 30.0f + kills*5.0f;
    shake = 4 + (int)floor(kills/3);
}

/// Is the game won
bool is_victory()
{
    return percentage >= 1000;
}

/// Create some nasty blood
void create_blood_effect(VEC2 pos, int amount, Uint8 color)
{
    VEC2 speed;

    int loop = 0;
    int i;
    for(; loop < amount; loop++)
    {
        for(i=0; i < BLOOD_COUNT; i++)
        {
            if(blood[i].exist) continue;

            speed.x = -1.5f + (float)(rand() % 300) / 100.0f;
            speed.y = 0.5f - (float)(rand() % 300 ) / 100.0f;

            put_blood(&blood[i],pos,speed,color);

            break;
        }
    }
}

/// Get the game over frame
FRAME* get_gameover_frame()
{
    fadeGoTimer = 30.0f;
    return goFrame;
}

/// Get game scene
SCENE get_game_scene()
{
    // Set scene functions
    SCENE s = (SCENE){game_init,game_update,game_draw,game_destroy,game_on_swap};

    // Set scene name
    set_scene_name(&s,"game");

    return s;
}
