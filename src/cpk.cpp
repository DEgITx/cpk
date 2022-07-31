#include <stdio.h>
#include <string.h>
#include <string>
#include <algorithm>
#include <json.hpp>

#include "cpk_structs.h"
#include "download.h"
#include "archive.h"
#include "global.h"
#include "thread_pool.h"
#include "degxlog.h"
#include "os.h"
#include "tools.h"

namespace cpk
{

std::map <std::string, int> CpkBuildTypes = {
    {"cmake", CMAKE},
};

std::map <std::string, int> CpkLanguages = {
    {"cpp", CPP},
    {"c", C},
    {"rust", RUST},
    {"python", PYTHON},
    {"javascript", JAVASCRIPT},
};

void InstallPackages(const std::vector<CPKPackage>& packages)
{
    if(packages.size() == 0)
        return;

    InstallBuildTools();

    nlohmann::json json;
    json["packages"] = {};
    for (const auto& package : packages) {
        json["packages"][package.package] = (package.version.length() > 0) ? package.version : "";
    }
    std::string jsonRequest = json.dump(4);

    std::string response = SendPostRequest(REMOTE_BACKEND_URL "/install", jsonRequest.c_str());
    if (response.length() == 0) {
        DX_ERROR("json", "no respoce from server");
        return;
    }
    DX_DEBUG("install", "responce: %s", response.c_str());

    nlohmann::json response_json;
    try {
        response_json = nlohmann::json::parse(response);
    } catch(...) {
        DX_ERROR("json", "error parse or respoce from server");
        return;
    }
    if (response_json.contains("error"))
    {
        bool error = response_json["error"];
        switch ((int)response_json["errorCode"]) {
            default:
                std::string errorDesc = response_json["errorDesc"];
                DX_ERROR("install", "error install: %s", errorDesc.c_str());
        }
        return;
    }

    std::string cpkDir = ".cpk";
    if (!IsDir(cpkDir))
        MkDir(cpkDir);

    thread_pool pool;
    const int processor_count = std::thread::hardware_concurrency();
    pool.start(processor_count);
    for(const auto& package : response_json["packages"])
    {
        pool.queue([package, cpkDir, processor_count](){
            std::string package_name = package["package"];
            std::string package_url = package["url"];
            std::string build_type = package["buildType"];
            std::string package_language = package["language"];

            DX_DEBUG("pkg", "install package %s", package_name.c_str());
            DX_DEBUG("install", "download package %s", package_url.c_str());
            std::string zipFile = cpkDir + "/" + package_name + ".zip";
            DownloadFile(package_url.c_str(), zipFile.c_str());
            DX_DEBUG("install", "downloaded %s as %s", package_url.c_str(), zipFile.c_str());
            std::string packageDir = cpkDir + "/" + package_name;
            if (!IsExists(packageDir))
                MkDir(packageDir);
            UnZip(zipFile, packageDir);
            DX_DEBUG("install", "Prepared package, start building...");
            switch(CpkBuildTypes[build_type])
            {
                case CMAKE:
                    {
                        DX_DEBUG("install", "build package %s with cmake", package_name.c_str());
                        ChDir(packageDir);
                        std::string buildDir = packageDir + "/build";
                        MkDir(buildDir);
                        DX_DEBUG("install", "prepare cmake build");
                        EXE("cmake -B \"build\" -G \"MinGW Makefiles\" -DCMAKE_BUILD_TYPE=RelWithDebInfo");
                        DX_DEBUG("install", "build");
                        EXE("cmake --build \"build\" -j" + std::to_string(processor_count));
                    }
                    break;
                default:
                    DX_ERROR("install", "no build type assotiated founded");
            }
        });
    }
    pool.stop();
}

void PublishPacket()
{  
    std::string packageName = Cwd();
    if (packageName.length() == 0) {
        DX_ERROR("publish", "no package name");
        return;
    }
    std::replace( packageName.begin(), packageName.end(), '\\', '/');
    std::size_t foundLastPathSep = packageName.find_last_of("/\\");
    if (foundLastPathSep >= 0 && foundLastPathSep < packageName.length()) {
        packageName = packageName.substr(foundLastPathSep+1);
        if (packageName.length() == 0) {
            DX_ERROR("publish", "no package name");
            return;
        }
    } else {
        DX_ERROR("publish", "no package name");
        return;
    }

    nlohmann::json json;
    json["package"] = packageName;
    json["language"] = "cpp";
    if (IsExists("CMakeLists.txt")) {
        json["buildType"] = "cmake";
    }

    if (!json.contains("buildType"))
    {
        DX_ERROR("publish", "build type is not detected");
        return;
    }

    auto all_files = AllFiles();
    std::string tmpFile = GetTempDir() + "/temp.zip";
    DX_DEBUG("publish", "generation temp.zip");
    CreateZip(all_files, tmpFile);
    DX_DEBUG("publish", "generated temp.zip");

    FILE* in_file = fopen(tmpFile.c_str(), "rb");
    if (!in_file) {
        DX_ERROR("zip", "cant open created zip");
        fclose(in_file);
        Remove(tmpFile);
        return;
    }

    size_t in_size = FileSize(tmpFile);
    const char* archive_content = (char*)malloc(in_size);
    fread((void*)archive_content, in_size, 1, in_file);

    DX_DEBUG("publish", "send %d", in_size);

    std::string jsonRequest = json.dump(4);
    std::string response = SendPostZip(REMOTE_BACKEND_URL "/publish", jsonRequest, archive_content, in_size);
    DX_DEBUG("publish", "response %s", response.c_str());
    fclose(in_file);
    Remove(tmpFile);
    nlohmann::json response_json;
    try {
        response_json = nlohmann::json::parse(response);
    } catch(...) {
        DX_ERROR("json", "error parse or respoce from server");
        return;
    }
    if (response_json.contains("error"))
    {
        bool error = response_json["error"];
        switch ((int)response_json["errorCode"]) {
            default:
                std::string errorDesc = response_json["errorDesc"];
                DX_ERROR("install", "error install: %s", errorDesc.c_str());
        }
        return;
    }
}

void PackagesList()
{
    std::string response = SendPostRequest(REMOTE_BACKEND_URL "/packages", "{}");
    DX_DEBUG("packages", "response %s", response.c_str());
    nlohmann::json response_json;
    try {
        response_json = nlohmann::json::parse(response);
    } catch(...) {
        DX_ERROR("json", "error parse or respoce from server");
        return;
    }
    if (response_json.contains("error"))
    {
        bool error = response_json["error"];
        switch ((int)response_json["errorCode"]) {
            default:
                std::string errorDesc = response_json["errorDesc"];
                DX_ERROR("install", "error install: %s", errorDesc.c_str());
        }
        return;
    }

    printf("packages:\n");
    for(const auto& package : response_json["packages"])
    {
        std::string package_name = package["package"];
        std::string package_version = package["version"];
        printf(" %s: %s\n", package_name.c_str(), package_version.c_str());
    }
}


char* getCmdOption(char ** begin, char ** end, const std::string & option)
{
    char ** itr = std::find(begin, end, option);
    if (itr != end && ++itr != end)
    {
        return *itr;
    }
    return 0;
}

bool cmdOptionExists(char** begin, char** end, const std::string& option)
{
    return std::find(begin, end, option) != end;
}

void printHelp()
{
    printf("./cpk [command] - CPK package manage\n");
    printf("commands:\n");
    printf("  install package1 [package2[@version]] - install package1, package2 and other\n");
    printf("  publish - publish current package\n");
    printf("  packages - list avaiable packages\n");
}

int cpk_main(int argc, char *argv[]) {
    if(argc == 1 || cmdOptionExists(argv, argv+argc, "-h"))
    {
        printHelp();
        return 0;
    }

    DX_DEBUG("arch", "OS: %s %s", cpk::GetOSType().c_str(), cpk::GetOSArch().c_str());

    if(argc > 2) {
        if (strcmp(argv[1], "install") == 0) {
            std::vector<CPKPackage> packages;
            for (int i = 2; i < argc; i++)
            {
                CPKPackage package;
                auto packageVer = Split(argv[i], "@");
                package.package = packageVer[0];
                if (packageVer.size() == 2)
                    package.package = packageVer[1];
                packages.push_back(package);
            }
            InstallPackages(packages);
        }
    }
    if(argc == 2) {
        if (strcmp(argv[1], "publish") == 0) {
                PublishPacket();
        }
        if (strcmp(argv[1], "packages") == 0) {
                PackagesList();
        }
    }

    //downloadFile("https://degitx.com/sitemap.xml", "sitemap.xml");
    return 0;
}

}