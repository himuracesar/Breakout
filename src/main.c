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
    SDL_SetRenderVSync(renderer, 1);
    if (renderer == NULL) {
        SDL_Log("It could not create the renderer: %s \n", SDL_GetError());
        fprintf(stderr, "It could not create the renderer: %s \n", SDL_GetError());

        SDL_DestroyWindow(window);
        SDL_Quit();

        return EXIT_FAILURE;
    }

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, WINDOW_WIDTH, WINDOW_HEIGHT);

    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
    //SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_NONE);
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

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

    uint64_t NOW = SDL_GetPerformanceCounter();
    uint64_t LAST = 0;
    double deltaTime = 0.0;

    uint64_t fpsTimer = SDL_GetPerformanceCounter();
    int frameCount = 0;
    int currentFPS = 0;
    uint64_t freq = SDL_GetPerformanceFrequency();

    srand(time(NULL));

    Init();

    while(isRunning) {
        LAST = NOW;
        NOW = SDL_GetPerformanceCounter();
        
        // Delta time en segundos (ej. 0.016s para 60 FPS)
        deltaTime = (double)((NOW - LAST) / (double)freq);

        // Protección contra saltos gigantes (ej. si minimizas la ventana)
        if (deltaTime > 0.25) {
            deltaTime = 0.25; 
        }

        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_EVENT_QUIT || (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE)) {
                isRunning = false;
            }

            Input(&event);
        }

        Clear(0xFF000000);

        Update(deltaTime); // Assuming 60 FPS
        Render();
        
        SDL_UpdateTexture(texture, NULL, framebuffer, WINDOW_WIDTH * sizeof(uint32_t));

        float offsetx = 0.0f;
        float offsety = 0.0f;

        if (shakeIntensity > 0.0f) {
            offsetx = (((float)rand() / RAND_MAX) * 2.0f - 1.0f) * shakeIntensity;
            offsety = (((float)rand() / RAND_MAX) * 2.0f - 1.0f) * shakeIntensity;
        }

        SDL_FRect rect = {
            offsetx, 
            offsety, 
            (float)WINDOW_WIDTH, 
            (float)WINDOW_HEIGHT
        };

        SDL_RenderClear(renderer);
        SDL_RenderTexture(renderer, texture, NULL, &rect);
        SDL_RenderPresent(renderer);

        frameCount++;
        if ((NOW - fpsTimer) >= freq) {
            currentFPS = frameCount;
            frameCount = 0;
            fpsTimer = NOW;
            
            // Imprimir FPS en la consola de depuración
            //SDL_Log("FPS: %d | Delta Time: %.4f ms", currentFPS, deltaTime * 1000.0);
        }
    }

    Release();

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}