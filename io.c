#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "buf.h"
#include "die.h"

#include "io.h"
 
#define KILO_VERSION "0.0.1"
#define CTRL_KEY(k) ((k) & 0x1f)

struct editor_config {
    int cx, cy;
    int screen_rows;
    int screen_cols;
};
struct editor_config E;

int START = 1;

char read_key() {
    char key = '0';
    if (read(STDIN_FILENO, &key, 1) == -1 && errno != EAGAIN)
        die("read");
    
    return key;
}

void process_key() {
    char key = read_key();

    buf_append(&key, 1);
    switch (key) {
        case 'j':
        case 'k':
        case 'h':
        case 'l':
            move_cursor(key);
            break;
        case CTRL_KEY('q'):
            write(STDOUT_FILENO, "\x1b[2J", 4);
            write(STDOUT_FILENO, "\x1b[H", 3);
            exit(0);
            break;
        default:
            break;
    }
}

void move_cursor(int key) {
    switch (key) {
        case 'j':
            if (E.cy < E.screen_rows - 1)
                E.cy++;
            break;
        case 'k':
            if (E.cy > 0)
                E.cy--;
            break;
        case 'h':
            if (E.cx > 0)
                E.cx--;
            break;
        case 'l':
            if (E.cx < E.screen_cols - 1)
                E.cx++;
            break;
    }
}

void get_win_size(int* rows, int* cols) {
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0)
        die("ioctl");
    else {
        *rows = ws.ws_row;
        *cols = ws.ws_col;
    }
}

void editor_init() {
    E.cx = 0;
    E.cy = 0;
    get_win_size(&E.screen_rows, &E.screen_cols);
    buf_init();
}

void print_tildes() {
    for (int i = 0; i < E.screen_rows - 1; i++) {
        if (i == E.screen_rows / 3) {
            char welcome[80];
            int welcomelen = snprintf(welcome, sizeof(welcome),
                "Kilo editor -- version %s", KILO_VERSION);
            if (welcomelen > E.screen_cols) welcomelen = E.screen_cols;
            int padding = (E.screen_cols - welcomelen) / 2;
            if (padding) {
                buf_append("~", 1);
                padding--;
            }
            while (padding--) buf_append(" ", 1);
            buf_append(welcome, welcomelen);
        }
        else {
            buf_append("~", 1);
        }
        buf_append("\x1b[K", 3);
        buf_append("\r\n", 2);
    }
    buf_append("\x1b[K", 3);
    buf_append("~", 1);
}

void start_screen() {
    buf_append("\x1b[H", 3);
    print_tildes();
    buf_append("\x1b[H", 3);
}

void refresh_screen() {
    buf_append("\x1b[?25l", 6);
    if (START) {
        start_screen();
        START = 0;
    }
    position_cursor();
    buf_append("\x1b[?25h", 6);
    buf_write();
    buf_free();
}

void position_cursor() {
    char pos[32];
    snprintf(pos, sizeof(pos), "\x1b[%d;%dH", E.cy + 1, E.cx + 1);
    buf_append(pos, strlen(pos));
}