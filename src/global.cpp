#include <filesystem>
#include "global.h"
#include "degxlog.h"
#include <string>
#include <sys/stat.h>
#include <vector>
namespace cpk
{

size_t FileSize(const std::string& filename)
{
    struct stat stat_buf;
    int rc = stat(filename.c_str(), &stat_buf);
    return rc == 0 ? stat_buf.st_size : -1;
}

int MkDir(const std::string& path)
{
#if defined(_WIN32)
    return mkdir(path.c_str());
#else
    mode_t mode = 0755;
    return mkdir(path.c_str(), mode);
#endif
}

bool IsDir(const std::string& path)
{
    struct stat sb;
    if (stat(path.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode)) {
        return true;
    }
    return false;
}

bool IsExists(const std::string& path)
{
    struct stat   buffer;   
    return (stat (path.c_str(), &buffer) == 0);
}

void EXE(const std::string& command)
{
    system(command.c_str());
}

std::vector<std::string> AllFiles(const std::string& ext)
{
    // collect all the filenames into a std::list<std::string>
    std::vector<std::string> filenames;
    for (auto const& file : std::filesystem::recursive_directory_iterator("."))
    {
        if(std::filesystem::is_directory(file))
            continue;

        if(!ext.empty() && file.path().extension().string() != ext)
            continue;

        filenames.push_back(file.path().string());
    }
    return filenames;
}

void Remove(const std::string& file)
{
   int ret = remove(file.c_str());
   if(ret != 0) {
    DX_ERROR("file", "can't remove %s", file.c_str());
   }
}

}