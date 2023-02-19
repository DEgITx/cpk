#include <filesystem>
#include "global.h"
#include "degxlog.h"
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>
#include <vector>
#include <algorithm>
#include <stdio.h>
#include <random>
#ifdef CPK_OS_MACOS
#include <pwd.h>
#endif

namespace cpk
{

bool m_gIsReleaseBackend = false;

std::string GetRemoteBackend()
{
    if (m_gIsReleaseBackend)
        return std::string(REMOTE_BACKEND_RELEASE_URL);
    else
        return std::string(REMOTE_BACKEND_URL);
}

void EnableRemoteBackend()
{
    DX_ERROR("dgx", "enabled remote backend");
    m_gIsReleaseBackend = true;
}

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

void EXES(const std::string& command)
{
    std::string silentCommand;
#ifdef CPK_OS_WIN
    silentCommand = command + " > nul";
#else
    silentCommand = command + " > /dev/null";
#endif
    system(silentCommand.c_str());
}

void EXEWithPrint(const std::string& command, std::function<void(const std::string& line)> callback)
{
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe)
    {
        DX_ERROR("exe", "cannot start command: %s", command.c_str());
        return;
    }
    char buffer[4096];
    while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
        callback(buffer);
    }
    pclose(pipe);
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

void ChDir(const std::string& path)
{
    chdir(path.c_str());
}

char* trim(char* str)
{
  char *end;

  // Trim leading space
  while(isspace((unsigned char)*str)) str++;

  if(*str == 0)  // All spaces?
    return str;

  // Trim trailing space
  end = str + strlen(str) - 1;
  while(end > str && isspace((unsigned char)*end)) end--;

  // Write new null terminator character
  end[1] = '\0';

  return str;
}

std::string ConsoleInput(const std::string& inputText, const std::string& defaultText)
{
   char str[4096];
   if (inputText.length() > 0) {
        if (defaultText.length() == 0)
            printf("%s: ", inputText.c_str());
        else
            printf("%s [%s]: ", inputText.c_str(), defaultText.c_str());
   }
   fgets( str, sizeof(str), stdin );
   std::string ret;
   if ((strlen(str) == 0 || strcmp(str, "\n") == 0 || strcmp(str, "\r\n") == 0) && defaultText.length() > 0)
        ret = defaultText;
   else
        ret = std::string(trim(str));
   DX_DEBUG("input", "%s = %s", inputText.c_str(), ret.c_str());
   return ret;
}

void WriteToFile(const std::string& file, const std::string& data)
{
   FILE *fp;
   fp = fopen(file.c_str(), "w");
   fprintf(fp, "%s", data.c_str());
   fclose(fp);
}

std::string ReadFileString(const std::string& file)
{
   std::string result = "";
   FILE *fp;
   fp = fopen(file.c_str(), "r");
   if (fp) {
        char buffer[1024];
        while (fgets(buffer, 1024, fp)) {
            result += buffer;
        }
   }
   fclose(fp);
   return result;
}

std::string GenerateRandomString(int length) {
  std::string alphabet = "abcdefghijklmnopqrstuvwxyz";
  std::string result;

  std::mt19937 generator(std::time(nullptr));
  std::uniform_int_distribution<int> distribution(0, 25);

  for (int i = 0; i < length; i++) {
    int randomIndex = distribution(generator);
    result += alphabet[randomIndex];
  }

  return result;
}

std::string CpkShareDir()
{
#ifdef CPK_OS_WIN
    return std::string(std::getenv("USERPROFILE")) + "/.cpk";
#endif
#ifdef CPK_OS_LINUX
    return std::string(std::getenv("HOME")) + "/.cpk";
#endif
#ifdef CPK_OS_MACOS
    struct passwd *pw = getpwuid(getuid());
    if (pw == NULL) {
        DX_ERROR("home", "Error: getpwuid failed");
        return std::string("");
    }
    std::string home(pw->pw_dir);
    std::string app_support = home + "/Library/Application Support";
    return app_support + "/.cpk";
#endif
}

}