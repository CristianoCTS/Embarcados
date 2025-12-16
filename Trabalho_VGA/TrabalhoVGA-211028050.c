#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define IO_JTAG_UART (*(volatile unsigned int *)0xFF201000)
#define VGA_BASE 0xc8000000
uint16_t *ppix = (uint16_t *)(VGA_BASE);

#define VGA_VISIBLE_COLS 320
#define VGA_PHYSICAL_COLS 512
#define VGA_ROWS 240

#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define BLACK   0x0000
#define WHITE   0xFFFF
#define GRAY    0x8410
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define CYAN    0x07FF

static uint16_t current_color = WHITE;

char jtag_getchar_poll(void) {
    unsigned int word;
    while (1) {
        word = IO_JTAG_UART;
        if (word & (1 << 15)) { 
            char c = (char)(word & 0xFF);
            putchar(c);
            return c;
        }
    }
}


int jtag_readline(char *buf, int maxlen) {
    int i = 0;
    char c;
    while (1) {
        c = jtag_getchar_poll();
        if (c == '\r') { 
            continue;
        }
        if (c == '\n') {
            if (i < maxlen) buf[i] = '\0';
            else buf[maxlen-1] = '\0';
            putchar('\n'); 
            return i;
        }
        if (i < maxlen - 1) {
            buf[i++] = c;
        }
    }
}


int jtag_read_int(void) {
    char buf[32];
    int i = 0;
    char c;
    int started = 0;
    while (1) {
        c = jtag_getchar_poll();
        if (!started) {
            if (c == '-' || (c >= '0' && c <= '9')) {
                buf[i++] = c;
                started = 1;
            } else {
                if (c == '\n') { putchar('\n'); }
                continue;
            }
        } else {
            if ((c >= '0' && c <= '9') && i < (int)sizeof(buf)-1) {
                buf[i++] = c;
            } else {
                buf[i] = '\0';
                if (c == '\n') putchar('\n');
                return atoi(buf);
            }
        }
    }
}

void set_pix(int lin, int col) {
    if (lin < 0 || lin >= VGA_ROWS) return;
    if (col < 0 || col >= VGA_VISIBLE_COLS) return;
    uint32_t idx = (uint32_t)lin * VGA_PHYSICAL_COLS + (uint32_t)col;
    ppix[idx] = current_color;
}

void fill_screen(void) {
    for (int r = 0; r < VGA_ROWS; ++r) {
        for (int c = 0; c < VGA_VISIBLE_COLS; ++c) {
            uint32_t idx = (uint32_t)r * VGA_PHYSICAL_COLS + (uint32_t)c;
            ppix[idx] = current_color;
        }
    }
}

