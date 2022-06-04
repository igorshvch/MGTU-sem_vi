#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define DEFAULT_BUFF_UTILS 2048
#define FILE_1 "1. test1.txt"
#define FILE_2 "2. test2.txt"
#define FILE_3 "3. test3.txt"
#define FILE_1_PATH "test1.txt"
#define FILE_2_PATH "test2.txt"
#define FILE_3_PATH "test3.txt"

int print_log(char *log_path, char *sent_file);
int create_record(char *buffer, char *file_name);
int server_menu_main(char *buffer);
char *menu_chose_file(char c);
int readfile(char *buf, char *file_name);