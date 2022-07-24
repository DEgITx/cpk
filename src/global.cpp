#include "global.h"
#include <string>
#include <sys/stat.h>

namespace cpk
{

size_t CPKGetFileSize(std::string filename)
{
    struct stat stat_buf;
    int rc = stat(filename.c_str(), &stat_buf);
    return rc == 0 ? stat_buf.st_size : -1;
}

}