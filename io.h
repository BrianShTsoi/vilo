#ifndef IO_H
#define IO_H

char read_key();
void process_key();
void move_cursor(int key);

void get_win_size(int* rows, int* cols);
void print_tildes();
void editor_init();
void start_screen();
void refresh_screen();
void position_cursor();

#endif