
#include <archive.h>
#include <string.h>

void readline(char* file, char* buff)
{
    FILE* fp = fopen(file, "r");
    fgets(buff, 255, (FILE*)fp);
    fclose(fp);
}

bool simple_test()
{
    unzip("test_zip.zip", "test_zip.txt");
    char buff[255];
    readline("test_zip.txt", buff);
    if (strcmp("hello test 1", buff) != 0)
        return false;

    return true;
}

bool dir_test()
{
    unzip("test_zip2.zip", "test_zip.txt");
    char buff[255];
    readline("test_zip_1.txt", buff);
    if (strcmp("hello test 1", buff) != 0)
        return false;

    char buff2[255];
    readline("testdir1/2 3 аб/test_zip_2.txt", buff2);
    if (strcmp("hello test 2", buff2) != 0)
        return false;

    return true;
}

int main()
{
    if(!simple_test()) return -1;
    if(!dir_test()) return -1;
    return 0;
}