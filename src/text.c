#include "text.h"

// Función auxiliar para traducir un carácter ASCII al índice de nuestra tabla
int GetFontIndex(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'Z') return c - 'A' + 10;
    if (c >= 'a' && c <= 'z') return c - 'a' + 10;
    if (c == ' ')  return 36;
    if (c == ':')  return 37;
    if (c == '-')  return 38;
    return 36;
}

// Dibujar un carácter individual
void DrawChar(int startX, int startY, char c, uint32_t color, int scale) {
    int index = GetFontIndex(c);
    int screenWidth = WINDOW_WIDTH;

    for (int y = 0; y < 7; y++) {
        uint8_t rowBits = FONT_5X7[index][y];
        
        // Bucle limpio de izquierda a derecha
        for (int x = 0; x < 5; x++) {
            if (rowBits & (1 << (4 - x))) {
                for (int sy = 0; sy < scale; sy++) {
                    for (int sx = 0; sx < scale; sx++) {
                        int px = startX + (x * scale) + sx;
                        int py = startY + (y * scale) + sy;
                        
                        if (px >= 0 && px < screenWidth && py >= 0) {
                            framebuffer[py * screenWidth + px] = color;
                        }
                    }
                }
            }
        }
    }
}

// Dibujar texto completo en pantalla
void DrawText(int startX, int startY, const char* text, uint32_t color, int scale) {
    int length = (int)strlen(text);
    int charWidth = 6 * scale; // 5 píxeles de la letra + 1 píxel de separación

    for (int i = 0; i < length; i++) {
        DrawChar(startX + (i * charWidth), startY, text[i], color, scale);
    }
}