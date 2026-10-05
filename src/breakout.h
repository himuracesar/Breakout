#pragma once
#ifndef BREAKOUT_H
#define BREAKOUT_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <SDl3/SDL_events.h>

typedef struct {
    uint32_t x;
    uint32_t y;
} Vector2;

typedef struct {
    Vector2 position;
    int width;
    int height;
    int speed;
} Player;

Player* player;

/**
 * Initiliaze all the components of the game.
 * 
 */
void Init();

/**
 * Update the logic of the game.
 * 
 * @param deltaTime 
 */
void Update(float deltaTime);

/**
 * Draw all the sprite of the game.
 * 
 */
void Render();

/**
 * Release all the resources of the game.
 * 
 */
void Release();

/**
 * Handle the input events of the game.
 * 
 * @param event 
 */
void Input(SDL_Event* event);

Player* CreatePlayer(uint32_t x, uint32_t y, int width, int height);

#endif