#include "httpfuncs.h"

/*
int main()
{
    printf("first\n");
    char buff1[DEFAULT_HTTP_BUFF];
    printf("second\n");
    char *buff2 = (char*)malloc(sizeof(char)*4194304);
    printf("third\n");
    char *file_address1 = " ";
    char *file_address2 = " ";
    //get_index(buff1, file_address1);
    //printf("%s\n", buff1);
    unsigned long len_ptr;
    printf("ready to start reading binary\n");
    get_image(buff2, file_address2, &len_ptr);
    printf("ready to print result\n");
    printf("content len: %d", len_ptr);
    free(buff2);
    return 0;   
}
*/


int get_redirect(char *buff, unsigned long *len_ptrs)
{
    char *content_length = "";

    sprintf(content_length, "Conten-Length: %d\r\n", DEFAULT_HTTP_BUFF * sizeof(char));
    
    sprintf(buff, "%s%s%s%s%s%s",
            "HTTP/1.1 301 Moved Permanently\r\n",
            "Content-Type: text/html\r\n",
            content_length,
            "Location: /index.html\r\n",
            "Connection: keep-alive\r\n",
            "\r\n");
    *len_ptrs = (unsigned long)strlen(buff);
    return 0;
}

int get_index(char *buff, char *file_address, unsigned long *len_ptr)
{
    char html[DEFAULT_HTTP_BUFF];
    char *content_length = "";

    FILE *fp = fopen(file_address, "r");
    fgets(html, DEFAULT_HTTP_BUFF, fp);
    fclose(fp);

    sprintf(content_length, "Conten-Length: %d\r\n", strlen(html) * sizeof(char));
    
    sprintf(buff, "%s%s%s%s%s%s%s",
            "HTTP/1.1 200 OK\r\n",
            "Content-Type: text/html\r\n",
            content_length,
            "Location: /index.html\r\n",
            "Connection: keep-alive\r\n",
            "\r\n",
            html);
    
    *len_ptr = (unsigned long)strlen(buff);
    return 0;
}

int get_image(char *buff, char *file_address, unsigned long *len_ptr)
{
    unsigned long len;
    char *content_length = "";

    FILE *fp = fopen(file_address, "rb");
    if (!fp) {
        printf("\t\t==>Could not open the file\n");
    }
    fseek(fp, 0L, SEEK_END);
    len = ftell(fp);
    rewind(fp);
    //fseek(fp, 0L, SEEK_SET);
    char *image_binary = (char *)malloc(sizeof(char)*len);
    fread(image_binary, len, 1, fp);
    fclose(fp);

    sprintf(content_length, "Conten-Length: %d\r\n", len);
    
    sprintf(buff, "%s%s%s%s%s%s",
            "HTTP/1.1 200 OK\r\n",
            "Content-Type: image/jpeg\r\n",
            content_length,
            "Connection: keep-alive\r\n",
            "Accept-Ranges: bytes\r\n",
            "\r\n");

    unsigned long html_header_len = strlen(buff);
    memcpy(buff+html_header_len, image_binary, sizeof(char)*len);

    *len_ptr = (unsigned long)(len+html_header_len);
    
    free(image_binary);

    return 0;
}

int get_ico(char *buff, char *file_address, unsigned long *len_ptr)
{
    unsigned long len;
    char *content_length = "";

    FILE *fp = fopen(file_address, "rb");
    if (!fp) {
        printf("\t\t==>Could not open the file\n");
    }
    fseek(fp, 0L, SEEK_END);
    len = ftell(fp);
    rewind(fp);
    char *image_binary = (char *)malloc(sizeof(char)*len);
    fread(image_binary, len, 1, fp);
    fclose(fp);

    sprintf(content_length, "Conten-Length: %d\r\n", len);
    
    sprintf(buff, "%s%s%s%s%s%s",
            "HTTP/1.1 200 OK\r\n",
            "Content-Type: image/x-icon\r\n",
            content_length,
            "Connection: keep-alive\r\n",
            "Accept-Ranges: bytes\r\n",
            "\r\n");

    unsigned long html_header_len = strlen(buff);
    memcpy(buff+html_header_len, image_binary, sizeof(char)*len);

    *len_ptr = (unsigned long)(len+html_header_len);
    
    free(image_binary);

    return 0;
}