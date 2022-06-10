
#include <zip.h>
#include <string>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

void unzip(const std::string& path, const std::string& outFile = "sitemap.xml")
{
    //Open the ZIP archive
    int err = 0;
    zip *z = zip_open(path.c_str(), 0, &err);
    if (z == NULL) {
        printf("error openning archive\n");
        return;
    }
    printf("opened archive %s\n", path.c_str());

    int i, n = zip_get_num_entries(z, 0);
    for (i = 0; i < n; ++i) {
        const char* file_name = zip_get_name(z, i, 0);
        if (strlen(file_name) > 0 && file_name[strlen(file_name) - 1] == '/') {
            struct stat sb;
            if (stat(file_name, &sb) == 0 && S_ISDIR(sb.st_mode)) {
                continue;
            }
            printf("create dir %s\n", file_name);
            int result = mkdir(file_name);
            if (result == -1) {
                printf("cant create %s\n", file_name);
                break;
            }
            continue;
        }
        struct zip_stat st;
        zip_stat_init(&st);
        zip_stat(z, file_name, 0, &st);
        zip_file *f = zip_fopen(z, file_name, 0);
        char *buffer = new char[st.size];
        zip_fread(f, buffer, st.size);
        zip_fclose(f);
        FILE* saveFile = fopen (file_name, "wb");
        fwrite (buffer, sizeof(char), st.size, saveFile);
        fclose (saveFile);
        delete[] buffer;
        printf("unpacked %s\n", file_name);
    }
    //And close the archive
    zip_close(z);
}
