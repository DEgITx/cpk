#include <filesystem>
#include "global.h"
#include "degxlog.h"
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>
#include <vector>
#include <algorithm>
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

std::vector<std::string> Split (std::string s, std::string delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::string token;
    std::vector<std::string> res;

    while ((pos_end = s.find (delimiter, pos_start)) != std::string::npos) {
        token = s.substr (pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back (token);
    }

    res.push_back (s.substr (pos_start));
    return res;
}

std::string Join(const std::vector<std::string> &lst, const std::string &delim)
{
    std::string ret;
    for(const auto &s : lst) {
        if(!ret.empty())
            ret += delim;
        ret += s;
    }
    return ret;
}

int MkDirP(const std::string& path)
{
    std::string normalPath = path;
    std::replace( normalPath.begin(), normalPath.end(), '\\', '/');

    char tmp[1024];
    char *p = NULL;
    size_t len;

    snprintf(tmp, sizeof(tmp),"%s", normalPath.c_str());
    len = strlen(tmp);
    if (tmp[len - 1] == '/')
        tmp[len - 1] = 0;
    for (p = tmp + 1; *p; p++)
        if (*p == '/') {
            *p = 0;
            MkDir(tmp);
            *p = '/';
        }
    MkDir(tmp);
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

std::string Cwd()
{
   char cwd[PATH_MAX];
   if (getcwd(cwd, sizeof(cwd)) != NULL) {
       return std::string(cwd);
   } else {
       perror("getcwd() error");
       return "";
   }
   return "";
}

}