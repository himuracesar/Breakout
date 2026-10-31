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
#define TRAIL_SIZE 50
#define MAX_FISSURES 16

#define DEBUG

typedef struct {
  Vector2 position;
  Vector2 direction;
  float width;
  float height;
  float speed;
} Player;

typedef struct {
  Vector2 position;
  Vector2 direction;
  Vector2 trailPositions[TRAIL_SIZE];
  float radio;
  float speed;
  float initialSpeed;
  float radioX;
  float radioY;
  float elasticity;
  int trailIndex;
  int trailCount;
  uint32_t color;
} Ball;

typedef struct {
  Vector2 start;
  Vector2 end;
} Fissure;

typedef struct {
  Vector2 position;
  uint32_t color;
  bool visible;
  int flashFrames;
  Fissure *fissures[MAX_FISSURES];
  int numFissures;
  int life;
  int maxLife;
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

typedef struct {
  Vector2 position;
  Vector2 direction;           
  char* text; 
  uint32_t color;
  float maxLife;
  float life;  
  float speed;
  bool visible;
} FloatText;

extern Player *player;
extern Ball *ball;
extern Brick *bricks[BRICKS_BOARD_ROWS][BRICKS_BOARD_COLS];
extern Explosion *explosions[MAX_EXPLOSIONS];
extern FloatText *ftexts[MAX_EXPLOSIONS];

extern int nextExplosion;
extern int nextFloatText;
extern int totalBricks;
extern int points;
extern int level;
extern int combo;
extern float pointsScale; 
extern float pointsElastilicity;

extern float shakeIntensity;
extern float shakeDecay;

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
 * @param event SDL event.
 */
void Input(SDL_Event *event);

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
 * @param color Color of the brick.
 */
void DrawBrick(int row, int col, uint32_t color);

/**
 * Check the collision between the ball and the bricks that
 * are not broken.
 *
 */
void CheckCollisionWithBricks();

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

/**
 * Active the screen shake
 * 
 * @param force Force to move the screen
 */
void activeScreenShake(float force);

/**
 * Create an float text and init all values.
 * 
 */
void CreateFloatText();

/**
 * Reinit a float text.
 * 
 * @param text Float text.
 * @param position Position of the text.
 * @param color Color of the text.
 */
void ReinitFloatText(char* text, Vector2 position, uint32_t color);

/**
 * Update the snimation of the float text.
 * 
 * @param deltaTime Time betwee frames.
 */
void UpdateFloatText(double deltaTime);

/**
 * Draw the float text in the framebuffer.
 * 
 */
void DrawFloatText();

void GenerateFissure(int row, int col, int numFissures);

void DrawBrickAndFissures(int row, int col);

#endif