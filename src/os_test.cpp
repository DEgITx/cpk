
#include "os.h"
#include <string.h>

using namespace cpk;

bool GetOSArch_test()
{
    if(GetOSArch().empty()) return false;
    return true;
}

bool GetOSType_test()
{
    if(GetOSType().empty()) return false;
    return true;
}

int main()
{
    if(!GetOSArch_test()) return -1;
    if(!GetOSType_test()) return -1;
    return 0;
}