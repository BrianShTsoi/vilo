#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "buf.h"

struct buf {
    char* string;
    int len;
};

struct buf B;

void buf_init() {
    B.string = NULL;
    B.len = 0;
}

void buf_append(const char* new_string, int len) {
    char* new = realloc(B.string, B.len + len);
    if (new == NULL) return;

    memcpy(&new[B.len], new_string, len);
    B.string = new;
    B.len += len;
}

void buf_write() {
    write(STDOUT_FILENO, B.string, B.len);
}

void buf_free() {
    free(B.string);
    buf_init();
}