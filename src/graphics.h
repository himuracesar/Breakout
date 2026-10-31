#pragma once
#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdint.h>

#include<SDL3/SDL.h>

#include "math.h"

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

/**
 * Draw a point with the radio.
 *
 * @param x X coordinate.
 * @param y Y coordinate.
 * @param radio Radio.
 * @param color Color of the point.
 */
void DrawPoint(float x, float y, float radio, uint32_t color);

/**
 * Get the Random Color
 * 
 * @param alpha Alpha channel of the color generated.
 * 
 * @return uint32_t Created color.
 */
uint32_t GetRandomColor(int alpha);

/**
 * Draw a rectangle in a custom position with width * height and custom color and alpha channel.
 * 
 * @param x X coordinate.
 * @param y Y coordinate.
 * @param width Width of the rectangle.
 * @param height Height of the rectangle
 * @param color Color of the rectangle.
 * @param alpha Alpha channel of the color.
 */
void DrawRectangle(int x, int y, int width, int height, uint32_t color, uint8_t alpha);

void DrawLine(int x0, int y0, int x1, int y1, uint32_t color);

#endif