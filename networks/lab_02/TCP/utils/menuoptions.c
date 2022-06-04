#include "utils.h"

int server_menu_main(char *buffer)
{
    
    sprintf(buffer, "%s\n%s\n%s\n%s\n>>> ",
           "Which file do you want to recive?[type line number: 1, 2, 3]",
           FILE_1,
           FILE_2,
           FILE_3);
    
    return 0;
}

char *menu_chose_file(char c)
{
    char *str1 = "You choose ";
    switch (c) {
        case '1':
            printf("%s'%s'\n", str1, FILE_1);
            return FILE_1_PATH;
        case '2':
            printf("%s'%s'\n", str1, FILE_2);
            return FILE_2_PATH;
        case '3':
            printf("%s'%s'\n", str1, FILE_3);
            return FILE_3_PATH;
        default:
            printf("Wrong option\n");
            return NULL;
    }
}