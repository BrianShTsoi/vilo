#define _DEFAULT_SOURCE
#define _BSD_SOURCE
#define _GNU_SOURCE

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <unistd.h>
#include "buf.h"
#include "die.h"

#include "io.h"
 
#define KILO_VERSION "0.0.1"
#define CTRL_KEY(k) ((k) & 0x1f)

enum EditorMode {
    NORMAL = 0,
    INSERT
};

enum KEYS {
    BACKSPACE = 127
};

struct TextRow {
    int len;
    char* text;
};

struct EditorConfig {
    int cx, cy;
    int x_off, y_off;
    int screen_rows;
    int screen_cols;
    enum EditorMode mode;

    int file_rows;
    struct TextRow* rows;
};
struct EditorConfig E;

void editor_init() {
    E.cx = 0;
    E.cy = 0;
    E.x_off = 0;
    E.y_off = 0;
    get_win_size(&E.screen_rows, &E.screen_cols);
    E.mode = NORMAL;

    E.file_rows = 0;
    E.rows = NULL;
    buf_init();

    E.screen_rows--;
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

void editor_open(char* filename) {
    FILE* fp = fopen(filename, "r");
    if (!fp) die("fopen");

    char* line = NULL;
    size_t line_cap = 0;
    ssize_t linelen;
    while((linelen = getline(&line, &line_cap, fp)) != -1) {
        while (linelen > 0 && line[linelen - 1] == '\r' || 
                                line[linelen - 1] == '\n') {
            linelen--;
        }
        append_text_row(line, linelen);
    }

    free(line);
    fclose(fp);
}

void append_text_row(char* string, size_t linelen) {
    E.file_rows++;
    E.rows = realloc(E.rows, sizeof(struct TextRow) * E.file_rows);
    struct TextRow* new_row = &E.rows[E.file_rows - 1];
    new_row->len = linelen;

    new_row->text = malloc(linelen + 1);
    memcpy(new_row->text, string, linelen);
    new_row->text[linelen] = '\0';
}

char read_key() {
    char key;
    int read_return = 0;
    while ((read_return = read(STDIN_FILENO, &key, 1)) != 1)
        if (read_return == -1 && errno != EAGAIN) die("read");
    return key;
}

void process_key() {
    char key = read_key();

    if (E.mode == NORMAL) {
        normal_process_key(key);
    }
    else if (E.mode == INSERT) {
        insert_process_key(key);
    }
}

void normal_process_key(char key) {
    switch (key) {
        case 'i':
            E.mode = INSERT;
            break;
        case 'a':
            E.cx++;
            E.mode = INSERT;
            break;
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

void insert_process_key(char key) {
    switch (key) {
        case '\x1b':
            if (E.cx > 0) E.cx--;
            E.mode = NORMAL;
            break;
        case '\r':
            break;
        case BACKSPACE:
        case CTRL_KEY('h'):
            backspace_char();
            break;
        default:
            insert_char(key);
            break;
    }
}

void move_cursor(int key) {
    int current_file_row_num = E.y_off + E.cy;
    int current_row_len = E.file_rows ? E.rows[current_file_row_num].len : 0;
    switch (key) {
        case 'j':
            if (E.cy < E.screen_rows - 1 && E.cy < E.file_rows - 1)
                E.cy++;
            else if (E.file_rows - E.screen_rows > E.y_off) 
                E.y_off++;
            break;
        case 'k':
            if (E.cy > 0)
                E.cy--;
            else if (E.y_off > 0)
                E.y_off--;
            break;
        case 'h':
            if (E.cx > 0)
                E.cx--;
            else if (E.x_off > 0)
                E.x_off--;
            break;
        case 'l':
            if (E.cx < E.screen_cols - 1 && E.cx < current_row_len - 1) //E.cx max is current_row_len - 1 
                E.cx++;
            else if (current_row_len - E.screen_cols > E.x_off)
                E.x_off++;
            break;
    }
    current_file_row_num = E.y_off + E.cy;
    current_row_len = E.file_rows ? E.rows[current_file_row_num].len : 0;

    if (current_row_len == 0) {
        E.cx = 0;
    }
    else if (E.cx > current_row_len - 1) {
        E.cx = current_row_len - 1;
    }
}

void insert_char(int key) {
    if (E.file_rows != 0) {
        struct TextRow* current_row = &E.rows[E.cy + E.y_off];
        int index = E.cx + E.x_off;

        current_row->text = realloc(current_row->text, current_row->len + 2);
        memmove(&current_row->text[index + 1], 
                &current_row->text[index], 
                current_row->len - index + 1);
        current_row->text[index] = key;
        current_row->len++;
    
        E.cx++;
    } 
    else
        insert_newline();
}

void backspace_char() {
    struct TextRow* current_row = &E.rows[E.cy + E.y_off];
    int index = E.cx + E.x_off;
    if (index == 0) return;

    memmove(&current_row->text[index - 1], 
            &current_row->text[index], 
            current_row->len - index + 1);
    current_row->len--;

    E.cx--;

}

void insert_newline() {
    return;
}

void display_tildes(int screen_row_num) {
    if (screen_row_num == E.screen_rows / 3 && E.file_rows == 0) {
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
}

void display_single_file_row(int screen_row_num) {
    int len = E.rows[screen_row_num + E.y_off].len - E.x_off;

    if (len < 0) 
        len = 0;
    if (len > E.screen_cols) 
        len = E.screen_cols;

    buf_append(E.rows[screen_row_num + E.y_off].text + E.x_off, len);
    buf_append("\x1b[K", 3);
}

void display_status_bar() {
    buf_append("\x1b[7m", 4);
    for (int i = 0; i < E.screen_cols; i++) {
        buf_append(" ", 1);
    }
    buf_append("\x1b[m", 3);
}

void display_rows() {
    buf_append("\x1b[H", 3);
    for (int screen_row_num = 0; screen_row_num < E.screen_rows; screen_row_num++) {
        if (screen_row_num < E.file_rows) {
            display_single_file_row(screen_row_num);
        }
        else {
            display_tildes(screen_row_num);
        }
        buf_append("\r\n", 2);
    }
    display_status_bar();
}

void refresh_screen() {
    if (E.mode == NORMAL) {
        buf_append("\x1b[2 q", 5);
    }
    else if (E.mode == INSERT) {
        buf_append("\x1b[6 q", 5);
    }

    buf_append("\x1b[?25l", 6);
    display_rows();
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