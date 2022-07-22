
#include "os.h"
#include <string.h>

bool GetOSArch_test()
{
    if(GetOSArch().empty()) return false;
    return true;
}

int main()
{
    if(!GetOSArch_test()) return -1;
    return 0;
}