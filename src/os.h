#pragma once
#include <string>
#include <map>

namespace cpk
{

enum OS_ARCH {
    X86_64 = 0,
    X86
};
extern std::map <std::string, int> os_arch_mapping;
enum OS_TYPE {
    LINUX = 0,
    MACOS,
    WINDOWS
};
extern std::map <std::string, int> os_type_mapping;

std::string GetOSArch();
std::string GetOSType();
std::string GetTempDir();

}