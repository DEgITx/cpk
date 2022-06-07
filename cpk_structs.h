#include <string>
#include <algorithm>

enum CPKLang
{
    CPP = 1,
    C,
    RUST,
    PYTHON,
    JAVASCRIPT
};

enum CPKBuildType
{
    SIMPLE = 1,
    CMAKE
};

struct CPKPackage
{
    std::string name;
    std::string version;
    std::string url;
    std::vector<CPKPackage> dependencies;
    CPKLang lang;
    CPKBuildType buildType;
};

std::unordered_map<std::string, CPKPackage*> installPackageList;
