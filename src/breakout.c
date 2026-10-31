#include "breakout.h"
#include "graphics.h"

Player *player = NULL;
Ball *ball = NULL;
Brick *bricks[BRICKS_BOARD_ROWS][BRICKS_BOARD_COLS] = {0};
Explosion *explosions[MAX_EXPLOSIONS] = {0};
FloatText *ftexts[MAX_EXPLOSIONS] = {0};

int nextExplosion = 0;
int nextFloatText = 0;
int totalBricks = 0;
int points = 0;
int level = 1;
int combo = 0;

float shakeIntensity = 0.0f; // Current force of the shake pixels
float shakeDecay = 0.90f;   
float pointsScale = 2.0f; 
float pointsElastilicity = 0.2f;

/**
 * Initialize all the components of the game.
 *
 */
void Init() {
  player = CreatePlayer(0.0f, 500.0f, 125.0f, 25.0f);
  if (player != NULL) {
    player->position.x = (WINDOW_WIDTH - player->width) / 2.0f;
  }

  ball = CreateBall(100.0f, 100.0f, 10.0f);

  nextExplosion = 0;
  nextFloatText = 0;
  points = 0;
  totalBricks = 0;
  level = 1;

  CreateBricks();
  CreateExplosions();
  CreateFloatText();
}

/**
 * Update the logic of the game.
 *
 * @param deltaTime
 */
void Update(double deltaTime) {
  if (ball == NULL || player == NULL)
    return;

  float x = player->position.x + player->direction.x * player->speed * (float)deltaTime;
  if(x + player->width <= WINDOW_WIDTH && x >= 0) {
    player->position.x = x;
  } else if(x + player->width > WINDOW_WIDTH) {
    player->position.x = WINDOW_WIDTH - player->width;
  } else if(x < 0) {
    player->position.x = 0.0f;
  }

  if (ball->speed == 0) {
    ball->position.x = player->position.x + (player->width / 2);
    ball->position.y = player->position.y - ball->radio;

    ball->radioX = ball->radio;
    ball->radioY = ball->radio;

    ball->trailPositions[ball->trailIndex].x = -10.0f;
    ball->trailPositions[ball->trailIndex].y = -10.0f;
    
    ball->trailIndex = (ball->trailIndex + 1) % TRAIL_SIZE;
    
    if (ball->trailCount < TRAIL_SIZE) {
      ball->trailCount++;
    }

    combo = 0;
  } else {
    float factor = ball->elasticity * (60.0f * deltaTime);
    if (factor > 1.0f) 
      factor = 1.0f;

    ball->radioX = lerp(ball->radioX, ball->radio, factor);
    ball->radioY = lerp(ball->radioY, ball->radio, factor);

    ball->trailPositions[ball->trailIndex].x = ball->position.x;
    ball->trailPositions[ball->trailIndex].y = ball->position.y;
    
    ball->trailIndex = (ball->trailIndex + 1) % TRAIL_SIZE;
    
    if (ball->trailCount < TRAIL_SIZE) {
      ball->trailCount++;
    }

    ball->position.x += ball->direction.x * ball->speed * (float)deltaTime;
    ball->position.y += ball->direction.y * ball->speed * (float)deltaTime;
  }

  if (ball->position.y > WINDOW_HEIGHT) {
    ball->speed = 0.0f;
    ball->radioX = ball->radio * 1.4f;
    ball->radioY = ball->radio * 0.6f;
  } else if (ball->position.y - ball->radio < 0.0f) {
    ball->direction.y = -ball->direction.y;
    ball->radioX = ball->radio * 1.4f;
    ball->radioY = ball->radio * 0.6f;
  } else if (ball->position.x - ball->radio < 0.0f) {
    ball->direction.x = -ball->direction.x;
    ball->radioX = ball->radio * 0.6f;
    ball->radioY = ball->radio * 1.4f;
  } else if (ball->position.x + ball->radio > WINDOW_WIDTH) {
    ball->direction.x = -ball->direction.x;
    ball->radioX = ball->radio * 0.6f;
        ball->radioY = ball->radio * 1.4f;
  } else if (ball->position.y + ball->radio > player->position.y &&
             ball->position.y - ball->radio < player->position.y + player->height &&
             ball->position.x >= player->position.x &&
             ball->position.x <= player->position.x + player->width) {
    HandlePaddleBounce();
  }

  CheckCollisionWithBricks();

  UpdateExplosions(deltaTime);

  if (shakeIntensity > 0.1f) {
      shakeIntensity *= shakeDecay;
  } else {
      shakeIntensity = 0.0f; 
  }

  float factor = pointsElastilicity * (60.0f * deltaTime);
  if (factor > 1.0f) 
    factor = 1.0f;

  pointsScale = lerp(pointsScale, 2.0f, factor);

  UpdateFloatText(deltaTime);
}

