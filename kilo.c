#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>

struct termios orig_termios;

void disable_raw_mode();
void enable_raw_mode();
void die(const char* s);

int main() {
    enable_raw_mode();
    char c;

    while(1) {
        c = '\0';
        if (read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN) 
            die("read");
        if(c == 'q') break;
        printf("%d\r\n", c);
    }

    return 0;
}

void disable_raw_mode() {
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
        die("tcsetattr");
}

void enable_raw_mode() {
    if (tcgetattr(STDIN_FILENO,  &orig_termios) == -1)
        die("tcgetattr");
    atexit(disable_raw_mode);

    struct termios raw_termios = orig_termios;

    raw_termios.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw_termios.c_oflag &= ~(OPOST);
    raw_termios.c_cflag |= (CS8);
    raw_termios.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    raw_termios.c_cc[VMIN] = 0;
    raw_termios.c_cc[VTIME] = 1;

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw_termios) == -1)
        die("tcsetattr");
}

void die(const char* s) {
    perror(s);
    exit(1);
}