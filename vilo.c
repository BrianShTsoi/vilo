#include <unistd.h>
#include <errno.h>
#include <stdio.h>

#include "rawmode.h"
#include "io.h"

int main(int argc, char *argv[]) {
    enable_raw_mode();
    editor_init();
    if (argc >= 2) editor_open(argv[1]);

    while(1) {
        refresh_screen();
        process_key();
    }

    return 0;
}
