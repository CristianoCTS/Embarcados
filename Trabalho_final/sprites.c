#include <stdint.h>

#define WIDTH          320
#define HEIGHT         240
#define PIXELS_PER_ROW 512

#define PIXBUF 0xC8000000

#define COLOR_BLACK        0x0000
#define COLOR_WHITE        0xFFFF
#define COLOR_BG           0x07E0
#define COLOR_BLUE_SHIRT   0x001F
#define COLOR_GREEN_SHIRT  0x05E0
#define COLOR_RED          0xF800
#define COLOR_SKIN1        0xCC6B
#define COLOR_SKIN2        0xFDB7
#define COLOR_BROWN_DARK   0x8A00
#define COLOR_BROWN_LIGHT  0x8A20
#define COLOR_LBLUE        0x7D7F

volatile uint16_t * const vga = (uint16_t *)PIXBUF;

#define SPR_W 11
#define SPR_H 26

static const uint8_t sprite1[SPR_H][SPR_W] = {
    {0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0},
    {0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0},
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
    {0, 2, 1, 1, 1, 1, 1, 1, 1, 2, 0},
    {0, 0, 2, 1, 1, 1, 1, 1, 2, 0, 0},
    {0, 0, 2, 2, 2, 2, 2, 2, 2, 0, 0},
    {0, 0, 0, 0, 2, 2, 2, 0, 0, 0, 0},
    {0, 3, 3, 3, 1, 1, 1, 3, 3, 3, 0},
    {3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3},
    {3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3},
    {3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3},
    {2, 2, 3, 3, 3, 3, 3, 3, 3, 2, 2},
    {2, 2, 2, 3, 3, 3, 3, 3, 3, 2, 2},
    {0, 2, 2, 2, 3, 3, 3, 3, 3, 2, 2},
    {0, 0, 3, 3, 3, 3, 3, 3, 3, 2, 0},
    {0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0},
    {0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0},
    {0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0},
    {0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0},
    {0, 0, 2, 2, 2, 0, 2, 2, 2, 0, 0},
    {0, 0, 4, 4, 4, 0, 4, 4, 4, 0, 0},
    {0, 0, 2, 2, 2, 0, 1, 4, 1, 0, 0},
    {0, 0, 2, 2, 2, 0, 1, 1, 1, 0, 0},
    {0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0},
};

static const uint8_t sprite2[SPR_H][SPR_W] = {
    {0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0},
    {0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0},
    {0, 1, 1, 6, 6, 6, 6, 6, 1, 1, 0},
    {0, 1, 6, 6, 6, 6, 6, 6, 6, 1, 0},
    {0, 6, 6, 6, 8, 6, 8, 6, 6, 6, 0},
    {0, 6, 6, 6, 6, 6, 6, 6, 6, 6, 0},
    {0, 0, 6, 6, 6, 7, 6, 6, 6, 0, 0},
    {0, 0, 6, 6, 6, 7, 6, 6, 6, 0, 0},
    {0, 0, 0, 0, 6, 6, 6, 0, 0, 0, 0},
    {0, 7, 7, 7, 5, 5, 5, 7, 7, 7, 0},
    {7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7},
    {7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7},
    {5, 5, 7, 7, 7, 7, 7, 7, 7, 5, 5},
    {6, 6, 7, 7, 7, 7, 7, 7, 7, 6, 6},
    {6, 6, 7, 7, 7, 7, 7, 7, 7, 6, 6},
    {6, 6, 7, 7, 7, 7, 7, 7, 7, 6, 6},
    {6, 6, 7, 7, 7, 7, 7, 7, 7, 6, 6},
    {0, 6, 5, 5, 5, 5, 5, 5, 5, 6, 0},
    {0, 0, 5, 5, 5, 5, 5, 5, 5, 0, 0},
    {0, 0, 5, 5, 5, 5, 5, 5, 5, 0, 0},
    {0, 0, 5, 5, 5, 0, 5, 5, 5, 0, 0},
    {0, 0, 6, 6, 6, 0, 6, 6, 6, 0, 0},
    {0, 0, 6, 6, 6, 0, 6, 6, 6, 0, 0},
    {0, 0, 6, 6, 6, 0, 6, 6, 6, 0, 0},
    {0, 0, 6, 6, 6, 0, 6, 6, 6, 0, 0},
    {0, 0, 7, 7, 7, 0, 7, 7, 7, 0, 0},
};

