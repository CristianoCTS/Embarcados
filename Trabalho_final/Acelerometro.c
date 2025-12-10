#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdint.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>

#define HW_REGS_BASE 0xFF200000
#define HW_REGS_SPAN 0x00200000

#define PUSHBUTTONS_OFFSET 0x00000050 
#define HEX3_HEX0_OFFSET   0x00000020
#define HEX5_HEX4_OFFSET   0x00000030

#define VGA_BASE 0xC8000000 
#define VGA_SPAN 0x00080000

#define SCREEN_W 320
#define SCREEN_H 240

#define LINE_STRIDE 1024 

#define GREEN 0x07E0
#define BLUE  0x001F
#define BLACK 0x0000 

volatile uint32_t *key_ptr;
volatile uint32_t *hex3_hex0_ptr;
volatile uint32_t *hex5_hex4_ptr;
volatile uint16_t *vga_ptr;

int fd_i2c;

void read_accel(float *ax, float *ay) {
    uint8_t reg = 0x32;
    write(fd_i2c, &reg, 1);

    uint8_t d[6];
    read(fd_i2c, d, 6);

    int16_t x = (d[1] << 8) | d[0];
    int16_t y = (d[3] << 8) | d[2];

    *ax = x / 256.0f;
    *ay = y / 256.0f;

    if (*ax > -0.1 && *ax < 0.1) *ax = 0;
    if (*ay > -0.1 && *ay < 0.1) *ay = 0;
}

void setpix(int y, int x, uint16_t color) {
    if (x < 0 || x >= SCREEN_W) return;
    if (y < 0 || y >= SCREEN_H) return;

    int offset = (y * (LINE_STRIDE >> 1)) + x; 
    vga_ptr[offset] = color;
}

void draw_square(int y, int x, uint16_t color) {
    for (int i = 0; i < 20; i++)
        for (int j = 0; j < 20; j++)
            setpix(y + i, x + j, color);
}

void fill_screen(uint16_t color) {
    for (int y = 0; y < SCREEN_H; y++)
        for (int x = 0; x < SCREEN_W; x++)
            setpix(y, x, color);
}

void set_hex_zeros() {
    *hex3_hex0_ptr = 0x3F3F3F3F;
    *hex5_hex4_ptr = 0x00003F3F;
}

int main() {
    int fd;

    //MAP /dev/mem
    if ((fd = open("/dev/mem", O_RDWR | O_SYNC)) == -1) {
        printf("ERRO: Nao foi possivel abrir /dev/mem\n");
        return 1;
    }

    void *virtual_base = mmap(NULL, HW_REGS_SPAN, 
                              (PROT_READ | PROT_WRITE), MAP_SHARED, 
                              fd, HW_REGS_BASE);

    if (virtual_base == MAP_FAILED) {
        printf("ERRO: mmap HW_REGS\n");
        close(fd);
        return 1;
    }

    key_ptr       = (volatile uint32_t *)((char *)virtual_base + PUSHBUTTONS_OFFSET);
    hex3_hex0_ptr = (volatile uint32_t *)((char *)virtual_base + HEX3_HEX0_OFFSET);
    hex5_hex4_ptr = (volatile uint32_t *)((char *)virtual_base + HEX5_HEX4_OFFSET);

    //MAP VGA
    void *vga_base_addr = mmap(NULL, VGA_SPAN, 
                               (PROT_READ | PROT_WRITE), MAP_SHARED, 
                               fd, VGA_BASE);

    if (vga_base_addr == MAP_FAILED) {
        printf("ERRO: mmap VGA\n");
        close(fd);
        return 1;
    }
    vga_ptr = (volatile uint16_t *)vga_base_addr;

    //I2C
    fd_i2c = open("/dev/i2c-0", O_RDWR);
    if (fd_i2c < 0) { printf("Erro abrindo I2C\n"); return 1; }
    ioctl(fd_i2c, I2C_SLAVE, 0x53);

    uint8_t init1[2] = {0x2D, 0x08};
    uint8_t init2[2] = {0x31, 0x08};
    uint8_t init3[2] = {0x2C, 0x0A};

    write(fd_i2c, init1, 2);
    write(fd_i2c, init2, 2);
    write(fd_i2c, init3, 2);

    printf("Acelerometro inicializado.\n");

    //VIDEO INIT
    set_hex_zeros();
    fill_screen(GREEN);

    int sq_x = 150;
    int sq_y = 100;

    draw_square(sq_y, sq_x, BLUE);

    printf("Jogo iniciado. Movimente inclinando a placa.\n");

    //LOOP
    while (1) {

        float ax, ay;
        read_accel(&ax, &ay);

        int old_x = sq_x;
        int old_y = sq_y;

        float speed = 5.0f;

        sq_x += (int)(ax * speed);
        sq_y -= (int)(ay * speed);

        // limites
        if (sq_x < 0) sq_x = 0;
        if (sq_x > SCREEN_W - 20) sq_x = SCREEN_W - 20;
        if (sq_y < 0) sq_y = 0;
        if (sq_y > SCREEN_H - 20) sq_y = SCREEN_H - 20;

        int moved = (sq_x != old_x || sq_y != old_y);

        if (moved) {
            draw_square(old_y, old_x, GREEN);
            draw_square(sq_y, sq_x, BLUE);
        }

        usleep(20000);
    }

    close(fd_i2c);
    munmap(virtual_base, HW_REGS_SPAN);
    munmap(vga_base_addr, VGA_SPAN);
    close(fd);
    return 0;
}