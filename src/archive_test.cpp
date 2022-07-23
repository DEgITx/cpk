
#include <archive.h>
#include <string.h>

using namespace cpk;

void readline(char* file, char* buff)
{
    FILE* fp = fopen(file, "r");
    fgets(buff, 255, (FILE*)fp);
    fclose(fp);
}

bool simple_test()
{
    UnZip("test_zip.zip", "test_zip.txt");
    char buff[255];
    readline("test_zip.txt", buff);
    if (strcmp("hello test 1", buff) != 0)
        return false;

    return true;
}

bool dir_test()
{
    UnZip("test_zip2.zip", "test_zip.txt");
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

bool create_test()
{
    CreateZip({"1.txt", "2.txt"}, "cr_archive.zip");
    return true;
}

bool create_subdir_test()
{
    CreateZip({"1.txt", "3/2.txt"}, "cr_archive.zip");
    return true;
}

int main()
{
    // if(!simple_test()) return -1;
    // if(!dir_test()) return -1;
    // if(!create_test()) return -1;
    // if(!create_subdir_test()) return -1;
    return 0;
}