#include "tools.h"
#include "os.h"
#include "download.h"
#include "archive.h"
#include "degxlog.h"
#include "global.h"
#include <stdlib.h>
namespace cpk 
{

std::string BuildToolsUrl()
{
    switch(os_type_mapping[GetOSType()])
    {
        case OS_TYPE::WINDOWS:
            switch(os_arch_mapping[GetOSArch()])
            {
                case X86_64:
                    return REMOTE_TOOLS_URL "/tools/winlibs-x86_64-posix-seh-gcc-12.1.0-llvm-14.0.4-mingw-w64ucrt-10.0.0-r2.zip";
                case X86:
                    return REMOTE_TOOLS_URL "/tools/winlibs-i686-posix-dwarf-gcc-12.1.0-llvm-14.0.4-mingw-w64ucrt-10.0.0-r2.zip";
            }
    }
}

std::string CmakeUrl()
{
    switch(os_type_mapping[GetOSType()])
    {
        case OS_TYPE::WINDOWS:
            switch(os_arch_mapping[GetOSArch()])
            {
                case X86_64:
                    return REMOTE_TOOLS_URL "/tools/cmake-3.23.2-windows-x86_64.zip";
                case X86:
                    return REMOTE_TOOLS_URL "/tools/cmake-3.23.2-windows-i386.zip";
            }
    }
}

void InstallBuildTools()
{
    switch(os_type_mapping[GetOSType()])
    {
        case OS_TYPE::WINDOWS:
        {
            std::string toolchainUrl = BuildToolsUrl();
            DX_DEBUG("tools", "download %s", toolchainUrl.c_str());
            std::string userPath = std::getenv("USERPROFILE");
            if (!IsDir(userPath + "/.cpk"))
            {
                DX_DEBUG("tools", "create dir %s", (userPath + "/.cpk").c_str());
                MkDir(userPath + "/.cpk");
            }
            if (!IsDir(userPath + "/.cpk/tools"))
            {
                DX_DEBUG("tools", "create dir %s", (userPath + "/.cpk/tools").c_str());
                MkDir(userPath + "/.cpk/tools");
            }
            std::string toolsPath = userPath + "/.cpk/tools";
            if (!IsExists((toolsPath + "/toolchain.zip").c_str()))
                DownloadFile(toolchainUrl.c_str(), (toolsPath + "/toolchain.zip").c_str());
            std::string mingw64Path = toolsPath + "/mingw64";
            std::string mingw32Path = toolsPath + "/mingw32";
            if (!IsDir(mingw64Path) && !IsDir(mingw32Path))
                UnZip(toolsPath + "/toolchain.zip", toolsPath);
            std::string addToPath;
            if (GetOSArch() == "x86_64") {
                addToPath = mingw64Path + "/bin";
            } else {
                addToPath = mingw32Path + "/bin";
            }

            std::string pathVar;
            pathVar = std::getenv("PATH");
            putenv((char*)("PATH=" + addToPath + ":" + pathVar).c_str());
            DX_DEBUG("tools", "set path: %s", std::getenv("PATH"));

            std::string cmakeUrl = CmakeUrl();
            std::string cmakeZip = (toolsPath + "/cmake.zip");
            if (!IsExists(cmakeZip.c_str()))
                DownloadFile(cmakeUrl.c_str(), cmakeZip.c_str());

            if (GetOSArch() == "x86_64") {
                if (!IsDir(toolsPath + "/cmake-3.23.2-windows-x86_64")) {
                    MkDir(toolsPath + "/cmake-3.23.2-windows-x86_64");
                    UnZip(cmakeZip.c_str(), toolsPath);
                }
                addToPath = toolsPath + "/cmake-3.23.2-windows-x86_64/bin";
            }
            if (GetOSArch() == "x86" && !IsDir(toolsPath + "/cmake-3.23.2-windows-i386")) {
                if (!IsDir(toolsPath + "/cmake-3.23.2-windows-i386")) {
                    MkDir(toolsPath + "/cmake-3.23.2-windows-i386");
                    UnZip(cmakeZip.c_str(), toolsPath);
                }
                addToPath = toolsPath + "/cmake-3.23.2-windows-i386/bin";
            }

            pathVar = std::getenv("PATH");
            putenv((char*)("PATH=" + addToPath + ":" + pathVar).c_str());
            DX_DEBUG("tools", "set path: %s", std::getenv("PATH"));
        }
        break;
    }
}

}