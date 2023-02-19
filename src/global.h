#pragma once
#include <string>
#include <vector>
#include <functional>

namespace cpk
{

#ifdef CPK_RELEASE
#define REMOTE_BACKEND_URL "https://cpkpkg.com"
#define REMOTE_BACKEND_RELEASE_URL REMOTE_BACKEND_URL
#define REMOTE_TOOLS_URL REMOTE_BACKEND_URL
#else
#define REMOTE_BACKEND_URL "http://127.0.0.1:9988"
#define REMOTE_BACKEND_RELEASE_URL "https://cpkpkg.com"
#define REMOTE_TOOLS_URL "https://cpkpkg.com"
#endif

extern bool m_gIsReleaseBackend;
std::string GetRemoteBackend();
void EnableRemoteBackend();
size_t FileSize(const std::string& filename);
int MkDir(const std::string& path);
int MkDirP(const std::string& path);
bool IsDir(const std::string& path);
bool IsExists(const std::string& path);
void EXE(const std::string& command);
void EXEWithPrint(const std::string& command, std::function<void(const std::string& line)> callback);
std::vector<std::string> AllFiles(const std::string& ext = "");
void Remove(const std::string& file);
std::vector<std::string> Split (std::string s, std::string delimiter);
std::string Join(const std::vector<std::string> &lst, const std::string &delim);
std::string Cwd();
void ChDir(const std::string& path);
std::string ConsoleInput(const std::string& inputText, const std::string& defaultText = std::string());
void WriteToFile(const std::string& file, const std::string& data = std::string());
char* trim(char* str);
std::string ReadFileString(const std::string& file);
std::string GenerateRandomString(int length);
std::string CpkShareDir();

}