#include <stdio.h>
#include <string.h>
#include <string>
#include <algorithm>

#include "cpk_structs.h"
#include "download.h"

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

void installPackages(const std::vector<CPKPackage>& packages)
{
    for(const auto& package : packages) 
    {
        downloadFile(package.url.c_str(), "file");
        switch(package.lang)
        {
            case CPP:
                printf("cpp\n");
                break;
            default:
                break;
        }
    }
}

void installPackagesArgs(const std::vector<std::string>& packages)
{
    for(const auto& package : packages) {
        printf("package %s\n", package.c_str());
    }
}

int main(int argc, char *argv[]) {
    if(argc > 2) {
        if (strcmp(argv[1], "install") == 0) {
            std::vector<std::string> packages;
            for (int i = 2; i < argc; i++)
            {
                packages.push_back(argv[i]);
            }
            installPackagesArgs(packages);
        }
    }

    downloadFile("https://degitx.com/sitemap.xml", "sitemap.xml");
    return 0;
}