
#include <curl/curl.h>
#include <string>
#include "degxlog.h"

void print_curl_protocols()
{
    DX_DEBUG("curl", "curl version: %s\n", curl_version());
    const char *const *proto;
    curl_version_info_data *curlinfo = curl_version_info(CURLVERSION_NOW);
    for(proto = curlinfo->protocols; *proto; proto++)
    {
        DX_DEBUG("curl", "curl support protocol: %s\n", *proto);
    }
}

size_t FileWriteData(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
}

void DownloadFile(const char* url, const char* output_file)
{
    CURL *curl;
    FILE *fp;
    CURLcode res;
    curl = curl_easy_init();
    if (curl) {
        DX_DEBUG("curl", "curl inited");
        fp = fopen(output_file, "wb");
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, FileWriteData);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

        res = curl_easy_perform(curl);
        if(res != CURLE_OK)
        {
            DX_ERROR("curl", "curl_easy_perform() failed: %s",
                    curl_easy_strerror(res));
        }
        else
        {
            DX_INFO("curl", "downloaded %s", output_file);
        }

        /* always cleanup */
        curl_easy_cleanup(curl);
        fclose(fp);
    }
}

size_t WriteStringData(char *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

std::string CreatePostRequest(const char* url, const char* contentType, const char* postData, size_t postDataLength = 0)
{
    CURLcode ret;
    CURL *curl;
    struct curl_slist *slist1;
    std::string readBuffer;

    slist1 = NULL;
    slist1 = curl_slist_append(slist1, contentType);

    curl = curl_easy_init();
    if (!curl) {
        return readBuffer;
    }
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData);
    if (postDataLength > 0)
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE_LARGE, postDataLength);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "curl/7.38.0");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist1);
    curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 50L);
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
    curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteStringData);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

    ret = curl_easy_perform(curl);

    curl_easy_cleanup(curl);
    curl_slist_free_all(slist1);
    return readBuffer;
}

std::string SendPostRequest(const char* url, const std::string& jsonString = "{\"username\":\"bob\",\"password\":\"12345\"}")
{
    return CreatePostRequest(url, "Content-Type: application/json", jsonString.c_str());
}

std::string SendPostZip(const char* url, const std::string& jsonString = "{\"username\":\"bob\",\"password\":\"12345\"}", const char* fileData = NULL, size_t fileSize = 0)
{
    auto ret = CreatePostRequest(url, "Content-Type: application/json", jsonString.c_str());
    auto ret2 = CreatePostRequest(url, "Content-Type: application/zip", fileData, fileSize);
    return ret2;
}