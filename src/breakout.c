#include "breakout.h"
#include "graphics.h"

/**
 * Initialize all the components of the game.
 *
 */
void Init() {
  player = CreatePlayer(0.0f, 500.0f, 125.0f, 25.0f);

  player->position.x = (WINDOW_WIDTH - player->width) / 2.0f;

  ball = CreateBall(100.0f, 100.0f, 10.0f);

  nextExplosion = 0;

  CreateBricks();
  CreateExplosions();
}

/**
 * Update the logic of the game.
 *
 * @param deltaTime
 */
void Update(double deltaTime) {
  if (ball->speed == 0) {
    ball->position.x = player->position.x + (player->width / 2);
    ball->position.y = player->position.y - ball->radio;
  } else {
    ball->position.x += ball->direction.x * ball->speed * (float)deltaTime;
    ball->position.y += ball->direction.y * ball->speed * (float)deltaTime;
  }

  if (ball->position.y > WINDOW_HEIGHT) {
    ball->speed = 0.0f;
  } else if (ball->position.y - ball->radio < 0.0f) {
    ball->direction.y = -ball->direction.y;
  } else if (ball->position.x - ball->radio < 0.0f) {
    ball->direction.x = -ball->direction.x;
  } else if (ball->position.x + ball->radio > WINDOW_WIDTH) {
    ball->direction.x = -ball->direction.x;
  } else if (ball->position.y + ball->radio > player->position.y &&
             ball->position.y - ball->radio <
                 player->position.y + player->height &&
             ball->position.x >= player->position.x &&
             ball->position.x <= player->position.x + player->width) {
    HandlePaddleBounce();
  }

  CheckCollisionWithBricks();

  UpdateExplosions(deltaTime);
}

/**
 * Draw all the sprites of the game.
 *
 */
void Render() {
  // Draw the player
  for (int y = 0; y < player->height; y++) {
    for (int x = 0; x < player->width; x++) {
      DrawPixel(x + player->position.x, y + player->position.y, 0xFFD1D1D1);
    }
  }

  // Draw the ball
  Vector2 startPoints = {ball->position.x - ball->radio,
                         ball->position.y - ball->radio};
  Vector2 endPoints = {ball->position.x + ball->radio,
                       ball->position.y + ball->radio};
  for (int y = startPoints.y; y < endPoints.y; y++) {
    for (int x = startPoints.x; x < endPoints.x; x++) {
      float distance = GetDistance(ball->position, (Vector2){x, y});
      if (distance <= (float)ball->radio) {
        DrawPixel(x, y, 0xFFFFFFFF);
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
        DrawBrick(i, j);
      }
    }
  }

  RenderExplosions();

  DrawText(20, 20, "ABCDEFGHIJKLMPQRSTUVWXYZ", 0xFFFFFFFF, 5);
  DrawText(20, 80, "1234567890", 0xFFFFFFFF, 5);
}

/**
 * Draw a point with the radio
 *
 * @param x X coordinate
 * @param y Y coordinate
 * @param radio Radio
 */
void DrawPoint(float x, float y, float radio, uint32_t color) {
  Vector2 startPoints = {x - radio, y - radio};
  Vector2 endPoints = {x + radio, y + radio};

  for (int _y = startPoints.y; _y < endPoints.y; _y++) {
    for (int _x = startPoints.x; _x < endPoints.x; _x++) {
      float distance = GetDistance((Vector2){x, y}, (Vector2){_x, _y});
      if (distance <= radio) {
        DrawPixel(_x, _y, color);
      }
    }
  }
}

/**
 * Release all the resources of the game.
 *
 */
void Release() {
  free(player);
  free(ball);

  for (int i = 0; i < BRICKS_BOARD_ROWS; i++) {
    for (int j = 0; j < BRICKS_BOARD_COLS; j++) {
      free(bricks[i][j]);
    }
  }
}

/**
 * Handle the input events of the game.
 *
 * @param event
 */
void Input(SDL_Event *event, double deltaTime) {
  if (event->type == SDL_EVENT_KEY_DOWN) {
    if (event->key.key == SDLK_A) {
      float x = player->position.x - player->speed * (float)deltaTime;
      player->position.x = (x < 0) ? 0 : x;
    } else if (event->key.key == SDLK_D) {
      float x = player->position.x + player->speed * (float)deltaTime;
      player->position.x =
          (x + player->width > WINDOW_WIDTH) ? WINDOW_WIDTH - player->width : x;
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
  // Calculate the relative distance of the impact from the center of the paddle
  // (-1.0 to 1.0f)
  float paddleCenterX = player->width / 2.0f + player->position.x;
  float relativeIntersectX =
      (ball->position.x - paddleCenterX) / (player->width / 2.0f);

  // Limit for security for the ball does not pass the extremes
  if (relativeIntersectX < -1.0f)
    relativeIntersectX = -1.0f;

  if (relativeIntersectX > 1.0f)
    relativeIntersectX = 1.0f;

  // Define max angle of bounce (60 degree)
  float maxBounceAngle = 1.04719f; // ~60 grados
  float bounceAngle = relativeIntersectX * maxBounceAngle;

  ball->direction.x = ball->speed * (bounceAngle);
  ball->direction.y = ball->speed * -1.0f * cosf(bounceAngle);

  ball->direction = normalized(ball->direction);
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
  pPlayer->speed = 800.0f;

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
      bricks[i][j]->visible = true;
      bricks[i][j]->color = GetRandomColor(128);
      bricks[i][j]->position.x = j * BRICK_WIDTH;
      bricks[i][j]->position.y = i * BRICK_HEIGHT;
    }
  }
}