/**
 * Draw all the sprites of the game.
 *
 */
void Render() {
  if (player == NULL || ball == NULL)
    return;

  // Draw the player
  for (int y = 0; y < player->height; y++) {
    for (int x = 0; x < player->width; x++) {
      DrawPixel(x + player->position.x, y + player->position.y, 0xFFD1D1D1);
    }
  }

  // Draw the ball
  Vector2 startPoints = { ball->position.x - ball->radioX - 1, ball->position.y - ball->radioY - 1 };
  Vector2 endPoints   = { ball->position.x + ball->radioX + 1, ball->position.y + ball->radioY + 1 };

  // Basic Clipping
  if (startPoints.x < 0) 
    startPoints.x = 0;
  if (startPoints.y < 0) 
    startPoints.y = 0;
  if (endPoints.x >= WINDOW_WIDTH)  
    endPoints.x = WINDOW_WIDTH - 1;
  if (endPoints.y >= WINDOW_HEIGHT) 
    endPoints.y = WINDOW_HEIGHT - 1;

  // Precalculate 1 / (radio^2) 
  float invR2X = 1.0f / (ball->radioX * ball->radioX);
  float invR2Y = 1.0f / (ball->radioY * ball->radioY);

  for (int y = (int)startPoints.y; y <= (int)endPoints.y; y++) {
      // Calcular la distancia vertical al centro al cuadrado multiplicada por su factor
      float dy = (float)y - ball->position.y;
      float termY = (dy * dy) * invR2Y;

      for (int x = (int)startPoints.x; x <= (int)endPoints.x; x++) {
          float dx = (float)x - ball->position.x;
          float termX = (dx * dx) * invR2X;

          // Ellipse equation: if the sum is <= 1.0 the pixel is inside
          if (termX + termY <= 1.0f) {
              DrawPixel(x, y, ball->color); 

              // Nota: Si quieres que tu estela use esta misma función con transparencia, 
              // puedes pasar un parámetro alpha aquí y usar tu lógica de blending.
          }
      }
  }

  #ifdef DEBUG
    DrawPoint(ball->position.x, ball->position.y, 2.0f, 0xFFFF0000);
    DrawPoint(ball->position.x, ball->position.y - ball->radio, 2.0f, 0xFFFF0000);
    DrawPoint(ball->position.x - ball->radio, ball->position.y, 2.0f, 0xFFFF0000);
    DrawPoint(ball->position.x, ball->position.y + ball->radio, 2.0f, 0xFFFF0000);
    DrawPoint(ball->position.x + ball->radio, ball->position.y, 2.0f, 0xFFFF0000);
  #endif

  // Draw the bricks
  for (int i = 0; i < BRICKS_BOARD_ROWS; i++) {
    for (int j = 0; j < BRICKS_BOARD_COLS; j++) {
      if (bricks[i][j]) {
        DrawBrickAndFissures(i, j);
      }
    }
  }

  RenderExplosions();

  // Draw wake
  for(int i = 0; i < ball->trailCount; i++) {
    int idx = (ball->trailIndex - ball->trailCount + i + TRAIL_SIZE) % TRAIL_SIZE;
    
    //scale
    float factor = (float)i / ball->trailCount; 
    uint8_t alpha = (uint8_t)(factor * 150);

    float widthHeightWake = ball->radio * (0.3f + 0.7f * factor);
    
    int posX = (int)(ball->trailPositions[idx].x + (ball->radio - widthHeightWake) / 2.0f);
    int posY = (int)(ball->trailPositions[idx].y + (ball->radio - widthHeightWake) / 2.0f);

    DrawRectangle(posX, posY, (int)widthHeightWake, (int)widthHeightWake, ball->color, alpha);
  }

  // Draw text
  char strpoints[12];
  snprintf(strpoints, sizeof(strpoints), "%01d", points);

  char strLevel[12];
  snprintf(strLevel, sizeof(strLevel), "%01d", level);

  DrawText(5, 5, "POINTS: ", 0xFFFFFFFF, 255, 2);

  int posX = 100 + pointsScale;
  int posY = 5 + pointsScale;
  DrawText(posX, posY, strpoints, 0xFFFFFFFF, 255, pointsScale);

  DrawText(5, 30, "LEVEL: ", 0xFFFFFFFF, 255, 2);
  DrawText(80, 30, strLevel, 0xFFFFFFFF, 255, 2);

  DrawFloatText();

  #ifdef DEBUG
    char strTotalBricks[12];
    snprintf(strTotalBricks, sizeof(totalBricks), "%01d", totalBricks);
    DrawText(80, WINDOW_HEIGHT - 80, strTotalBricks, 0xFFFFFF00, 255, 2);
  #endif
}

