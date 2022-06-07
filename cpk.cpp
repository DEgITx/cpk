#include <stdio.h>
#include <curl/curl.h>
#include <string>
#include <algorithm>

size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
}

void downloadFile(char* url, char* output_file)
{
    CURL *curl;
    FILE *fp;
    CURLcode res;
    curl = curl_easy_init();
    if (curl) {
        printf("curl inited\n");
        fp = fopen(output_file, "wb");
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

        res = curl_easy_perform(curl);
        if(res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(res));
        else
            printf("downloaded %s\n", output_file);

        /* always cleanup */
        curl_easy_cleanup(curl);
        fclose(fp);
    }
}

char* getCmdOption(char ** begin, char ** end, const std::string & option)
{
    char ** itr = std::find(begin, end, option);
    if (itr != end && ++itr != end)
    {
        return *itr;
    }
    return 0;
}

bool cmdOptionExists(char** begin, char** end, const std::string& option)
{
    return std::find(begin, end, option) != end;
}

void installPackages(const std::vector<std::string>& packages)
{
    for(const auto& package : packages) {
        printf("package %s\n", package.c_str());
    }
}

int main(int argc, char *argv[]) {
    if(argc > 2) {
        if (strcmp(argv[1], "install") == 0) {
            std::vector<std::string> packages;
            for (int i = 2; i < argc; i++)
            {
                packages.push_back(argv[i]);
            }
            installPackages(packages);
        }
    }

    downloadFile("https://degitx.com/sitemap.xml", "sitemap.xml");
    return 0;
}