/**
 * Draw a brick if it's visible
 *
 * @param row Row in the matrix.
 * @param col Column in the matrix.
 */
void DrawBrick(int row, int col) {
  for (int i = 0; i < BRICK_HEIGHT; i++) {
    for (int j = 0; j < BRICK_WIDTH; j++) {
      int y = row * BRICK_HEIGHT + i;
      int x = col * BRICK_WIDTH + j;
      if (!bricks[row][col]->visible)
        continue;

      DrawPixel(x, y, bricks[row][col]->color);
    }
  }

  #ifdef DEBUG
    for (int i = 0; i < BRICKS_BOARD_ROWS; i++) {
      for (int j = 0; j < BRICKS_BOARD_COLS; j++) {
        if (bricks[i][j]->visible)
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
  if (ball->speed == 0)
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
    int row = collisionPoints[i].y / BRICK_HEIGHT;
    int col = collisionPoints[i].x / BRICK_WIDTH;

    if (row >= BRICKS_BOARD_ROWS || row < 0 || col < 0 ||
        col >= BRICKS_BOARD_COLS)
      continue;

    if (!bricks[row][col]->visible)
      continue;

    if (collisionPoints[i].x < bricks[row][col]->position.x + BRICK_WIDTH &&
        collisionPoints[i].x > bricks[row][col]->position.x &&
        collisionPoints[i].y < bricks[row][col]->position.y + BRICK_HEIGHT &&
        collisionPoints[i].y > bricks[row][col]->position.y) {
      if (i == 0) {
        ball->direction.x = -ball->direction.x;
        ball->position.x += ball->radio;
        // BRICK_WIDTH;
        ball->position.x += ball->radio;
      } else if (i == 1) {
        ball->direction.y = -ball->direction.y;
        ball->position.y += ball->radio;
      } else if (i == 2) {
        ball->direction.x = -ball->direction.x;
        ball->position.x -= ball->radio;
      } else if (i == 3) {
        ball->direction.y = -ball->direction.y;
        ball->position.y -= ball->radio;
      }

      bricks[row][col]->visible = false;

      Vector2 explPos = (Vector2){ 
        bricks[row][col]->position.x + BRICK_WIDTH / 2.0f, bricks[row][col]->position.y + BRICK_HEIGHT / 2.0f,
      };
      ReinitExplosion(explPos, bricks[row][col]->color);
    }
  }
}

/**
 * Create a Random Level object
 *
 */
void CreateRandomLevel() {}

/**
 * Create an Explosion and init all values for the particles.
 * 
 */
void CreateExplosions() {
  for(int j = 0; j < MAX_EXPLOSIONS; j++) {
    explosions[j] = malloc(sizeof(Explosion));
    for(int i = 0; i < MAX_PARTICLES; i++) {
      explosions[j]->sparks[i] = malloc(sizeof(Particle));
      explosions[j]->sparks[i]->color = GetRandomColor(255);
      explosions[j]->sparks[i]->direction = (Vector2){ 0.0f, 0.0f };
      explosions[j]->sparks[i]->position = (Vector2){ 0.0f, 0.0f };
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
    int count = 0;

    if(explosions[j]->numParticlesLive > 0) {
      for(int i = 0; i < MAX_PARTICLES; i++) {
        if(explosions[j]->sparks[i]->life <= 0.0f)
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
    if(explosions[j]->numParticlesLive > 0) {
      for(int i = 0; i < MAX_PARTICLES; i++) {
        if(explosions[j]->sparks[i]->life <= 0.0f)
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
  const float maxSpeed = 400.0f;
  const float minSpeed = 150.0f;
  const float minRadio = 1.0f;
  const float maxRadio = 3.0f;

  for(int i = 0; i < MAX_PARTICLES; i++) {
    float randomAngle = ((float)rand() / (float)RAND_MAX) * (2.0f * M_PI);

    explosions[nextExplosion]->sparks[i] = malloc(sizeof(Particle));
    explosions[nextExplosion]->sparks[i]->color = color;
    explosions[nextExplosion]->sparks[i]->direction = (Vector2){ cosf(randomAngle), sinf(randomAngle) };
    explosions[nextExplosion]->sparks[i]->position = position;
    explosions[nextExplosion]->sparks[i]->life = 10.0f;
    explosions[nextExplosion]->sparks[i]->speed = minSpeed + ((float)rand() / (float)RAND_MAX) * (maxSpeed - minSpeed);
    explosions[nextExplosion]->sparks[i]->radio = minRadio + ((float)rand() / (float)RAND_MAX) * (maxRadio - minRadio);
  }

  explosions[nextExplosion]->numParticlesLive = MAX_PARTICLES;

  nextExplosion = (nextExplosion + 1 == MAX_EXPLOSIONS) ? 0 : nextExplosion + 1;
}