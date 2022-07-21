#pragma once
#include <sys/stat.h>

#ifdef CPK_RELEASE
#define REMOTE_BACKEND_URL "http://143.244.189.114:9988"
#else
#define REMOTE_BACKEND_URL "http://127.0.0.1:9988"
#endif

size_t CPKGetFileSize(std::string filename)
{
    struct stat stat_buf;
    int rc = stat(filename.c_str(), &stat_buf);
    return rc == 0 ? stat_buf.st_size : -1;
}