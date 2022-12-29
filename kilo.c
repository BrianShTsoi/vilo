#include <unistd.h>
#include <errno.h>
#include <stdio.h>

#include "rawmode.h"
#include "io.h"

int main() {
    enable_raw_mode();
    editor_init();

    while(1) {
        refresh_screen();
        process_key();
    }

    return 0;
}
