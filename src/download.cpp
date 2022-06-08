
#include <curl/curl.h>
#include <string>

void print_curl_protocols()
{
    printf("curl version: %s\n", curl_version());
    const char *const *proto;
    curl_version_info_data *curlinfo = curl_version_info(CURLVERSION_NOW);
    for(proto = curlinfo->protocols; *proto; proto++)
    {
        printf("curl support protocol: %s\n", *proto);
    }
}

size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
}

void downloadFile(const char* url, const char* output_file)
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