/**
 * Release all the resources of the game.
 *
 */
void Release() {
  if (player != NULL) {
    free(player);
    player = NULL;
  }

  if (ball != NULL) {
    free(ball);
    ball = NULL;
  }

  for (int i = 0; i < BRICKS_BOARD_ROWS; i++) {
    for (int j = 0; j < BRICKS_BOARD_COLS; j++) {
      if (bricks[i][j] != NULL) {
        free(bricks[i][j]);
        bricks[i][j] = NULL;
      }
    }
  }

  for (int j = 0; j < MAX_EXPLOSIONS; j++) {
    if (explosions[j] != NULL) {
      for (int i = 0; i < MAX_PARTICLES; i++) {
        if (explosions[j]->sparks[i] != NULL) {
          free(explosions[j]->sparks[i]);
          explosions[j]->sparks[i] = NULL;
        }
      }

      free(explosions[j]);
      explosions[j] = NULL;
    }
  }

  for (int j = 0; j < MAX_EXPLOSIONS; j++) {
    free(ftexts[j]);
    ftexts[j] = NULL;
  }
}

/**
 * Handle the input events of the game.
 *
 * @param event
 */
void Input(SDL_Event *event) {
  if (player == NULL || ball == NULL)
    return;

  player->direction.x = 0.0f;
  if (event->type == SDL_EVENT_KEY_DOWN) {
    if (event->key.key == SDLK_A) {
      player->direction.x = -1.0f;
    } else if (event->key.key == SDLK_D) {
      player->direction.x = 1.0f;
    }

    if (event->key.key == SDLK_SPACE && ball->speed == 0) {
      ball->speed = ball->initialSpeed;
      ball->direction.x = 0.0f;
      ball->direction.y = -1.0f;
    }
  }
}

/**
 * Handle the ball's bouncing with the paddle (player).
 * Depends on which part o paddle the bouncing the ball's direction has a new
 * angle. If it's in the center of paddle the angle keeps same, the angle
 * increase in lateral of paddle. Maximum 60 degrees.
 *
 */
