#include "tools.h"
#include "os.h"
#include "download.h"
#include "archive.h"
#include "degxlog.h"
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
                    return "https://github.com/brechtsanders/winlibs_mingw/releases/download/12.1.0-14.0.4-10.0.0-ucrt-r2/winlibs-x86_64-posix-seh-gcc-12.1.0-llvm-14.0.4-mingw-w64ucrt-10.0.0-r2.zip";
                case X86:
                    return "https://github.com/brechtsanders/winlibs_mingw/releases/download/12.1.0-14.0.4-10.0.0-ucrt-r2/winlibs-i686-posix-dwarf-gcc-12.1.0-llvm-14.0.4-mingw-w64ucrt-10.0.0-r2.zip";
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
            DownloadFile(toolchainUrl.c_str(), "c:/Users/DEg/.cpk/tools/toolchain.zip");
        }
        break;
    }
}

}