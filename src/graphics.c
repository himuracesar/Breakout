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

/**
 * Get the Random Color
 * 
 * @param alpha Alpha channel of the color generated.
 * 
 * @return uint32_t Created color.
 */
uint32_t GetRandomColor(int alpha) {
    uint32_t r = rand() % 256;
    uint32_t g = rand() % 256;
    uint32_t b = rand() % 256;
    uint32_t a = alpha % 256;

    return (a << 24) | (r << 16) | (g << 8) | b;
}

/**
 * Draw a point with the radio.
 *
 * @param x X coordinate.
 * @param y Y coordinate.
 * @param radio Radio.
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
 * Draw a rectangle in a custom position with width * height and custom color and alpha channel.
 * 
 * @param x X coordinate.
 * @param y Y coordinate.
 * @param width Width of the rectangle.
 * @param height Height of the rectangle
 * @param color Color of the rectangle.
 * @param alpha Alpha channel of the color.
 */
void DrawRectangle(int x, int y, int width, int height, uint32_t color, uint8_t alpha) {
    if (alpha == 0) 
        return;
        
    int isOpaque = (alpha == 255);

    // Split the color's components (format ARGB / ORGB)
    uint8_t srcR = (color >> 16) & 0xFF;
    uint8_t srcG = (color >> 8)  & 0xFF;
    uint8_t srcB = color         & 0xFF;
    
    int xStart = (x < 0) ? 0 : x;
    int yStart = (y < 0) ? 0 : y;
    int xEnd = (x + width > WINDOW_WIDTH) ? WINDOW_WIDTH : x + width;
    int yEnd = (y + height > WINDOW_HEIGHT) ? WINDOW_HEIGHT : y + height;

    for (int y = yStart; y < yEnd; y++) {
        int rowIndex = y * WINDOW_WIDTH;
        
        for (int x = xStart; x < xEnd; x++) {
            int pixelIndex = rowIndex + x;

            if (isOpaque) {
                framebuffer[pixelIndex] = color;
            } else {
                // Read the color background
                uint32_t background = framebuffer[pixelIndex];
                uint8_t bgr = (background >> 16) & 0xFF;
                uint8_t bgg = (background >> 8)  & 0xFF;
                uint8_t bgb = background         & 0xFF;

                // Alpha Blending: background + (objeto - fondo) * alpha / 255
                uint8_t r = bgr + (((srcR - bgr) * alpha) >> 8);
                uint8_t g = bgg + (((srcG - bgg) * alpha) >> 8);
                uint8_t b = bgb + (((srcB - bgb) * alpha) >> 8);

                framebuffer[pixelIndex] = (0xFF000000) | (r << 16) | (g << 8) | b;
            }
        }
    }
}

/**
 * Bresenham Algorithm El algoritmo de Bresenham es el estándar de la industria en gráficos procedurales por usar solo aritmética de enteros (es ridículamente rápido)
 * 
 * @param x0 
 * @param y0 
 * @param x1 
 * @param y1 
 * @param color 
 */
void DrawLine(int x0, int y0, int x1, int y1, uint32_t color) {
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2;

    while (1) {
        // Asegurar límites de pantalla (Clipping)
        if (x0 >= 0 && x0 < WINDOW_WIDTH && y0 >= 0 && y0 < WINDOW_HEIGHT) {
            framebuffer[y0 * WINDOW_WIDTH + x0] = color;
        }
        
        if (x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        if (e2 >= dy) { 
            err += dy; x0 += sx; 
        }
        if (e2 <= dx) { 
            err += dx; 
            y0 += sy; 
        }
    }
}

