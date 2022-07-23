#include "os.h"

#ifdef CPK_OS_WIN
#include <windows.h>
#else
#include <sys/utsname.h>
#endif
#include <stdio.h>
#include <string.h>

namespace cpk
{


std::unordered_map <std::string, int> os_arch_mapping = {
    {"x86_64", X86_64},
    {"x86", X86},
};
std::unordered_map <std::string, int> os_type_mapping = {
    {"linux", LINUX},
    {"macos", MACOS},
    {"windows", WINDOWS},
};


std::string GetOSArch()
{
#ifdef CPK_OS_WIN
    SYSTEM_INFO info;
    GetNativeSystemInfo(&info);
    switch (info.wProcessorArchitecture) {
        case PROCESSOR_ARCHITECTURE_AMD64:
            return "x86_64";
        case PROCESSOR_ARCHITECTURE_INTEL:
            return "x86";
        case PROCESSOR_ARCHITECTURE_IA64:
            return "ia64";
    };
#endif
#if defined(CPK_OS_LINUX) || defined(CPK_OS_MACOS)
    long ret = -1;
    struct utsname u;

    if (ret == -1)
        ret = uname(&u);
    if (ret != -1) {
        if (strlen(u.machine) == 4 && u.machine[0] == 'i'
                && u.machine[2] == '8' && u.machine[3] == '6')
            return std::string("x86");
        if (strcmp(u.machine, "amd64") == 0) // Solaris
            return std::string("x86_64");

        return std::string(u.machine);
    }
#endif
    return "";
}

std::string GetOSType()
{
#ifdef CPK_OS_WIN
    return "windows";
#endif
#ifdef CPK_OS_LINUX
    return "linux";
#endif
#ifdef CPK_OS_MACOS
    return "macos";
#endif
    return "";
}

}