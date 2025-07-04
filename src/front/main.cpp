// Very Rough! refactor later!

#define SDL_MAIN_USE_CALLBACKS 1  /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include "game_machine.h"
#include <thread>
#include <chrono>
#include <SDL3_ttf/SDL_ttf.h>
#include <iostream>
#include <string>
#include <format>

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static SDL_Texture *redTile = NULL;
static SDL_Texture *blueTile = NULL;
static SDL_Texture *player = NULL;
static float tileWidth;
static float tileHeight;
static float charWidth;
static float charHeight;
static GameMachine::In gameIn;
static GameMachine::Machine game;
static GameMachine::Out data;
static TTF_TextEngine* engine = NULL;
static TTF_Font* font = NULL;
static TTF_Text* text = NULL;

static const Uint32 interval = 34;

static const int WINDOW_WIDTH = 1600;
static const int WINDOW_HEIGHT = 900;


Uint32 GameLoop(void *userdata, SDL_TimerID timerID, Uint32 interval) {
    data = game.step(gameIn);
    return interval;
}

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    SDL_SetAppMetadata("ntbtl", "1.0", "ntbtl");

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("ntblt", WINDOW_WIDTH, WINDOW_HEIGHT, 0, &window, &renderer)) {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!TTF_Init()) {
        SDL_Log("Couldn't initialize TTF: %s", SDL_GetError());
    }

    /* Textures are pixel data that we upload to the video hardware for fast drawing. Lots of 2D
       engines refer to these as "sprites." We'll do a static texture (upload once, draw many
       times) with data from a bitmap file. */

    /* SDL_Surface is pixel data the CPU can access. SDL_Texture is pixel data the GPU can access.
       Load a .bmp into a surface, move it to a texture from there. */
    SDL_Surface * s = SDL_LoadBMP("../res/redtile.bmp");
    if (!s) {
        SDL_Log("Couldn't load bitmap: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    redTile = SDL_CreateTextureFromSurface(renderer,s);

    s = SDL_LoadBMP("../res/bluetile.bmp");
    if (!s) {
        SDL_Log("Couldn't load bitmap: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    blueTile = SDL_CreateTextureFromSurface(renderer,s);

    s = IMG_Load("../res/stances.png");
    if (!s) {
        SDL_Log("Couldn't load bitmap: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    player = SDL_CreateTextureFromSurface(renderer,s);


    gameIn.player1Keys = {};
    gameIn.player2Keys = {};

    //text renderer
    engine = TTF_CreateRendererTextEngine(renderer);
    //walgreens! its the first ttf I found online!
    font = TTF_OpenFont("../res/walgreensscriptfreeversion.ttf",50.0);
    if(!font) {
        SDL_Log("Couldn't load font: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    text = TTF_CreateText(engine,font,"hello",5);
    if(!text) {
        SDL_Log("Couldn't load text: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    


    //start the game loop
    SDL_AddTimer(interval,GameLoop,nullptr);

    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;  /* end the program, reporting success to the OS. */
    }
    
    if(event->type == SDL_EVENT_KEY_DOWN) {
        switch(event->key.key) {
            case SDLK_Z: 
                gameIn.player1Keys.Move1 = true;
                break;
            case SDLK_X: 
                gameIn.player1Keys.Move2 = true;
                break;
            case SDLK_C: 
                gameIn.player1Keys.Move3 = true;
                break;
            case SDLK_A: 
                gameIn.player1Keys.Move4 = true;
                break;
            case SDLK_D: 
                gameIn.player1Keys.Move6 = true;
                break;
            case SDLK_Q: 
                gameIn.player1Keys.Move7 = true;
                break;
            case SDLK_W: 
                gameIn.player1Keys.Move8 = true;
                break;
            case SDLK_E: 
                gameIn.player1Keys.Move9 = true;
                break;
            case SDLK_J:
                gameIn.player1Keys.Attack1 = true;
                break;
            case SDLK_K:
                gameIn.player1Keys.Attack2 = true;
                break;
            default:
                break;
        }
    }
    if(event->type == SDL_EVENT_KEY_DOWN) {
        switch(event->key.key) {
            case SDLK_KP_1: 
                gameIn.player2Keys.Move1 = true;
                break;
            case SDLK_KP_2: 
                gameIn.player2Keys.Move2 = true;
                break;
            case SDLK_KP_3: 
                gameIn.player2Keys.Move3 = true;
                break;
            case SDLK_KP_4: 
                gameIn.player2Keys.Move4 = true;
                break;
            case SDLK_KP_6: 
                gameIn.player2Keys.Move6 = true;
                break;
            case SDLK_KP_7: 
                gameIn.player2Keys.Move7 = true;
                break;
            case SDLK_KP_8: 
                gameIn.player2Keys.Move8 = true;
                break;
            case SDLK_KP_9: 
                gameIn.player2Keys.Move9 = true;
                break;
            case SDLK_KP_0:
                gameIn.player2Keys.Attack1 = true;
                break;
            case SDLK_KP_PERIOD:
                gameIn.player2Keys.Attack2 = true;
                break;
            default:
                break;
        }
    }
    if(event->type == SDL_EVENT_KEY_UP) {
        switch(event->key.key) {
            case SDLK_Z: 
                gameIn.player1Keys.Move1 = false;
                break;
            case SDLK_X: 
                gameIn.player1Keys.Move2 = false;
                break;
            case SDLK_C: 
                gameIn.player1Keys.Move3 = false;
                break;
            case SDLK_A: 
                gameIn.player1Keys.Move4 = false;
                break;
            case SDLK_D: 
                gameIn.player1Keys.Move6 = false;
                break;
            case SDLK_Q: 
                gameIn.player1Keys.Move7 = false;
                break;
            case SDLK_W: 
                gameIn.player1Keys.Move8 = false;
                break;
            case SDLK_E: 
                gameIn.player1Keys.Move9 = false;
                break;
            case SDLK_J:
                gameIn.player1Keys.Attack1 = false;
                break;
            case SDLK_K:
                gameIn.player1Keys.Attack2 = false;
                break;
            default:
                break;
        }
    }
    if(event->type == SDL_EVENT_KEY_UP) {
        switch(event->key.key) {
            case SDLK_KP_1: 
                gameIn.player2Keys.Move1 = false;
                break;
            case SDLK_KP_2: 
                gameIn.player2Keys.Move2 = false;
                break;
            case SDLK_KP_3: 
                gameIn.player2Keys.Move3 = false;
                break;
            case SDLK_KP_4: 
                gameIn.player2Keys.Move4 = false;
                break;
            case SDLK_KP_6: 
                gameIn.player2Keys.Move6 = false;
                break;
            case SDLK_KP_7: 
                gameIn.player2Keys.Move7 = false;
                break;
            case SDLK_KP_8: 
                gameIn.player2Keys.Move8 = false;
                break;
            case SDLK_KP_9: 
                gameIn.player2Keys.Move9 = false;
                break;
            case SDLK_KP_0:
                gameIn.player2Keys.Attack1 = false;
                break;
            case SDLK_KP_PERIOD:
                gameIn.player2Keys.Attack2 = false;
                break;
            default:
                break;
        }
    }

    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    TTF_DestroyRendererTextEngine(engine);
    TTF_DestroyText(text);
    TTF_CloseFont(font);
    
    SDL_DestroyTexture(redTile);
    SDL_DestroyTexture(blueTile);
    SDL_DestroyTexture(player);
    /* SDL will clean up the window/renderer for us. */
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void *appstate)
{
    SDL_FRect dst_rect;

    /* as you can see from this, rendering draws over whatever was drawn before it. */
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);  /* black, full alpha */
    SDL_RenderClear(renderer);  /* start with a blank canvas. */

    /* Just draw the static texture a few times. You can think of it like a
       stamp, there isn't a limit to the number of times you can draw with it. */

    /* top left */
    //dst_rect.x = (100.0f * scale);
    //dst_rect.y = 0.0f;
    //dst_rect.w = (float) texture_width;
    //dst_rect.h = (float) texture_height;
    //SDL_RenderTexture(renderer, texture, NULL, &dst_rect);

    //tiles (who needs for loops?)
    dst_rect.x = 0.25f * WINDOW_WIDTH;
    dst_rect.y = 5.0f / 6.0f * WINDOW_HEIGHT;
    dst_rect.w = 0.125f * WINDOW_WIDTH;
    dst_rect.h = WINDOW_HEIGHT / 6;
    SDL_RenderTexture(renderer,blueTile,NULL,&dst_rect);
    dst_rect.y = 4.0f / 6.0f * WINDOW_HEIGHT;
    SDL_RenderTexture(renderer,blueTile,NULL,&dst_rect);
    dst_rect.y = 3.0f / 6.0f * WINDOW_HEIGHT;
    SDL_RenderTexture(renderer,blueTile,NULL,&dst_rect);
    dst_rect.x = 0.375f * WINDOW_WIDTH;
    dst_rect.y = 5.0f / 6.0f * WINDOW_HEIGHT;
    SDL_RenderTexture(renderer,blueTile,NULL,&dst_rect);
    dst_rect.y = 4.0f / 6.0f * WINDOW_HEIGHT;
    SDL_RenderTexture(renderer,blueTile,NULL,&dst_rect);
    dst_rect.y = 3.0f / 6.0f * WINDOW_HEIGHT;
    SDL_RenderTexture(renderer,blueTile,NULL,&dst_rect);
    dst_rect.x = 0.5f * WINDOW_WIDTH;
    dst_rect.y = 5.0f / 6.0f * WINDOW_HEIGHT;
    SDL_RenderTexture(renderer,redTile,NULL,&dst_rect);
    dst_rect.y = 4.0f / 6.0f * WINDOW_HEIGHT;
    SDL_RenderTexture(renderer,redTile,NULL,&dst_rect);
    dst_rect.y = 3.0f / 6.0f * WINDOW_HEIGHT;
    SDL_RenderTexture(renderer,redTile,NULL,&dst_rect);
    dst_rect.x = 0.625f * WINDOW_WIDTH;
    dst_rect.y = 5.0f / 6.0f * WINDOW_HEIGHT;
    SDL_RenderTexture(renderer,redTile,NULL,&dst_rect);
    dst_rect.y = 4.0f / 6.0f * WINDOW_HEIGHT;
    SDL_RenderTexture(renderer,redTile,NULL,&dst_rect);
    dst_rect.y = 3.0f / 6.0f * WINDOW_HEIGHT;
    SDL_RenderTexture(renderer,redTile,NULL,&dst_rect);

    //chars
    if(data.player1Data.sprite == "Idle") {
        dst_rect.x = WINDOW_WIDTH / 4.0 + (WINDOW_WIDTH / 8.0) * ((data.player1Data.px *1.0) / MoveMachine::Machine::TILE_WIDTH) - WINDOW_WIDTH / 16;
        dst_rect.y = WINDOW_HEIGHT * 3.0 / 6.0 - (WINDOW_HEIGHT / 6.0) * ((data.player1Data.py *1.0) / MoveMachine::Machine::TILE_HEIGHT);
        dst_rect.w = WINDOW_WIDTH / 8.0;
        dst_rect.h = WINDOW_HEIGHT / 2.0;
        SDL_FRect src_rect;
        src_rect.x = 0;
        src_rect.y = 0;
        src_rect.w = 200;
        src_rect.h = 450;
        SDL_RenderTexture(renderer,player,&src_rect,&dst_rect);
    }
    if(data.player1Data.sprite == "Move") {
        dst_rect.x = WINDOW_WIDTH / 4.0 + (WINDOW_WIDTH / 8.0) * ((data.player1Data.px *1.0) / MoveMachine::Machine::TILE_WIDTH) - WINDOW_WIDTH / 16;
        dst_rect.y = WINDOW_HEIGHT * 3.0 / 6.0 - (WINDOW_HEIGHT / 6.0) * ((data.player1Data.py *1.0) / MoveMachine::Machine::TILE_HEIGHT);
        dst_rect.w = WINDOW_WIDTH / 8.0;
        dst_rect.h = WINDOW_HEIGHT / 2.0;
        SDL_FRect src_rect;
        src_rect.x = 200;
        src_rect.y = 0;
        src_rect.w = 200;
        src_rect.h = 450;
        SDL_RenderTexture(renderer,player,&src_rect,&dst_rect);
    }
    if(data.player1Data.sprite == "Block") {
        dst_rect.x = WINDOW_WIDTH / 4.0 + (WINDOW_WIDTH / 8.0) * ((data.player1Data.px *1.0) / MoveMachine::Machine::TILE_WIDTH) - WINDOW_WIDTH / 16;
        dst_rect.y = WINDOW_HEIGHT * 3.0 / 6.0 - (WINDOW_HEIGHT / 6.0) * ((data.player1Data.py *1.0) / MoveMachine::Machine::TILE_HEIGHT);
        dst_rect.w = WINDOW_WIDTH / 8.0;
        dst_rect.h = WINDOW_HEIGHT / 2.0;
        SDL_FRect src_rect;
        src_rect.x = 400;
        src_rect.y = 0;
        src_rect.w = 200;
        src_rect.h = 450;
        SDL_RenderTexture(renderer,player,&src_rect,&dst_rect);
    }
    if(data.player1Data.sprite == "PunchWindup") {
        dst_rect.x = WINDOW_WIDTH / 4.0 + (WINDOW_WIDTH / 8.0) * ((data.player1Data.px *1.0) / MoveMachine::Machine::TILE_WIDTH) - WINDOW_WIDTH / 16;
        dst_rect.y = WINDOW_HEIGHT * 3.0 / 6.0 - (WINDOW_HEIGHT / 6.0) * ((data.player1Data.py *1.0) / MoveMachine::Machine::TILE_HEIGHT);
        dst_rect.w = WINDOW_WIDTH / 4.0;
        dst_rect.h = WINDOW_HEIGHT / 2.0;
        SDL_FRect src_rect;
        src_rect.x = 0;
        src_rect.y = 450;
        src_rect.w = 400;
        src_rect.h = 450;
        SDL_RenderTexture(renderer,player,&src_rect,&dst_rect);
    }
    if(data.player1Data.sprite == "PunchFollow") {
        dst_rect.x = WINDOW_WIDTH / 4.0 + (WINDOW_WIDTH / 8.0) * ((data.player1Data.px *1.0) / MoveMachine::Machine::TILE_WIDTH) - WINDOW_WIDTH / 16;
        dst_rect.y = WINDOW_HEIGHT * 3.0 / 6.0 - (WINDOW_HEIGHT / 6.0) * ((data.player1Data.py *1.0) / MoveMachine::Machine::TILE_HEIGHT);
        dst_rect.w = WINDOW_WIDTH / 4.0;
        dst_rect.h = WINDOW_HEIGHT / 2.0;
        SDL_FRect src_rect;
        src_rect.x = 0;
        src_rect.y = 900;
        src_rect.w = 400;
        src_rect.h = 450;
        SDL_RenderTexture(renderer,player,&src_rect,&dst_rect);
    }
    if(data.player1Data.sprite == "KickWindup") {
        dst_rect.x = WINDOW_WIDTH / 4.0 + (WINDOW_WIDTH / 8.0) * ((data.player1Data.px *1.0) / MoveMachine::Machine::TILE_WIDTH) - WINDOW_WIDTH / 16;
        dst_rect.y = WINDOW_HEIGHT * 3.0 / 6.0 - (WINDOW_HEIGHT / 6.0) * ((data.player1Data.py *1.0) / MoveMachine::Machine::TILE_HEIGHT);
        dst_rect.w = WINDOW_WIDTH * 3.0 / 8.0;
        dst_rect.h = WINDOW_HEIGHT / 2.0;
        SDL_FRect src_rect;
        src_rect.x = 0;
        src_rect.y = 1350;
        src_rect.w = 600;
        src_rect.h = 450;
        SDL_RenderTexture(renderer,player,&src_rect,&dst_rect);
    }
    if(data.player1Data.sprite == "KickFollow") {
        dst_rect.x = WINDOW_WIDTH / 4.0 + (WINDOW_WIDTH / 8.0) * ((data.player1Data.px *1.0) / MoveMachine::Machine::TILE_WIDTH) - WINDOW_WIDTH / 16;
        dst_rect.y = WINDOW_HEIGHT * 3.0 / 6.0 - (WINDOW_HEIGHT / 6.0) * ((data.player1Data.py *1.0) / MoveMachine::Machine::TILE_HEIGHT);
        dst_rect.w = WINDOW_WIDTH * 3.0 / 8.0;
        dst_rect.h = WINDOW_HEIGHT / 2.0;
        SDL_FRect src_rect;
        src_rect.x = 0;
        src_rect.y = 1800;
        src_rect.w = 600;
        src_rect.h = 450;
        SDL_RenderTexture(renderer,player,&src_rect,&dst_rect);
    }





    if(data.player2Data.sprite == "Idle") {
        dst_rect.x = WINDOW_WIDTH * 3.0 / 4.0 - (WINDOW_WIDTH / 8.0) * ((data.player2Data.px *1.0) / MoveMachine::Machine::TILE_WIDTH) - WINDOW_WIDTH / 16;
        dst_rect.y = WINDOW_HEIGHT * 3.0 / 6.0 - (WINDOW_HEIGHT / 6.0) * ((data.player2Data.py *1.0) / MoveMachine::Machine::TILE_HEIGHT);
        dst_rect.w = WINDOW_WIDTH / 8.0;
        dst_rect.h = WINDOW_HEIGHT / 2.0;
        SDL_FRect src_rect;
        src_rect.x = 0;
        src_rect.y = 0;
        src_rect.w = 200;
        src_rect.h = 450;
        SDL_RenderTextureRotated(renderer,player,&src_rect,&dst_rect,0,nullptr,SDL_FlipMode::SDL_FLIP_HORIZONTAL);
    }
    if(data.player2Data.sprite == "Move") {
        dst_rect.x = WINDOW_WIDTH * 3.0 / 4.0 - (WINDOW_WIDTH / 8.0) * ((data.player2Data.px *1.0) / MoveMachine::Machine::TILE_WIDTH) - WINDOW_WIDTH / 16;
        dst_rect.y = WINDOW_HEIGHT * 3.0 / 6.0 - (WINDOW_HEIGHT / 6.0) * ((data.player2Data.py *1.0) / MoveMachine::Machine::TILE_HEIGHT);
        dst_rect.w = WINDOW_WIDTH / 8.0;
        dst_rect.h = WINDOW_HEIGHT / 2.0;
        SDL_FRect src_rect;
        src_rect.x = 200;
        src_rect.y = 0;
        src_rect.w = 200;
        src_rect.h = 450;
        SDL_RenderTextureRotated(renderer,player,&src_rect,&dst_rect,0,nullptr,SDL_FlipMode::SDL_FLIP_HORIZONTAL);
    }
    if(data.player2Data.sprite == "Block") {
        dst_rect.x = WINDOW_WIDTH * 3.0 / 4.0 - (WINDOW_WIDTH / 8.0) * ((data.player2Data.px *1.0) / MoveMachine::Machine::TILE_WIDTH) - WINDOW_WIDTH / 16;
        dst_rect.y = WINDOW_HEIGHT * 3.0 / 6.0 - (WINDOW_HEIGHT / 6.0) * ((data.player2Data.py *1.0) / MoveMachine::Machine::TILE_HEIGHT);
        dst_rect.w = WINDOW_WIDTH / 8.0;
        dst_rect.h = WINDOW_HEIGHT / 2.0;
        SDL_FRect src_rect;
        src_rect.x = 400;
        src_rect.y = 0;
        src_rect.w = 200;
        src_rect.h = 450;
        SDL_RenderTextureRotated(renderer,player,&src_rect,&dst_rect,0,nullptr,SDL_FlipMode::SDL_FLIP_HORIZONTAL);
    }
    if(data.player2Data.sprite == "PunchWindup") {
        dst_rect.x = WINDOW_WIDTH * 3.0 / 4.0 - WINDOW_WIDTH / 8.0 - (WINDOW_WIDTH / 8.0) * ((data.player2Data.px *1.0) / MoveMachine::Machine::TILE_WIDTH) - WINDOW_WIDTH / 16;
        dst_rect.y = WINDOW_HEIGHT * 3.0 / 6.0 - (WINDOW_HEIGHT / 6.0) * ((data.player2Data.py *1.0) / MoveMachine::Machine::TILE_HEIGHT);
        dst_rect.w = WINDOW_WIDTH / 4.0;
        dst_rect.h = WINDOW_HEIGHT / 2.0;
        SDL_FRect src_rect;
        src_rect.x = 0;
        src_rect.y = 450;
        src_rect.w = 400;
        src_rect.h = 450;
        SDL_RenderTextureRotated(renderer,player,&src_rect,&dst_rect,0,nullptr,SDL_FlipMode::SDL_FLIP_HORIZONTAL);
    }
    if(data.player2Data.sprite == "PunchFollow") {
        dst_rect.x = WINDOW_WIDTH * 3.0 / 4.0 - WINDOW_WIDTH / 8.0  - (WINDOW_WIDTH / 8.0) * ((data.player2Data.px *1.0) / MoveMachine::Machine::TILE_WIDTH) - WINDOW_WIDTH / 16;
        dst_rect.y = WINDOW_HEIGHT * 3.0 / 6.0 - (WINDOW_HEIGHT / 6.0) * ((data.player2Data.py *1.0) / MoveMachine::Machine::TILE_HEIGHT);
        dst_rect.w = WINDOW_WIDTH / 4.0;
        dst_rect.h = WINDOW_HEIGHT / 2.0;
        SDL_FRect src_rect;
        src_rect.x = 0;
        src_rect.y = 900;
        src_rect.w = 400;
        src_rect.h = 450;
        SDL_RenderTextureRotated(renderer,player,&src_rect,&dst_rect,0,nullptr,SDL_FlipMode::SDL_FLIP_HORIZONTAL);
    }
    if(data.player2Data.sprite == "KickWindup") {
        dst_rect.x = WINDOW_WIDTH * 3.0 / 4.0 - WINDOW_WIDTH * 2.0 / 8.0 - (WINDOW_WIDTH / 8.0) * ((data.player2Data.px *1.0) / MoveMachine::Machine::TILE_WIDTH) - WINDOW_WIDTH / 16;
        dst_rect.y = WINDOW_HEIGHT * 3.0 / 6.0 - (WINDOW_HEIGHT / 6.0) * ((data.player2Data.py *1.0) / MoveMachine::Machine::TILE_HEIGHT);
        dst_rect.w = WINDOW_WIDTH * 3.0 / 8.0;
        dst_rect.h = WINDOW_HEIGHT / 2.0;
        SDL_FRect src_rect;
        src_rect.x = 0;
        src_rect.y = 1350;
        src_rect.w = 600;
        src_rect.h = 450;
        SDL_RenderTextureRotated(renderer,player,&src_rect,&dst_rect,0,nullptr,SDL_FlipMode::SDL_FLIP_HORIZONTAL);
    }
    if(data.player2Data.sprite == "KickFollow") {
        dst_rect.x = WINDOW_WIDTH * 3.0 / 4.0 - WINDOW_WIDTH * 2.0 / 8.0 - (WINDOW_WIDTH / 8.0) * ((data.player2Data.px *1.0) / MoveMachine::Machine::TILE_WIDTH) - WINDOW_WIDTH / 16;
        dst_rect.y = WINDOW_HEIGHT * 3.0 / 6.0 - (WINDOW_HEIGHT / 6.0) * ((data.player2Data.py *1.0) / MoveMachine::Machine::TILE_HEIGHT);
        dst_rect.w = WINDOW_WIDTH * 3.0 / 8.0;
        dst_rect.h = WINDOW_HEIGHT / 2.0;
        SDL_FRect src_rect;
        src_rect.x = 0;
        src_rect.y = 1800;
        src_rect.w = 600;
        src_rect.h = 450;
        SDL_RenderTextureRotated(renderer,player,&src_rect,&dst_rect,0,nullptr,SDL_FlipMode::SDL_FLIP_HORIZONTAL);
    }
    
    std::string str1 = std::format("Player 1 hp: {}",data.player1hp);
    TTF_SetTextString(text,str1.c_str(),str1.length());
    TTF_DrawRendererText(text,0,0);
    std::string str2 = std::format("Player 2 hp: {}",data.player2hp);
    TTF_SetTextString(text,str2.c_str(),str2.length());
    TTF_DrawRendererText(text,0,50);

    SDL_RenderPresent(renderer);  /* put it all on the screen! */
    

    return SDL_APP_CONTINUE;  /* carry on with the program! */
}