void HandlePaddleBounce() {
  if (player == NULL || ball == NULL)
    return;

  // 1. Calculate the relative distance from center (-1.0f to 1.0f)
  float paddleCenterX = player->width / 2.0f + player->position.x;
  float relativeIntersectX = (ball->position.x - paddleCenterX) / (player->width / 2.0f);

  ball->radioX = ball->radio * 1.4f;
  ball->radioY = ball->radio * 0.6f;

  if (relativeIntersectX < -1.0f) 
    relativeIntersectX = -1.0f;
  if (relativeIntersectX > 1.0f)  
    relativeIntersectX = 1.0f;

  // --- ARCADE 1: Minimum angle in the center
  // If the ball hit the center of the paddle. There ir a minimum detour to avoid vertical infinite bouncings.
  if (fabsf(relativeIntersectX) < 0.05f) {
      relativeIntersectX = (ball->direction.x >= 0.0f) ? 0.05f : -0.05f;
  }

  // --- ARCADE 2: Curveball ---
  float factor_pala = player->direction.x * player->speed * 0.5f; // Ajusta este multiplicador según la escala de tus unidades
  relativeIntersectX += factor_pala;
  if (relativeIntersectX < -1.0f) 
    relativeIntersectX = -1.0f;
  if (relativeIntersectX > 1.0f)  
    relativeIntersectX = 1.0f;
  

  // 2. Define maximum angle of bounce (~60 o 70 degrees)
  float maxBounceAngle = 1.13446f; // ~65 degrees
  float bounceAngle = relativeIntersectX * maxBounceAngle;

  Vector2 dir;
  dir.x = sinf(bounceAngle);
  dir.y = -cosf(bounceAngle); 

  ball->direction = normalized(dir);

  if(totalBricks == 0)
    CreateRandomLevel();

  combo = 0;
}

/**
 * Create a Player object
 *
 * @param x Coordinate X of the player.
 * @param y Coordinate Y of the player.
 * @param width Width of the player.
 * @param height Height of the player.
 * @return Player* Pointer to the created Player object.
 */
Player *CreatePlayer(float x, float y, float width, float height) {
  Player *pPlayer = malloc(sizeof(Player));
  if (pPlayer == NULL) {
    fprintf(stderr, "It could not create the player \n");

    return NULL;
  }

  pPlayer->position.x = x;
  pPlayer->position.y = y;
  pPlayer->width = width;
  pPlayer->height = height;
  pPlayer->speed = 400.0f;
  pPlayer->direction = (Vector2){ 0.0f, 0.0f };

  return pPlayer;
}

/**
 * Create a Ball object
 *
 * @param x X position if the ball.
 * @param y Y position of the ball.
 * @param radio Radio of the ball.
 * @return Ball* Pointer to the created Ball object.
 */
Ball *CreateBall(float x, float y, float radio) {
  Ball *pBall = malloc(sizeof(Ball));
  if (pBall == NULL) {
    fprintf(stderr, "It could not create the ball \n");

    return NULL;
  }

  pBall->position.x = x;
  pBall->position.y = y;
  pBall->radio = radio;
  pBall->initialSpeed = 400.0f;
  pBall->speed = 0.0f;
  pBall->trailCount = 0;
  pBall->trailIndex = 0;
  pBall->color = 0xFFFFFFFF;
  pBall->radioX = radio;
  pBall->radioY = radio;
  pBall->elasticity = 0.03f;

  for(int i = 0; i < TRAIL_SIZE; i++) {
    pBall->trailPositions[i].x = 0.0f; 
    pBall->trailPositions[i].y = 0.0f;
  }

  return pBall;
}

/**
 * Create all the bricks
 *
 */
void CreateBricks() {
  for (int i = 0; i < BRICKS_BOARD_ROWS; i++) {
    for (int j = 0; j < BRICKS_BOARD_COLS; j++) {
      bricks[i][j] = malloc(sizeof(Brick));
      if (bricks[i][j] == NULL) {
        fprintf(stderr, "It could not create brick at [%d][%d]\n", i, j);
        continue;
      }

      bricks[i][j]->visible = true;
      bricks[i][j]->color = GetRandomColor(128);
      bricks[i][j]->position.x = j * BRICK_WIDTH;
      bricks[i][j]->position.y = i * BRICK_HEIGHT;
      bricks[i][j]->flashFrames = 0;
      bricks[i][j]->maxLife = 1;
      bricks[i][j]->life = bricks[i][j]->maxLife;
      bricks[i][j]->numFissures = 0;

      for(int k = 0; k < MAX_FISSURES; k++)
        bricks[i][j]->fissures[k] = malloc(sizeof(Fissure));
    }
  }

  totalBricks = BRICKS_BOARD_COLS * BRICKS_BOARD_ROWS;
}

