#include <stdint.h>

#define VGA_BASE 0xC8000000
#define VGA_VISIBLE_COLS 320
#define VGA_PHYSICAL_COLS 512
#define VGA_ROWS 240

volatile uint16_t *ppix = (uint16_t *)VGA_BASE;

#define GREEN_D 0x03E0
#define GREEN_L 0x07C0

#define STRIPE_H 16

void draw_row_color(int y, uint16_t color)
{
    int x;
    uint32_t base = y * VGA_PHYSICAL_COLS;

    for (x = 0; x < VGA_VISIBLE_COLS; x++)
        ppix[base + x] = color;
}

void copy_row(int dst, int src)
{
    int x;
    uint32_t bd = dst * VGA_PHYSICAL_COLS;
    uint32_t bs = src * VGA_PHYSICAL_COLS;

    for (x = 0; x < VGA_VISIBLE_COLS; x++)
        ppix[bd + x] = ppix[bs + x];
}

void delay()
{
    volatile int i;
    for (i = 0; i < 8000; i++);
}

int main()
{
    int y;
    int scroll_pos = 0;
    int stripe_color_toggle = 0;

    /*start*/
    for (y = 0; y < VGA_ROWS; y++) {
        int stripe_index = y / STRIPE_H;
        uint16_t color = (stripe_index & 1) ? GREEN_D : GREEN_L;
        draw_row_color(y, color);
    }

    while (1) {

        /*scroll*/
        for (y = VGA_ROWS - 1; y > 0; y--)
            copy_row(y, y - 1);

        scroll_pos++;
        if (scroll_pos >= STRIPE_H) {
            scroll_pos = 0;
            stripe_color_toggle ^= 1;
        }

        {
            uint16_t color = stripe_color_toggle ? GREEN_D : GREEN_L;
            draw_row_color(0, color);
        }

        delay();
    }

    return 0;
}
