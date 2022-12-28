#include <errno.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "buf.h"
#include "die.h"

#include "io.h"
 
#define CTRL_KEY(k) ((k) & 0x1f)

struct editor_config {
    int screen_rows;
    int screen_cols;
};
struct editor_config E;

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
        case CTRL_KEY('q'):
            write(STDOUT_FILENO, "\x1b[2J", 4);
            write(STDOUT_FILENO, "\x1b[H", 3);
            exit(0);
            break;
        default:
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

void print_tildes() {
    for (int i = 0; i < E.screen_rows - 1; i++) {
        buf_append("~\r\n", 3);
    }
    buf_append("~", 1);
    buf_append("\x1b[H", 3);
}

void editor_init() {
    get_win_size(&E.screen_rows, &E.screen_cols);
    buf_init();
}

void start_screen() {
    buf_append("\x1b[2J", 4);
    buf_append("\x1b[H", 3);
    print_tildes();
}

void refresh_screen() {
    buf_write();
    buf_free();
}