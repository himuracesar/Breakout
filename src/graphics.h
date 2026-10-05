#pragma once
#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdint.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

extern uint32_t framebuffer[WINDOW_WIDTH * WINDOW_HEIGHT];

/**
 * Clear the screen with a specific color.
 *
 * @param color Color to clear the screen framebuffer.
 */
void Clear(uint32_t color);

/**
 * Draw a pixel on the screen.
 * 
 * @param x X coordinate of the pixel.
 * @param y Y coordinate of the pixel.
 * @param color Color of the pixel.
 */
void DrawPixel(int x, int y, uint32_t color);

#endif