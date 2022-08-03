#pragma once
#include <string>
#include <vector>

namespace cpk
{

#ifndef CPK_RELEASE
#define REMOTE_BACKEND_URL "http://143.244.189.114:9988"
#define REMOTE_TOOLS_URL REMOTE_BACKEND_URL
#else
#define REMOTE_BACKEND_URL "http://127.0.0.1:9988"
#define REMOTE_TOOLS_URL "http://143.244.189.114:9988"
#endif

size_t FileSize(const std::string& filename);
int MkDir(const std::string& path);
int MkDirP(const std::string& path);
bool IsDir(const std::string& path);
bool IsExists(const std::string& path);
void EXE(const std::string& command);
std::vector<std::string> AllFiles(const std::string& ext = "");
void Remove(const std::string& file);
std::vector<std::string> Split (std::string s, std::string delimiter);
std::string Join(const std::vector<std::string> &lst, const std::string &delim);
std::string Cwd();
void ChDir(const std::string& path);

}