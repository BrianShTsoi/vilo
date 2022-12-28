#include <unistd.h>
#include <errno.h>
#include <stdio.h>

#include "die.h"
#include "rawmode.h"
#include "io.h"

int main() {
    enable_raw_mode();
    editor_init();

    start_screen();
    while(1) {
        refresh_screen();
        process_key();
    }

    return 0;
}
