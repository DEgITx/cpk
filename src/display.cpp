#include "display.h"
#include "degxlog.h"

namespace cpk
{

int max_render_bar_message_length = 0;

void InitRenderProgressBars(int size)
{
    max_render_bar_message_length = 0;
    for (int i = 0; i < size; ++i)
    {
        printf("\n");
    }
}

uint64_t renderBarTime = 0;

void RenderProgressBars(const std::vector<std::string>& names, const std::vector<int>& progresses, bool force, const std::vector<std::string>& messages)
{
    int barWidth = 60;

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
        if (messages.size() <= i || messages[i].empty())
        {
            printf("] %d %%\n\r", (int)progresses[i]);
        }
        else
        {
            if (messages[i].length() > max_render_bar_message_length)
                max_render_bar_message_length = messages[i].length();

            std::string fill_empty(max_render_bar_message_length - messages[i].length(), ' ');
            printf("] %d %% %s\n\r", (int)progresses[i], (messages[i] + fill_empty).c_str());
        }
    }
}

}