/**
 * Draw a brick if it's visible
 *
 * @param row Row in the matrix.
 * @param col Column in the matrix.
 * @param color Color of the brick.
 */
void DrawBrick(int row, int col, uint32_t color) {
  if (bricks[row][col] == NULL || !bricks[row][col]->visible)
    return;

  for (int i = 0; i < BRICK_HEIGHT; i++) {
    for (int j = 0; j < BRICK_WIDTH; j++) {
      int y = row * BRICK_HEIGHT + i;
      int x = col * BRICK_WIDTH + j;

      DrawPixel(x, y, color);
    }
  }

  #ifdef DEBUG
    for (int i = 0; i < BRICKS_BOARD_ROWS; i++) {
      for (int j = 0; j < BRICKS_BOARD_COLS; j++) {
        if (bricks[i][j] != NULL && bricks[i][j]->visible)
          DrawPoint(bricks[i][j]->position.x, bricks[i][j]->position.y, 1.0f, 0xFFFFFFFF);
      }
    }
  #endif
}

/**
 * Check the collision between the ball and the bricks that
 * are not broken.
 *
 */
void CheckCollisionWithBricks() {
  if (ball == NULL || ball->speed == 0)
    return;

  Vector2 collisionPoints[4];
  // left
  collisionPoints[0].x = ball->position.x - ball->radio;
  collisionPoints[0].y = ball->position.y;
  // top
  collisionPoints[1].x = ball->position.x;
  collisionPoints[1].y = ball->position.y - ball->radio;
  // right
  collisionPoints[2].x = ball->position.x + ball->radio;
  collisionPoints[2].y = ball->position.y;
  // bottom
  collisionPoints[3].x = ball->position.x;
  collisionPoints[3].y = ball->position.y + ball->radio;

  for (int i = 0; i < 4; i++) {
    int row = (int)(collisionPoints[i].y / BRICK_HEIGHT);
    int col = (int)(collisionPoints[i].x / BRICK_WIDTH);

    if (row >= BRICKS_BOARD_ROWS || row < 0 || col < 0 || col >= BRICKS_BOARD_COLS)
      continue;

    if (bricks[row][col] == NULL || !bricks[row][col]->visible)
      continue;

    if (collisionPoints[i].x < bricks[row][col]->position.x + BRICK_WIDTH &&
        collisionPoints[i].x > bricks[row][col]->position.x &&
        collisionPoints[i].y < bricks[row][col]->position.y + BRICK_HEIGHT &&
        collisionPoints[i].y > bricks[row][col]->position.y) {
      if (i == 0) {
        ball->direction.x = -ball->direction.x;
        ball->position.x += ball->radio;
        ball->radioX = ball->radio * 0.6f;
        ball->radioY = ball->radio * 1.4f;
      } else if (i == 1) {
        ball->direction.y = -ball->direction.y;
        ball->position.y += ball->radio;
        ball->radioX = ball->radio * 1.4f;
        ball->radioY = ball->radio * 0.6f;
      } else if (i == 2) {
        ball->direction.x = -ball->direction.x;
        ball->position.x -= ball->radio;
        ball->radioX = ball->radio * 0.6f;
        ball->radioY = ball->radio * 1.4f;
      } else if (i == 3) {
        ball->direction.y = -ball->direction.y;
        ball->position.y -= ball->radio;
        ball->radioX = ball->radio * 1.4f;
        ball->radioY = ball->radio * 0.6f;
      }

      bricks[row][col]->flashFrames = 1;
      bricks[row][col]->life--;
      if(bricks[row][col]->life > 0) {
        GenerateFissure(row, col, MAX_FISSURES / bricks[row][col]->maxLife + 1); 
      } else if(bricks[row][col]->life == 0){
        totalBricks--;
      }

      combo++;

      points += combo;

      if(points % 8 == 0) {
        activeScreenShake(20.0f);
        pointsScale = 24.6f;
      } else {
        activeScreenShake(2.0f);
        pointsScale = 8.6f;
      }

      Vector2 explPos = (Vector2){ 
        bricks[row][col]->position.x + BRICK_WIDTH / 2.0f, bricks[row][col]->position.y + BRICK_HEIGHT / 2.0f,
      };

      ReinitExplosion(explPos, bricks[row][col]->color);

      char str[12];
      //str[0] = '\0';
      sprintf(str, "+%d", combo);

      ReinitFloatText(str, explPos, bricks[row][col]->color);
    }
  }
}

