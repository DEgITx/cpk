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

void InstallPackages(const std::vector<CPKPackage>& packages)
{
    if(packages.size() == 0)
        return;

    std::string response = SendPostRequest("http://127.0.0.1:9988/install", "{\"packages\": [\"example\"]}");
    DX_DEBUG("install", "responce: %s", response.c_str());

    nlohmann::json response_json = nlohmann::json::parse(response);

    thread_pool pool;
    pool.start(5);
    for(const auto& package : response_json["packages"])
    {
        pool.queue([package](){
            std::string package_name = package["package"];
            std::string package_url = package["url"];
            DX_DEBUG("install", "install package %s", package_name.c_str());
        });
    }
    pool.stop();

    // install
    // for (const auto& package : install_packages)
    // {
    //     DownloadFile(package.url.c_str(), "file");
    //     switch(package.lang)
    //     {
    //         case CPP:
    //             printf("install %s\n", package.name.c_str());
    //             UnZip("test_zip.zip", "sitemap.xml");
    //             break;
    //         default:
    //             break;
    //     }
    // }
}

void PublishPacket()
{
    FILE* in_file = fopen("cr_archive.zip", "rb");
    if (!in_file) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    size_t in_size = CPKGetFileSize("cr_archive.zip");
    const char* archive_content = (char*)malloc(in_size);
    fread((void*)archive_content, in_size, 1, in_file);

    std::string response = SendPostZip("http://127.0.0.1:9988/publish", "{\"package\": \"example\"}", archive_content, in_size);
    DX_DEBUG("publish", "response %s", response.c_str());
}

int cpk_main(int argc, char *argv[]) {
    if(argc > 2) {
        if (strcmp(argv[1], "install") == 0) {
            std::vector<CPKPackage> packages;
            for (int i = 2; i < argc; i++)
            {
                CPKPackage package;
                package.package = argv[i];
                package.url = "https://degitx.com/sitemap.xml";
                package.lang = CPP;
                packages.push_back(package);
            }
            InstallPackages(packages);
        }
    }
    if(argc == 2) {
        if (strcmp(argv[1], "publish") == 0) {
                PublishPacket();
        }
    }

    //downloadFile("https://degitx.com/sitemap.xml", "sitemap.xml");
    return 0;
}
