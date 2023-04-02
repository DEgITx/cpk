#include "nn.h"
#include "download.h"
#include "global.h"
#include "degxlog.h"
#include <json.hpp>

namespace cpk
{

std::string AISearch(const std::string& description)
{
    std::string response = SendPostRequest(GetRemoteBackend() + "/nn", "{}");
    DX_DEBUG("packages", "response %s", response.c_str());
    nlohmann::json response_json;
    try {
        response_json = nlohmann::json::parse(response);
    } catch(...) {
        DX_ERROR("json", "error parse or response from server");
        return;
    }
    if (response_json.contains("error"))
    {
        bool error = response_json["error"];
        switch ((int)response_json["errorCode"]) {
            default:
                std::string errorDesc = response_json["errorDesc"];
                DX_ERROR("install", "error install: %s", errorDesc.c_str());
        }
        return;
    }
    std::string packages = std::string(response_json["search"]);
    printf("suggested packages: %s\n", packages.c_str());
}

}