/**
 * Create a Random Level object
 *
 */
void CreateRandomLevel() {
  int totalBrick = 0;

  for(int i = 0; i < BRICKS_BOARD_ROWS; i++) {
    for(int j = 0; j < BRICKS_BOARD_COLS; j++) {
      if (bricks[i][j] == NULL)
        continue;
      int r = (rand() % 100) % 2;
      if(r == 0) {
        bricks[i][j]->visible = true;
        bricks[i][j]->life = bricks[i][j]->maxLife;
        totalBricks++;
      }
    }
  }

  level++;
}

/**
 * Create an Explosion and init all values for the particles.
 * 
 */
void CreateExplosions() {
  for(int j = 0; j < MAX_EXPLOSIONS; j++) {
    explosions[j] = malloc(sizeof(Explosion));
    if (explosions[j] == NULL) {
      fprintf(stderr, "It could not create explosion %d\n", j);
      continue;
    }
    for(int i = 0; i < MAX_PARTICLES; i++) {
      explosions[j]->sparks[i] = malloc(sizeof(Particle));
      if (explosions[j]->sparks[i] == NULL) {
        continue;
      }
      explosions[j]->sparks[i]->color = GetRandomColor(255);
      explosions[j]->sparks[i]->direction = (Vector2){ 0.0f, 0.0f };
      explosions[j]->sparks[i]->position = (Vector2){ -10.0f, -10.0f };
      explosions[j]->sparks[i]->life = 0.0f;
      explosions[j]->sparks[i]->speed = 0.0f;
      explosions[j]->sparks[i]->radio = 0.0f;
    }

    explosions[j]->numParticlesLive = 0;
  }
}

/**
 * Create all the explosion. Malloc all th necesary memory.
 * 
 */
void UpdateExplosions(double deltaTime) {
  for(int j = 0; j < MAX_EXPLOSIONS; j++) {
    if (explosions[j] == NULL)
      continue;

    int count = 0;

    if(explosions[j]->numParticlesLive > 0) {
      for(int i = 0; i < MAX_PARTICLES; i++) {
        if(explosions[j]->sparks[i] == NULL || explosions[j]->sparks[i]->life <= 0.0f)
          continue;

        count++;

        float g = 150.0f * (float)deltaTime;

        explosions[j]->sparks[i]->life -= (float)deltaTime;
        explosions[j]->sparks[i]->position.x += explosions[j]->sparks[i]->direction.x * (float)deltaTime * explosions[j]->sparks[i]->speed;
        explosions[j]->sparks[i]->position.y += explosions[j]->sparks[i]->direction.y * (float)deltaTime * explosions[j]->sparks[i]->speed + g;
      }
    }

    explosions[j]->numParticlesLive = count;
  }
}

/**
 * Draw all the live particles belong an explosion.
 *  
 */
void RenderExplosions() {
  for(int j = 0; j < MAX_EXPLOSIONS; j++) {
    if (explosions[j] == NULL)
      continue;

    if(explosions[j]->numParticlesLive > 0) {
      for(int i = 0; i < MAX_PARTICLES; i++) {
        if(explosions[j]->sparks[i] == NULL || explosions[j]->sparks[i]->life <= 0.0f)
          continue;

        DrawPoint(explosions[j]->sparks[i]->position.x, explosions[j]->sparks[i]->position.y, explosions[j]->sparks[i]->radio, explosions[j]->sparks[i]->color);
      }
    }
  }
}

/**
 * Reboot all the particles. The explosion list has a pointer that point to the next explosions to reboot.
 * 
 * @param position Position where the particle has to create.
 * @param color Color of the particles of the explosion.
 */
