#include <stdio.h>
#include <stdlib.h>

#include "die.h"

void die(const char* s) {
    perror(s);
    exit(1);
}