#include "utils.h"

int readfile(char *buf, char *file_name)
{
    FILE *fp = fopen(file_name, "r");
    fgets(buf, (int)strlen(buf), fp);
    fclose(fp);

    return 0;
}