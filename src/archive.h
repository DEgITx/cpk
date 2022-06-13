#pragma once

#include <string>
#include <vector>

void UnZip(const std::string& path, const std::string& outFile = "unzip_file");
void CreateZip(const std::vector<std::string>& files, const std::string out_path);