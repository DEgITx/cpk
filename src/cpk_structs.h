#include <string>
#include <algorithm>
#include <vector>
#include <map>

namespace cpk
{

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

extern std::map <std::string, int> CpkBuildTypes;
extern std::map <std::string, int> CpkLanguages;

struct CPKPackage
{
    std::string package;
    std::string version;
    std::string url;
    size_t size;
    std::vector<CPKPackage> dependencies;
    CPKLang lang;
    CPKBuildType buildType;
};

}