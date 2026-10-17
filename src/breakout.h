#pragma once
#ifndef BREAKOUT_H
#define BREAKOUT_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <SDl3/SDL_events.h>

#include "math.h"
#include "text.h"

#define MARGIN_HORIZONTAL 20.0f
#define MARGIN_VERTICAL 10.0f
#define BRICK_WIDTH 80.0f
#define BRICK_HEIGHT 25.0f
#define BRICKS_BOARD_ROWS 10
#define BRICKS_BOARD_COLS 10
#define MAX_PARTICLES 200
#define MAX_EXPLOSIONS 10

#define DEBUG

typedef struct {
  Vector2 position;
  float width;
  float height;
  float speed;
} Player;

typedef struct {
  Vector2 position;
  Vector2 direction;
  float radio;
  float speed;
  float initialSpeed;
} Ball;

typedef struct {
  Vector2 position;
  uint32_t color;
  bool visible;
} Brick;

typedef struct {
  Vector2 position;
  Vector2 direction;
  float radio;
  float life;
  float maxLife;
  float speed;
  uint32_t color;
} Particle;

typedef struct {
  Particle* sparks[MAX_PARTICLES];
  int numParticlesLive;
} Explosion;

Player *player;
Ball *ball;
Brick *bricks[BRICKS_BOARD_ROWS][BRICKS_BOARD_COLS];
Explosion *explosions[MAX_EXPLOSIONS];

int nextExplosion;

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
void Update(double deltaTime);

/**
 * Draw all the sprites of the game.
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
void Input(SDL_Event *event, double deltaTime);

/**
 * Handle the ball's bouncing with the paddle (player).
 * Depends on which part o paddle the bouncing the ball's direction has a new
 * angle. If it's in the center of paddle the angle keeps same, the angle
 * increase in lateral of paddle. Maximum 60 degrees.
 *
 */
void HandlePaddleBounce();

/**
 * Create a Player object.
 *
 * @param x X position of the player.
 * @param y Y position of the player.
 * @param width Width of the player.
 * @param height Height of the player
 * @return Player* Pointer to created player object.
 */
Player *CreatePlayer(float x, float y, float width, float height);

/**
 * Create a Ball object
 *
 * @param x X position if the ball.
 * @param y Y position of the ball.
 * @param radio Radio of the ball.
 * @return Ball* Pointer to the created Ball object.
 */
Ball *CreateBall(float x, float y, float radio);

/**
 * Create all the bricks
 *
 */
void CreateBricks();

/**
 * Draw a brick if it's visible
 *
 * @param row Row in the matrix.
 * @param col Column in the matrix.
 */
void DrawBrick(int row, int col);

/**
 * Check the collision between the ball and the bricks that
 * are not broken.
 *
 */
void CheckCollisionWithBricks();

/**
 * Draw a point with the radio
 *
 * @param x X coordinate
 * @param y Y coordinate
 * @param radio Radio
 * @param color Color of the point
 */
void DrawPoint(float x, float y, float radio, uint32_t color);

/**
 * Create a Random Level object
 *
 */
void CreateRandomLevel();

/**
 * Create all the explosion. Malloc all th necesary memory.
 * 
 */
void CreateExplosions();

/**
 * Update all the explosion that have live particles.
 * 
 * @param deltaTime Time between frames
 */
void UpdateExplosions(double deltaTime);

/**
 * Draw all the live particles belong an explosion.
 *  
 */
void RenderExplosions();

/**
 * Reboot all the particles. The explosion list has a pointer that point to the next explosions to reboot.
 * 
 * @param position Position where the particle has to create.
 * @param color Color of the particles of the explosion.
 */
void ReinitExplosion(Vector2 position, uint32_t color);

#endif