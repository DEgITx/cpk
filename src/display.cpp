#include "display.h"

namespace cpk
{

void RenderProgressBars(const std::vector<std::string>& names, const std::vector<int>& progresses)
{
    int barWidth = 70;

    for (int i = 0; i < progresses.size(); ++i)
    {
        float percent = (float)progresses[i] / 100.0;
        int pos = barWidth * percent;

        printf("%s [", names[i].c_str());
        for (int j = 0; j < barWidth; ++j)
        {
            if (j < pos) printf("=");
            else if (j == pos) printf(">");
            else printf(" ");
        }
        printf("] %d %\n\r", (int)progresses[i]);
    }
    
    printf("\033[%dA", progresses.size());
}

}