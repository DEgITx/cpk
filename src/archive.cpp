
#include <zip.h>
#include <string>

void unzip(const std::string& path, const std::string& outFile = "sitemap.xml")
{
    //Open the ZIP archive
    int err = 0;
    zip *z = zip_open(path.c_str(), 0, &err);

    //Search for the file of given name
    struct zip_stat st;
    zip_stat_init(&st);
    zip_stat(z, outFile.c_str(), 0, &st);

    //Alloc memory for its uncompressed contents
    char *contents = new char[st.size];

    //Read the compressed file
    zip_file *f = zip_fopen(z, outFile.c_str(), 0);
    zip_fread(f, contents, st.size);
    zip_fclose(f);

    //And close the archive
    zip_close(z);

    //save file
    FILE* saveFile = fopen (outFile.c_str(), "wb");
    fwrite (contents , sizeof(char), st.size, saveFile);
    fclose (saveFile);

    //delete allocated memory
    delete[] contents;
}
