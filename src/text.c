#include "text.h"

// Función auxiliar para traducir un carácter ASCII al índice de nuestra tabla
int GetFontIndex(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'Z') return c - 'A' + 10;
    if (c >= 'a' && c <= 'z') return c - 'a' + 10;
    if (c == ' ')  return 36;
    if (c == ':')  return 37;
    if (c == '-')  return 38;
    if (c == '+')  return 39;
    return 36;
}

// Dibujar un carácter individual
/*void DrawChar(int startX, int startY, char c, uint32_t color, int scale) {
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
                        
                        if (px >= 0 && px < screenWidth && py >= 0 && py < WINDOW_HEIGHT) {
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
}*/

void DrawChar(int startX, int startY, char c, uint32_t color, uint8_t alpha, int scale) {
    int index = GetFontIndex(c);

    // Extraer las componentes RGB del color base del texto
    uint8_t sr = (color >> 16) & 0xFF;
    uint8_t sg = (color >> 8) & 0xFF;
    uint8_t sb = color & 0xFF;

    for (int y = 0; y < 7; y++) {
        uint8_t rowBits = FONT_5X7[index][y];
        
        for (int x = 0; x < 5; x++) {
            // Verificamos si el píxel de la fuente está activo
            if (rowBits & (1 << (4 - x))) {
                for (int sy = 0; sy < scale; sy++) {
                    for (int sx = 0; sx < scale; sx++) {
                        int px = startX + (x * scale) + sx;
                        int py = startY + (y * scale) + sy;
                        
                        // Validar límites de la pantalla
                        if (px >= 0 && px < WINDOW_WIDTH && py >= 0) {
                            int pixelIndex = py * WINDOW_WIDTH + px;

                            if (alpha == 255) {
                                // Optimización: Si es totalmente opaco, escribimos directo
                                framebuffer[pixelIndex] = (0xFF << 24) | (sr << 16) | (sg << 8) | sb;
                            } else if (alpha > 0) {
                                // Mezcla Alfa (Alpha Blending) con el color que ya estaba en el fondo
                                uint32_t dstColor = framebuffer[pixelIndex];
                                uint8_t dr = (dstColor >> 16) & 0xFF;
                                uint8_t dg = (dstColor >> 8) & 0xFF;
                                uint8_t db = dstColor & 0xFF;

                                // Fórmula lineal: Out = (Src * alpha + Dst * (255 - alpha)) / 255
                                uint8_t r = (sr * alpha + dr * (255 - alpha)) / 255;
                                uint8_t g = (sg * alpha + dg * (255 - alpha)) / 255;
                                uint8_t b = (sb * alpha + db * (255 - alpha)) / 255;

                                framebuffer[pixelIndex] = (0xFF << 24) | (r << 16) | (g << 8) | b;
                            }
                            // Si alpha == 0, no dibujamos nada (completamente transparente)
                        }
                    }
                }
            }
        }
    }
}

// Dibujar texto completo pasando el parámetro alpha
void DrawText(int startX, int startY, const char* text, uint32_t color, uint8_t alpha, int scale) {
    int length = (int)strlen(text);
    int charWidth = 6 * scale; // 5 píxeles de ancho + 1 de separación

    for (int i = 0; i < length; i++) {
        DrawChar(startX + (i * charWidth), startY, text[i], color, alpha, scale);
    }
}