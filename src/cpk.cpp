#include <stdio.h>
#include <string.h>
#include <string>
#include <algorithm>

#include "cpk_structs.h"
#include "download.h"
#include "archive.h"

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

void installPackages(const std::vector<CPKPackage>& packages, int level = 0)
{
    if(packages.size() == 0)
        return;

    for(const auto& package : packages) 
    {
        printf("add deps for install = %s\n", package.name.c_str());
        installPackageList.insert({package.name, (CPKPackage*)&package});
        installPackages(package.dependencies, level + 1);
    }

    // install
    if (level == 0) 
    {
        for (const auto& package : packages)
        {
            downloadFile(package.url.c_str(), "file");
            switch(package.lang)
            {
                case CPP:
                    printf("install %s\n", package.name.c_str());
                    UnZip("test_zip.zip", "sitemap.xml");
                    break;
                default:
                    break;
            }
        }
    }
}

int cpk_main(int argc, char *argv[]) {
    if(argc > 2) {
        if (strcmp(argv[1], "install") == 0) {
            std::vector<CPKPackage> packages;
            for (int i = 2; i < argc; i++)
            {
                CPKPackage package;
                package.name = argv[i];
                package.url = "https://degitx.com/sitemap.xml";
                package.lang = CPP;
                packages.push_back(package);
            }
            installPackages(packages);
        }
    }

    //downloadFile("https://degitx.com/sitemap.xml", "sitemap.xml");
    return 0;
}
