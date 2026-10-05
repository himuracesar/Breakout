#include "breakout.h"
#include "graphics.h"

/**
 * Initialize all the components of the game.
 * 
 */
void Init() {
    player = CreatePlayer(0, 500, 125, 25);
}

void Update(float deltaTime) {

}

void Render() {
    //Draw the player
    for(int y = 0; y < player->height; y++){
        for(int x = 0; x < player->width; x++){
            DrawPixel(x + player->position.x, y + player->position.y, 0xD1D1D1);
        }
    }
}

void Release() {
    free(player);
}

void Input(SDL_Event* event) {
    if(event->type == SDL_EVENT_KEY_DOWN){
        if(event->key.key == SDLK_A){
            int x = player->position.x - player->speed;
            player->position.x = (x < 0) ? 0 : x;
        } else if(event->key.key == SDLK_D){
            int x = player->position.x + player->speed;
            player->position.x = (x + player->width > WINDOW_WIDTH) ? WINDOW_WIDTH - player->width : x;
        }
    } 
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
Player* CreatePlayer(uint32_t x, uint32_t y, int width, int height) {
    Player* pPlayer = malloc(sizeof(Player));
    if(pPlayer == NULL) {
        fprintf(stderr, "It could not create the player \n");

        return NULL;
    }

    pPlayer->position.x = x;
    pPlayer->position.y = y;
    pPlayer->width = width;
    pPlayer->height = height;
    pPlayer->speed = 10;

    return pPlayer;
}