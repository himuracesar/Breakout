#include <SDL3/SDL.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "breakout.h"
#include "graphics.h"

int main(int argc, char* argv[]) {
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    SDL_Texture* texture = NULL;
    SDL_Event event;

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("It could not initialize SDL: %s \n", SDL_GetError());
        fprintf(stderr, "It could not initialize SDL: %s \n", SDL_GetError());

        return EXIT_FAILURE;
    }

    window = SDL_CreateWindow("Breakout", WINDOW_WIDTH, WINDOW_HEIGHT, 0);

    if (window == NULL) {
        SDL_Log("It could not create the window: %s \n", SDL_GetError());
        fprintf(stderr, "It could not create the window: %s \n", SDL_GetError());
        SDL_Quit();

        return EXIT_FAILURE;
    }

    renderer = SDL_CreateRenderer(window, NULL);
    if (renderer == NULL) {
        SDL_Log("It could not create the renderer: %s \n", SDL_GetError());
        fprintf(stderr, "It could not create the renderer: %s \n", SDL_GetError());

        SDL_DestroyWindow(window);
        SDL_Quit();

        return EXIT_FAILURE;
    }

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_XRGB8888, SDL_TEXTUREACCESS_STREAMING, WINDOW_WIDTH, WINDOW_HEIGHT);

    printf("Último log de SDL: %s\n", SDL_GetError());

    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_NONE); 

    if(texture == NULL) {
        SDL_Log("It could not create the texture: %s \n", SDL_GetError());
        fprintf(stderr, "It could not create the texture: %s \n", SDL_GetError());

        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();

        return EXIT_FAILURE;
    }

    //----
    bool isRunning = true;

    Init();

    while(isRunning) {
        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_EVENT_QUIT || (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE)) {
                isRunning = false;
            }

            Input(&event);
        }

        Clear(0x00000000);

        Update(0.016f); // Assuming 60 FPS
        Render();
        
        SDL_UpdateTexture(texture, NULL, framebuffer, WINDOW_WIDTH * sizeof(uint32_t));

        SDL_RenderClear(renderer);
        SDL_RenderTexture(renderer, texture, NULL, NULL);
        //SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
    }

    Release();

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}