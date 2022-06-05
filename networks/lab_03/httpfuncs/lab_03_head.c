#include "httpfuncs.h"


int head_redirect(char *buff, unsigned long *len_ptrs)
{
    char *content_length = "";

    sprintf(content_length, "Conten-Length: %d\r\n", DEFAULT_HTTP_BUFF * sizeof(char));
    
    sprintf(buff, "%s%s%s%s%s%s",
            "HTTP/1.1 301\r\n",
            "Content-Type: text/html\r\n",
            content_length,
            "Location: /index.html\r\n",
            "Connection: keep-alive\r\n",
            "\r\n");
    *len_ptrs = (unsigned long)strlen(buff);
    return 0;
}

int head_index(char *buff, char *file_address, unsigned long *len_ptr)
{
    char html[DEFAULT_HTTP_BUFF];
    char *content_length = "";
    unsigned long len;

    FILE *fp = fopen(file_address, "r");
    fseek(fp, 0L, SEEK_END);
    len = ftell(fp);
    fclose(fp);

    sprintf(content_length, "Conten-Length: %d\r\n", strlen(html) * sizeof(char));
    
    sprintf(buff, "%s%s%s%s%s%s",
            "HTTP/1.1 200\r\n",
            "Content-Type: text/html\r\n",
            content_length,
            "Location: /index.html\r\n",
            "Connection: keep-alive\r\n",
            "\r\n");
    
    *len_ptr = (unsigned long)strlen(buff);
    return 0;
}

int head_image(char *buff, char *file_address, unsigned long *len_ptr)
{
    unsigned long len;
    char *content_length = "";

    FILE *fp = fopen(file_address, "rb");
    if (!fp) {
        printf("\t\t==>Could not open the file\n");
    }
    fseek(fp, 0L, SEEK_END);
    len = ftell(fp);
    fclose(fp);

    sprintf(content_length, "Conten-Length: %d\r\n", len);
    
    sprintf(buff, "%s%s%s%s%s%s",
            "HTTP/1.1 200\r\n",
            "Content-Type: image/jpeg\r\n",
            content_length,
            "Connection: keep-alive\r\n",
            "Accept-Ranges: bytes\r\n",
            "\r\n");

    unsigned long html_header_len = (unsigned long)strlen(buff);

    *len_ptr = html_header_len;

    return 0;
}

int head_ico(char *buff, char *file_address, unsigned long *len_ptr)
{
    unsigned long len;
    char *content_length = "";

    FILE *fp = fopen(file_address, "rb");
    if (!fp) {
        printf("\t\t==>Could not open the file\n");
    }
    fseek(fp, 0L, SEEK_END);
    len = ftell(fp);
    fclose(fp);

    sprintf(content_length, "Conten-Length: %d\r\n", len);
    
    sprintf(buff, "%s%s%s%s%s%s",
            "HTTP/1.1 200\r\n",
            "Content-Type: image/x-icon\r\n",
            content_length,
            "Connection: keep-alive\r\n",
            "Accept-Ranges: bytes\r\n",
            "\r\n");

    unsigned long html_header_len = (unsigned long)strlen(buff);

    *len_ptr = html_header_len;

    return 0;
}