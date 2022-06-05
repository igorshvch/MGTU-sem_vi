#include "httpfuncs.h"

/*
int main()
{
    //char buff[DEFAULT_HTTP_BUFF];
    char *req1 = "GET /universe.jpg HTTP/1.1\r\nSomething else\r\n\r\nhtml body\0";
    char *req2 = "PUT /universe.jpg HTTP/1.1\r\nSomething else\r\n\r\nhtml body\0";
    char *req3 = "DELETE /universe.jpg HTTP/1.1\r\nSomething else\r\n\r\nhtml body\0";
    char *req4 = "GET /universe.png HTTP/1.1\r\nSomething else\r\n\r\nhtml body\0";
    char *req5 = "POST /universe.jpg HTTP/1.1\r\nSomething else\r\n\r\nhtml body\0";
    char method[10];
    char file[DEFAULT_HTTP_BUFF];
    int res = 0;
    //strcpy(buff, "Nothing\0");    
    
    //res = request_parser(req1, buff, method, file);
    //if (res == 0){
    //    printf("parser result:\n>method: %s\n>file: %s\n", method, file);
    //}
    
    strcpy(file, "(null)");
    printf("====>Iteration # %d\n", 1);
    res = request_parser(req1, method, file);
    if (res == 0){
        printf("parser result:\n>method: %s\n>file: %s\n", method, file);
    }

    strcpy(file, "(null)");
    printf("====>Iteration # %d\n", 2);
    res = request_parser(req2, method, file);
    if (res == 0){
        printf("parser result:\n>method: %s\n>file: %s\n", method, file);
    }

    strcpy(file, "(null)");
    printf("====>Iteration # %d\n", 3);
    res = request_parser(req3, method, file);
    if (res == 0){
        printf("parser result:\n>method: %s\n>file: %s\n", method, file);
    }

    strcpy(file, "(null)");
    printf("====>Iteration # %d\n", 4);
    res = request_parser(req4, method, file);
    if (res == 0){
        printf("parser result:\n>method: %s\n>file: %s\n", method, file);
    }

    strcpy(file, "(null)");
    printf("====>Iteration # %d\n", 5);
    res = request_parser(req5, method, file);
    if (res == 0){
        printf("parser result:\n>method: %s\n>file: %s\n", method, file);
    }
    
    return 0;
}
*/


int request_parser(char *request_string, char *method, char *file)
{
    char *local_method;
    char *local_file;

    local_method = strtok(request_string, " /");
    local_file = strtok(NULL, " ");

    if ((strcmp(local_method, "GET") != 0)
        && (strcmp(local_method, "DELETE") != 0)
        && (strcmp(local_method, "PUT") != 0)
        && (strcmp(local_method, "HEAD") != 0))
        return 1;

    strcpy(method, local_method);
    
    if (strstr(local_file, "jpg") != NULL
        || strstr(local_file, "jpeg") != NULL
        || strstr(local_file, "ico") != NULL
        || strstr(local_file, "html") != NULL) {
        strcpy(file, local_file);
    }
    
    return 0;
}