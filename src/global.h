#pragma once
#include <string>

namespace cpk
{

#ifdef CPK_RELEASE
#define REMOTE_BACKEND_URL "http://143.244.189.114:9988"
#define REMOTE_TOOLS_URL REMOTE_BACKEND_URL
#else
#define REMOTE_BACKEND_URL "http://127.0.0.1:9988"
#define REMOTE_TOOLS_URL "http://143.244.189.114:9988"
#endif

size_t CPKGetFileSize(std::string filename);

}