#include "global.h"
#include <string>
#include <sys/stat.h>

namespace cpk
{

size_t CPKGetFileSize(const std::string& filename)
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

}