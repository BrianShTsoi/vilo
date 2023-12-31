#ifndef IO_H
#define IO_H

void editor_init();
void get_win_size(int* rows, int* cols);
void editor_open(char* filename);
void append_text_row(char* string, size_t len);

char read_key();
void process_key();
void normal_process_key(char key);
void insert_process_key(char key);
void move_cursor(int key);
void insert_char(int key);
void backspace_char();
void insert_newline();

void display_tildes(int rownum);
void display_single_file_row(int rownum);
void display_status_bar();
void display_rows();
void refresh_screen();
void position_cursor();

#endif