void ReinitExplosion(Vector2 position, uint32_t color) {
  const float maxSpeed = 300.0f;
  const float minSpeed = 100.0f;
  const float minRadio = 1.0f;
  const float maxRadio = 3.0f;

  if (explosions[nextExplosion] == NULL)
    return;

  for(int i = 0; i < MAX_PARTICLES; i++) {
    float randomAngle = ((float)rand() / (float)RAND_MAX) * (2.0f * M_PI);

    Particle *p = explosions[nextExplosion]->sparks[i];
    if (p == NULL) {
      p = malloc(sizeof(Particle));
      if (p == NULL)
        continue;
      explosions[nextExplosion]->sparks[i] = p;
    }

    p->color = color;
    p->direction = (Vector2){ cosf(randomAngle), sinf(randomAngle) };
    p->position = position;
    p->life = 5.0f;
    p->speed = minSpeed + ((float)rand() / (float)RAND_MAX) * (maxSpeed - minSpeed);
    p->radio = minRadio + ((float)rand() / (float)RAND_MAX) * (maxRadio - minRadio);
  }

  explosions[nextExplosion]->numParticlesLive = MAX_PARTICLES;

  nextExplosion = (nextExplosion + 1 == MAX_EXPLOSIONS) ? 0 : nextExplosion + 1;
}

/**
 * Active the screen shake
 * 
 * @param force Force to move the screen
 */
void activeScreenShake(float force) {
    shakeIntensity = force;
}

/**
 * Create an float text and init all values.
 * 
 */
void CreateFloatText() {
  for(int j = 0; j < MAX_EXPLOSIONS; j++) {
    ftexts[j] = malloc(sizeof(FloatText));
    if (ftexts[j] == NULL) {
      fprintf(stderr, "It could not create float text %d\n", j);
      continue;
    }

    ftexts[j]->position = (Vector2) { 0.0f, 0.0f };
    ftexts[j]->direction = (Vector2) { 0.0f, -10.0f };
    ftexts[j]->speed =5.0f;
    ftexts[j]->life = 0.0f;
    ftexts[j]->maxLife = 2.0f;
    ftexts[j]->visible = false;
    ftexts[j]->text = NULL;
  }
}

/**
 * Reinit a float text.
 * 
 * @param text Float text.
 * @param position Position of the text.
 * @param color Color of the text.
 */
void ReinitFloatText(char* text, Vector2 position, uint32_t color) {
  if(text == NULL) {
    fprintf(stderr, "The text is null \n");
    return;
  }

  if (ftexts[nextFloatText] == NULL) {
    fprintf(stderr, "It could not reinit float text %d\n", nextFloatText);
    return;
  }
  
  ftexts[nextFloatText]->position = position;
  ftexts[nextFloatText]->color = color;
  ftexts[nextFloatText]->life = ftexts[nextFloatText]->maxLife;
  ftexts[nextFloatText]->visible = true;
  
  if (ftexts[nextFloatText]->text != NULL) {
      free(ftexts[nextFloatText]->text); 
  }

  ftexts[nextFloatText]->text = strdup(text);

  nextFloatText++;
  nextFloatText = nextFloatText % MAX_EXPLOSIONS;
}

/**
 * Update the snimation of the float text.
 * 
 * @param deltaTime Time betwee frames.
 */
void UpdateFloatText(double deltaTime) {
  for (int i = 0; i < MAX_EXPLOSIONS; i++) {
    if (ftexts[i]->life > 0.0f) {
        ftexts[i]->position.y += ftexts[i]->direction.y * ftexts[i]->speed * (float)deltaTime;
        
        ftexts[i]->life -= (float)deltaTime;
    }
  }
}

/**
 * Draw the float text in the framebuffer.
 * 
 */
void DrawFloatText() {
  for (int i = 0; i < MAX_EXPLOSIONS; i++) {
    if (ftexts[i]->life > 0.0f) {
        float lifeRate = ftexts[i]->life / ftexts[i]->maxLife;
        uint8_t alpha = (uint8_t)(lifeRate * 255);

        float currentX = ftexts[i]->position.x;
        float currentY = ftexts[i]->position.y;

        DrawText(currentX, currentY, ftexts[i]->text, ftexts[i]->color, alpha, 3);
    }
  }
}

