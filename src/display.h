#pragma once
#include <string>
#include <vector>
#include <unordered_map>

namespace cpk
{

extern int max_render_bar_message_length;

void InitRenderProgressBars(int size);
void RenderProgressBars(
    const std::vector<std::string>& names,
    const std::vector<std::string>& versions,
    const std::vector<int>& progresses, 
    bool force = false, 
    const std::vector<std::string>& messages = std::vector<std::string>(),
    const std::unordered_map<std::string, bool>& success = std::unordered_map<std::string, bool>(),
    const std::unordered_map<std::string, bool>& failed = std::unordered_map<std::string, bool>()
);

}