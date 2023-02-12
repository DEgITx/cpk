#pragma once

namespace cpk
{

void DownloadFile(const char* url, const char* output_file);
std::string SendPostRequest(const std::string& url, const std::string& jsonString);
std::string SendPostZip(const std::string& url, const std::string& jsonString = "{\"username\":\"bob\",\"password\":\"12345\"}", const char* fileData = NULL, size_t fileSize = 0);

}