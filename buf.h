#ifndef BUF_H
#define BUF_H

void buf_init();
void buf_append(const char* new_string, int len);
void buf_write();
void buf_free();

#endif
