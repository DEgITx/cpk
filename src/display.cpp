#include "display.h"
#include "degxlog.h"

namespace cpk
{

void InitRenderProgressBars(int size)
{
    for (int i = 0; i < size; ++i)
    {
        printf("\n");
    }
    printf("\033[%dA", size);
}

uint64_t renderBarTime = 0;

void RenderProgressBars(const std::vector<std::string>& names, const std::vector<int>& progresses, bool force)
{
    int barWidth = 70;

    if (DXGetMiliTime() - renderBarTime < 40 && !force) {
        return;
    }
    renderBarTime = DXGetMiliTime();
    printf("\033[%dA", progresses.size());

    for (int i = 0; i < progresses.size(); ++i)
    {
        if (names[i].empty())
            continue;

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
}

}