static const uint8_t sprite3[SPR_H][SPR_W] = {
    {0, 0, 0, 8, 8, 8, 8, 8, 0, 0, 0},
    {0, 0, 8, 8, 8, 8, 8, 8, 8, 0, 0},
    {0, 8, 8, 6, 6, 6, 6, 6, 8, 8, 0},
    {0, 8, 6, 6, 6, 6, 6, 6, 6, 8, 0},
    {0, 6, 6, 6, 9, 6, 9, 6, 6, 6, 0},
    {0, 6, 6, 6, 6, 6, 6, 6, 6, 6, 0},
    {0, 0, 6, 6, 6, 7, 6, 6, 6, 0, 0},
    {0, 0, 6, 6, 6, 7, 6, 6, 6, 0, 0},
    {0, 0, 0, 0, 6, 6, 6, 0, 0, 0, 0},
    {0, 7, 7, 7, 5, 5, 5, 7, 7, 7, 0},
    {7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7},
    {7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7},
    {5, 5, 7, 7, 7, 7, 7, 7, 7, 5, 5},
    {6, 6, 7, 7, 7, 7, 7, 7, 7, 6, 6},
    {6, 6, 7, 7, 7, 7, 7, 7, 7, 6, 6},
    {6, 6, 7, 7, 7, 7, 7, 7, 7, 6, 6},
    {6, 6, 7, 7, 7, 7, 7, 7, 7, 6, 6},
    {0, 6, 5, 5, 5, 5, 5, 5, 5, 6, 0},
    {0, 0, 5, 5, 5, 5, 5, 5, 5, 0, 0},
    {0, 0, 5, 5, 5, 5, 5, 5, 5, 0, 0},
    {0, 0, 5, 5, 5, 0, 5, 5, 5, 0, 0},
    {0, 0, 6, 6, 6, 0, 6, 6, 6, 0, 0},
    {0, 0, 6, 6, 6, 0, 6, 6, 6, 0, 0},
    {0, 0, 6, 6, 6, 0, 6, 6, 6, 0, 0},
    {0, 0, 6, 6, 6, 0, 6, 6, 6, 0, 0},
    {0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0},
};

void plot_pixel(int x, int y, uint16_t color) {
    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) return;
    int index = y * PIXELS_PER_ROW + x;
    vga[index] = color;
}

void clear_screen(uint16_t color) {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            plot_pixel(x, y, color);
        }
    }
}

void draw_sprite1(int x0, int y0) {
    for (int y = 0; y < SPR_H; y++) {
        for (int x = 0; x < SPR_W; x++) {
            uint8_t v = sprite1[y][x];
            if (v == 0) continue;
            uint16_t color;
            if      (v == 1) color = COLOR_BLACK;
            else if (v == 2) color = COLOR_SKIN1;
            else if (v == 3) color = COLOR_BLUE_SHIRT;
            else             color = COLOR_BROWN_DARK;
            plot_pixel(x0 + x, y0 + y, color);
        }
    }
}

void draw_sprite2(int x0, int y0) {
    for (int y = 0; y < SPR_H; y++) {
        for (int x = 0; x < SPR_W; x++) {
            uint8_t v = sprite2[y][x];
            if (v == 0) continue;
            uint16_t color;
            switch (v) {
                case 1: color = COLOR_BLACK;       break;
                case 5: color = COLOR_WHITE;       break;
                case 6: color = COLOR_BROWN_LIGHT; break;
                case 7: color = COLOR_RED;         break;
                case 8: color = COLOR_LBLUE;       break;
                default: return;
            }
            plot_pixel(x0 + x, y0 + y, color);
        }
    }
}

void draw_sprite3(int x0, int y0) {
    for (int y = 0; y < SPR_H; y++) {
        for (int x = 0; x < SPR_W; x++) {
            uint8_t v = sprite3[y][x];
            if (v == 0) continue;
            uint16_t color;
            switch (v) {
                case 1: color = COLOR_BLACK;       break;
                case 5: color = COLOR_WHITE;       break;
                case 6: color = COLOR_SKIN2;       break;
                case 7: color = COLOR_RED;         break;
                case 8: color = COLOR_BROWN_DARK;  break;
                case 9: color = COLOR_GREEN_SHIRT; break;
                default: return;
            }
            plot_pixel(x0 + x, y0 + y, color);
        }
    }
}

int main(void) {
    clear_screen(COLOR_BG);

    int y = (HEIGHT - SPR_H) / 2;
    int x1 = WIDTH/2 - SPR_W - SPR_W;
    int x2 = WIDTH/2 - SPR_W/2;
    int x3 = WIDTH/2 + SPR_W;

    draw_sprite1(x1, y);
    draw_sprite2(x2, y);
    draw_sprite3(x3, y);

    while (1) {
    }
    return 0;
}