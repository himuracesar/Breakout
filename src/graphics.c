#pragma once

#include "graphics.h"

uint32_t framebuffer[WINDOW_WIDTH * WINDOW_HEIGHT];

/**
 * Clear the screen with a specific color.
 * 
 * @param color Color to clear the screen (framebuffer) with.
 */
void Clear(uint32_t color) {
    for(uint32_t i = 0; i < WINDOW_WIDTH * WINDOW_HEIGHT; i++) {
        framebuffer[i] = color;
    }
}

/**
 * Draw a pixel on the screen.
 * 
 * @param x X coordinate of the pixel.
 * @param y Y coordinate of the pixel.
 * @param color Color of the pixel.
 */
void DrawPixel(int x, int y, uint32_t color) {
    if(x < 0 || x >= WINDOW_WIDTH || y < 0 || y >= WINDOW_HEIGHT) {
        return; // Out of bounds
    }

    framebuffer[y * WINDOW_WIDTH + x] = color;
}

uint32_t GetRandomColor(int alpha) {
    uint32_t r = rand() % 256;
    uint32_t g = rand() % 256;
    uint32_t b = rand() % 256;
    uint32_t a = alpha % 256;

    return (a << 24) | (r << 16) | (g << 8) | b;
}