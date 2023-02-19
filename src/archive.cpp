
#include <zip.h>
#include <string>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <vector>
#include "degxlog.h"
#include "global.h"
#include <algorithm>

namespace cpk
{

void UnZip(const std::string& path, const std::string& outFile = "")
{
    //Open the ZIP archive
    int err = 0;
    zip *z = zip_open(path.c_str(), 0, &err);
    if (z == NULL) {
        DX_ERROR("zip", "error openning archive\n");
        return;
    }
    DX_DEBUG("zip", "opened archive %s", path.c_str());

    int i, n = zip_get_num_entries(z, 0);
    for (i = 0; i < n; ++i) {
        const char* file_name = zip_get_name(z, i, 0);
        std::string out_file = (!outFile.empty() && IsDir(outFile)) ? (outFile + "/" + file_name) : file_name;
        std::string normalOutPath = out_file;
        std::replace( normalOutPath.begin(), normalOutPath.end(), '\\', '/');
#ifndef CPK_OS_WIN
        out_file = normalOutPath;
#endif
        std::size_t foundLastPathSep = normalOutPath.find_last_of("/\\");
        if (foundLastPathSep >= 0 && foundLastPathSep < normalOutPath.length()) {
            normalOutPath = normalOutPath.substr(0, foundLastPathSep);
            MkDirP(normalOutPath);
        }
        
        struct zip_stat st;
        zip_stat_init(&st);
        zip_stat(z, file_name, 0, &st);
        zip_file *f = zip_fopen(z, file_name, 0);
        char *buffer = new char[st.size];
        zip_fread(f, buffer, st.size);
        zip_fclose(f);
        FILE* saveFile = fopen (out_file.c_str(), "wb");
        fwrite (buffer, sizeof(char), st.size, saveFile);
        fclose (saveFile);
        delete[] buffer;
        DX_DEBUG("zip", "unpacked %s", file_name);
    }
    //And close the archive
    zip_close(z);
}

void CreateZip(const std::vector<std::string>& files, const std::string out_path) {
    if (IsExists(out_path))
        Remove(out_path);

    //Open the ZIP archive
    int err = 0;
    zip *z = zip_open(out_path.c_str(), ZIP_CREATE, &err);
    zip_source_t* zs;
    for(const auto& file : files)
    {
        zs = zip_source_file(z, file.c_str(), 0, FileSize(file.c_str()));
        zip_file_add(z, file.c_str(), zs, ZIP_FL_OVERWRITE | ZIP_FL_ENC_UTF_8);
    }
    zip_close(z);
}

}