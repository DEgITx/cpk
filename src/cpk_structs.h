#include <string>
#include <algorithm>
#include <vector>
#include <unordered_map>

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
    size_t size;
    std::vector<CPKPackage> dependencies;
    CPKLang lang;
    CPKBuildType buildType;
};

std::unordered_map<std::string, CPKPackage*> installPackageList;
