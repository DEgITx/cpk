#include <stdio.h>
#include <string.h>
#include <string>
#include <algorithm>
#include <json.hpp>
#include <vector>
#include <fstream>

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

    std::mutex wait_install_mutex;
    std::unordered_map<std::string, std::condition_variable> need_install_deps_conditions;
    std::unordered_map<std::string, bool> need_install_deps_map_ready;
    for(const auto& package : response_json["packages"])
    {
        pool.queue([package, cpkDir, processor_count, &wait_install_mutex, &need_install_deps_conditions, &need_install_deps_map_ready]() 
        {
            std::string package_name = package["package"];
            std::string package_url = package["url"];
            std::string build_type = package["buildType"];
            std::string package_language = package["language"];

            nlohmann::json installedFileSave;
            if (IsExists(cpkDir + "/packages.json"))
            {
                std::ifstream ifs(cpkDir + "/packages.json");
                installedFileSave = nlohmann::json::parse(ifs);
            }
            else
            {
                installedFileSave["packages"] = {};
            }

            if (installedFileSave["packages"].contains(package_name))
            {
                DX_INFO("install", "%s package installed. Skip.", package_name.c_str());
                return;
            }

            DX_DEBUG("pkg", "wait deps install for package %s", package_name.c_str());
            if (package.contains("dependencies"))
            {
                for(const auto& dep : package["dependencies"].items())
                {
                    DX_DEBUG("pkg", "dep %s", dep.key().c_str());
                    std::unique_lock lock_install(wait_install_mutex);
                    need_install_deps_conditions[dep.key()].wait(lock_install, [&need_install_deps_map_ready, &dep]{
                        return need_install_deps_map_ready[dep.key()];
                    });
                }
            }

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
                        std::string buildDir = packageDir + "/build";
                        MkDir(buildDir);
                        DX_DEBUG("install", "prepare cmake build");
#ifdef CPK_OS_WIN
                        std::string cmake_build_type = "-G \"MinGW Makefiles\"";
#else
                        std::string cmake_build_type = "";
#endif
                        EXE("cd \"" + packageDir + "\" && cmake -B \"build\" " + cmake_build_type + " -DCMAKE_BUILD_TYPE=RelWithDebInfo");
                        DX_DEBUG("install", "build");
                        EXE("cd \"" + packageDir + "\" && cmake --build \"build\" -j" + std::to_string(processor_count));
                    }
                    break;
                default:
                    DX_ERROR("install", "no build type assotiated founded");
            }
            wait_install_mutex.lock();

            need_install_deps_map_ready[package_name] = true;

            if (IsExists(cpkDir + "/packages.json"))
            {
                std::ifstream ifs(cpkDir + "/packages.json");
                installedFileSave = nlohmann::json::parse(ifs);
            }
            else
            {
                installedFileSave["packages"] = {};
            }
            installedFileSave["packages"][package_name] = package;
            std::ofstream installedFile(cpkDir + "/packages.json");
            installedFile << std::setw(4) << installedFileSave << std::endl;
            DX_DEBUG("install", "written package.json");
            SendPostRequest(REMOTE_BACKEND_URL "/installed", "{\"package\": \"" + package_name + "\", \"success\": true}");
            DX_DEBUG("install", "post statistic after installation");

            wait_install_mutex.unlock();
            need_install_deps_conditions[package_name].notify_all();
        });
    }
    pool.stop();
}

void PublishPacket()
{  
    std::string packageName;
    nlohmann::json cpkConfig;
    if (IsExists("cpk.json"))
    {
        DX_DEBUG("publish", "found cpk.json");
        std::ifstream ifs("cpk.json");
        cpkConfig = nlohmann::json::parse(ifs);
        packageName = cpkConfig["name"];
    }
    else
    {
        DX_DEBUG("publish", "cpk.json not founded");
        packageName = Cwd();
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
    }

    nlohmann::json json;
    json["package"] = packageName;
    if (!cpkConfig.is_null() && cpkConfig.contains("dependencies"))
    {
        json["dependencies"] = cpkConfig["dependencies"];
    }
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