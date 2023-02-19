#pragma once
#include <string>
#include <vector>

namespace cpk
{

void InitRenderProgressBars(int size);
void RenderProgressBars(const std::vector<std::string>& names, const std::vector<int>& progresses, bool force = false, const std::vector<std::string>& messages = std::vector<std::string>());

}