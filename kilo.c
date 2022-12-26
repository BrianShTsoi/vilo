#include <unistd.h>
#include <errno.h>
#include <stdio.h>

#include "die.h"
#include "rawmode.h"
#include "input.h"

#define CTRL_KEY(k) ((k) & 0x1f)

int main() {
    enable_raw_mode();
    char c;

    while(1) {
        c = '\0';
        if (read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN) 
            die("read");
        printf("%d\r\n", c);

        if(c == CTRL_KEY('q')) break;
    }

    return 0;
}
