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
            std::string cpkShareDir = CpkShareDir();
            if (!IsDir(cpkShareDir))
            {
                DX_DEBUG("tools", "create dir %s", cpkShareDir.c_str());
                MkDir(cpkShareDir);
            }
            if (!IsDir(cpkShareDir + "/tools"))
            {
                DX_INFO("tools", "prepare build tools during first run");
                DX_DEBUG("tools", "create dir %s", (cpkShareDir + "/tools").c_str());
                MkDir(cpkShareDir + "/tools");
            }
            std::string toolsPath = cpkShareDir + "/tools";
            if (!IsExists((toolsPath + "/toolchain.zip").c_str()))
            {
                DX_INFO("tools", "downloading toolchain");
                DownloadFile(toolchainUrl.c_str(), (toolsPath + "/toolchain.zip").c_str());
            }
            std::string mingw64Path = toolsPath + "/mingw64";
            std::string mingw32Path = toolsPath + "/mingw32";
            if (!IsDir(mingw64Path) && !IsDir(mingw32Path))
            {
                DX_INFO("tools", "unpack toolchain... just wait...");
                UnZip(toolsPath + "/toolchain.zip", toolsPath);
                DX_INFO("tools", "done!");
            }
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
            {
                DX_INFO("tools", "downloading build tools...");
                DownloadFile(cmakeUrl.c_str(), cmakeZip.c_str());
            }

            if (GetOSArch() == "x86_64") {
                if (!IsDir(toolsPath + "/cmake-3.23.2-windows-x86_64")) {
                    DX_INFO("tools", "prepare build tools... just wait...");
                    MkDir(toolsPath + "/cmake-3.23.2-windows-x86_64");
                    UnZip(cmakeZip.c_str(), toolsPath);
                    DX_INFO("tools", "done!");
                }
                addToPath = toolsPath + "/cmake-3.23.2-windows-x86_64/bin";
            }
            if (GetOSArch() == "x86" && !IsDir(toolsPath + "/cmake-3.23.2-windows-i386")) {
                if (!IsDir(toolsPath + "/cmake-3.23.2-windows-i386")) {
                    DX_INFO("tools", "prepare build tools... just wait...");
                    MkDir(toolsPath + "/cmake-3.23.2-windows-i386");
                    UnZip(cmakeZip.c_str(), toolsPath);
                    DX_INFO("tools", "done!");
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