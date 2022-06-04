#include "utils.h"

int print_log(char *log_path, char *sent_file)
{    
    char res_str[DEFAULT_BUFF_UTILS];

    create_record(res_str, sent_file);
    
    FILE *file = fopen(log_path, "a+");
    fputs(res_str, file);
    fclose(file);

    printf(">>> transaction successfully recorded to log!");

    return 0;
}

int create_record(char *buffer, char *sent_file) {
    time_t rawtime;
    struct tm *timeinfo;


    time(&rawtime);
    timeinfo = localtime(&rawtime);

    sprintf(buffer, "%d-%02d-%02d_%02d:%02d:%02d >>> %s\n",
           timeinfo->tm_year + 1900,
           timeinfo->tm_mon + 1,
           timeinfo->tm_mday,
           timeinfo->tm_hour,
           timeinfo->tm_min,
           timeinfo->tm_sec, 
           sent_file);
    
    return 0;
}