void line (int x0, int y0, int x1, int y1) {
    int dx = abs (x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs (y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2;

    for (;;) {
        set_pix (x0, y0);
        if (x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

void circle(int cx, int cy, int r) {
    if (r < 0) return;
    int x = 0;
    int y = r;
    int d = 3 - 2 * r;

    while (x <= y) {
        set_pix(cx + x, cy + y);
        set_pix(cx - x, cy + y);
        set_pix(cx + x, cy - y);
        set_pix(cx - x, cy - y);
        set_pix(cx + y, cy + x);
        set_pix(cx - y, cy + x);
        set_pix(cx + y, cy - x);
        set_pix(cx - y, cy - x);

        if (d <= 0) {
            d = d + 4 * x + 6;
        } else {
            d = d + 4 * (x - y) + 10;
            y--;
        }
        x++;
    }
}

void rect_outline(int lin0, int col0, int lin1, int col1) {
    int top = (lin0 < lin1) ? lin0 : lin1;
    int bottom = (lin0 < lin1) ? lin1 : lin0;
    int left = (col0 < col1) ? col0 : col1;
    int right = (col0 < col1) ? col1 : col0;

    line(top, left, top, right);
    line(bottom, left, bottom, right);
    line(top, left, bottom, left);
    line(top, right, bottom, right);
}

void rect_filled(int lin0, int col0, int lin1, int col1) {
    int top = (lin0 < lin1) ? lin0 : lin1;
    int bottom = (lin0 < lin1) ? lin1 : lin0;
    int left = (col0 < col1) ? col0 : col1;
    int right = (col0 < col1) ? col1 : col0;

    for (int r = top; r <= bottom; ++r) {
        for (int c = left; c <= right; ++c) {
            set_pix(r, c);
        }
    }
}

void choose_color(void) {
    printf("\nEscolha a cor (digite o número e Enter):\n");
    printf("0 Black\n1 Red\n2 Green\n3 Blue\n4 Gray\n5 White\n6 Magenta\n7 Yellow\n8 Cyan\n> ");
    int opt = jtag_read_int();
    switch (opt) {
        case 0: current_color = BLACK; break;
        case 1: current_color = RED; break;
        case 2: current_color = GREEN; break;
        case 3: current_color = BLUE; break;
        case 4: current_color = GRAY; break;
        case 5: current_color = WHITE; break;
        case 6: current_color = MAGENTA; break;
        case 7: current_color = YELLOW; break;
        case 8: current_color = CYAN; break;
        default: printf("\nOpção inválida. Mantendo cor atual.\n"); break;
    }
}

void print_menu(void) {
    printf("\n\n=== MENU GRÁFICOS CPUlator ===\n");
    printf("1 - COLOR (definir cor atual)\n");
    printf("2 - LINE  (desenhar linha)\n");
    printf("3 - CIRC  (desenhar círculo)\n");
    printf("4 - RECT  (retângulo vazado)\n");
    printf("5 - TILE  (retângulo preenchido)\n");
    printf("6 - FUNDO (preencher tela com cor atual)\n");
    printf("7 - CLEAR (limpar para branco)\n");
    printf("q - Sair\n");
    printf("Escolha opcao: ");
}

int main(void) {
    current_color = WHITE;
    uint16_t prev = current_color;
    current_color = BLACK;
    fill_screen();
    current_color = prev;

    printf("Trabalho: Graficos CPUlator II - Iniciado\n");

    while (1) {
        print_menu();
        char cmd = 0;
        while (1) {
            char c = jtag_getchar_poll();
            if (c == '\r') continue;
            if (c == '\n') continue;
            cmd = c;
            putchar('\n');
            break;
        }

        if (cmd == 'q' || cmd == 'Q') {
            printf("Saindo...\n");
            break;
        }

        switch (cmd) {
            case '1': /* COLOR */
                choose_color();
                break;

            case '2': { /* LINE */
                printf("\nDigite lin0 (linha inicial) e pressione Enter: ");
                int lin0 = jtag_read_int();
                printf("Digite col0 (col inicial) e pressione Enter: ");
                int col0 = jtag_read_int();
                printf("Digite lin1 (linha final) e pressione Enter: ");
                int lin1 = jtag_read_int();
                printf("Digite col1 (col final) e pressione Enter: ");
                int col1 = jtag_read_int();
                line(lin0, col0, lin1, col1);
                printf("\nLinha desenhada.\n");
                break;
            }

            case '3': { /* CIRC */
                printf("\nDigite lin_centro e pressione Enter: ");
                int lin = jtag_read_int();
                printf("Digite col_centro e pressione Enter: ");
                int col = jtag_read_int();
                printf("Digite raio (r) e pressione Enter: ");
                int r = jtag_read_int();
                circle(lin, col, r);
                printf("\nCirculo desenhado.\n");
                break;
            }

            case '4': { /* RECT */
                printf("\nDigite lin0 e pressione Enter: ");
                int lin0 = jtag_read_int();
                printf("Digite col0 e pressione Enter: ");
                int col0 = jtag_read_int();
                printf("Digite lin1 e pressione Enter: ");
                int lin1 = jtag_read_int();
                printf("Digite col1 e pressione Enter: ");
                int col1 = jtag_read_int();
                rect_outline(lin0, col0, lin1, col1);
                printf("\nRetangulo vazado desenhado.\n");
                break;
            }

            case '5': { /* TILE */
                printf("\nDigite lin0 e pressione Enter: ");
                int lin0 = jtag_read_int();
                printf("Digite col0 e pressione Enter: ");
                int col0 = jtag_read_int();
                printf("Digite lin1 e pressione Enter: ");
                int lin1 = jtag_read_int();
                printf("Digite col1 e pressione Enter: ");
                int col1 = jtag_read_int();
                rect_filled(lin0, col0, lin1, col1);
                printf("\nRetangulo preenchido desenhado.\n");
                break;
            }

            case '6': /* FUNDO */
                printf("\nPreenchendo tela com a cor atual...\n");
                fill_screen();
                printf("Tela preenchida.\n");
                break;

            case '7': /* CLEAR */
                printf("\nLimpando tela (Branco)...\n");
                {
                    uint16_t saved = current_color;
                    current_color = WHITE;
                    fill_screen();
                    current_color = saved;
                }
                printf("Tela limpa.\n");
                break;

            default:
                printf("\nOpcao desconhecida.\n");
                break;
        }
    }

    return 0;
}
