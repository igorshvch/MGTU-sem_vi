#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define DEFAULT_HTTP_BUFF 4096

int get_redirect(char *buff, unsigned long *len_ptr);
int get_index(char *buff, char *file_address, unsigned long *len_ptr);
int get_image(char *buff, char *file_address, unsigned long *len_ptr);

int head_redirect(char *buff, unsigned long *len_ptrs);
int head_index(char *buff, char *file_address, unsigned long *len_ptr);
int head_redirect(char *buff, unsigned long *len_ptrs);
int head_ico(char *buff, char *file_address, unsigned long *len_ptr);

int request_parser(char *request_string, char *method, char *file);