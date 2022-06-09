
#include <archive.h>
#include <string.h>

void readline(char* file, char* buff)
{
    FILE* fp = fopen(file, "r");
    fgets(buff, 255, (FILE*)fp);
    fclose(fp);
}

int main()
{
    unzip("test_zip.zip", "test_zip.txt");
    char buff[255];
    readline("test_zip.txt", buff);
    if (strcmp("hello test 1", buff) != 0)
        return 1;
    return 0;
}