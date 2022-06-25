#pragma once

void DownloadFile(const char* url, const char* output_file);
std::string SendPostRequest(const char* url, const std::string& jsonString);