/**
 * 
 * 
 * @param row 
 * @param col 
 * @param cantidad_nuevas 
 */
void GenerateFissure(int row, int col, int numFissures) {
  Brick* brick = bricks[row][col];

  // Si ya alcanzamos el límite de líneas, no agregamos más
  if (brick->numFissures >= MAX_FISSURES) 
    return;

  for (int i = 0; i < numFissures; i++) {
      if (brick->numFissures >= MAX_FISSURES) 
        break;

      Vector2 start;
      
      if (brick->numFissures == 0) {
          // La primera grieta inicia cerca del centro del brick (con un toque aleatorio)
          start.x = brick->position.x + (BRICK_WIDTH / 2) + (rand() % 11 - 5);
          start.y = brick->position.y + (BRICK_HEIGHT / 2) + (rand() % 11 - 5);
      } else {
          // Las siguientes grietas ramifican desde el final de una grieta existente
          int indice_ancestro = rand() % brick->numFissures;
          start = brick->fissures[indice_ancestro]->end;
      }

      // Determinar un punto final aleatorio pero manteniéndolo dentro del brick
      Vector2 end;
      end.x = start.x + (rand() % 21 - 10); // Desplazamiento en X (-10 a 10)
      end.y = start.y + (rand() % 21 - 10); // Desplazamiento en Y (-10 a 10)

      // Restringir el punto final para que no se salga de los límites del brick
      if (end.x < brick->position.x + 2) end.x = brick->position.x + 2;
      if (end.x > brick->position.x + BRICK_WIDTH - 3) end.x = brick->position.x + BRICK_WIDTH - 3;
      if (end.y < brick->position.y + 2) end.y = brick->position.y + 2;
      if (end.y > brick->position.y + BRICK_HEIGHT - 3) end.y = brick->position.y + BRICK_HEIGHT - 3;

      // Guardar el segmento
      brick->fissures[brick->numFissures]->start = start;
      brick->fissures[brick->numFissures]->end = end;
      brick->numFissures++;
  }
}

void DrawBrickAndFissures(int row, int col) {
  Brick *brick = bricks[row][col];

  if (!brick->visible) 
    return;

  uint32_t color = brick->color;

  if (brick->flashFrames > 0) {
      color = 0xFFFFFFFF; // Destello blanco de impacto
  } else if (brick->life < brick->maxLife) {
      // TRUCO: Oscurecer el brick proceduralmente según el daño recibido
      uint8_t r = (brick->color >> 16) & 0xFF;
      uint8_t g = (brick->color >> 8) & 0xFF;
      uint8_t b = brick->color & 0xFF;
      
      // Reducir componentes un 30% por ejemplo
      r = (uint8_t)(r * 0.7f);
      g = (uint8_t)(g * 0.7f);
      b = (uint8_t)(b * 0.7f);
      color = (0xFF000000) | (r << 16) | (g << 8) | b;
  }

  DrawBrick(row, col, color);

  // [Aquí pones tus bucles FOR actuales para rellenar el rectángulo del brick con 'color_a_dibujar']
  // ... (Tu código actual de rellenado de píxeles del brick) ...

  // Si no está en frame de flash y tiene grietas, las dibujamos encima del rectángulo ya pintado
  if (brick->flashFrames <= 0 && brick->numFissures > 0) {
      uint32_t colorFissure = 0xFF1A1A1A; // Gris muy oscuro casi negro
      
      for (int i = 0; i < brick->numFissures; i++) {
          DrawLine(
              (int)brick->fissures[i]->start.x, 
              (int)brick->fissures[i]->start.y, 
              (int)brick->fissures[i]->end.x, 
              (int)brick->fissures[i]->end.y, 
              colorFissure
          );
      }
  }
  
  if (brick->flashFrames > 0) 
    brick->flashFrames--;

  //brick->life--;
  if(brick->life <= 0)
    brick->visible = false;
}