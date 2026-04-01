#include "headfile.hpp"
#include <openssl/md5.h>   
#include <openssl/evp.h>   
#include <curl/curl.h> 
#include <nlohmann/json.hpp> 

/**
  * @brief curl发送http请求调用的回调函数，回调函数中对返回的json格式的body进行了解析，解析结果储存在result中
  * @param 参数定义见libcurl库文档
  * @retval 返回值定义见libcurl库文档
  */
static size_t callback(void *ptr, size_t size, size_t nmemb, void *stream) {
    // 获取到的http body存放在ptr中，先将其转换为string格式
    std::string s((char *) ptr, size * nmemb);
    std::string* result = static_cast<std::string*>(stream);
    *result += s;
    return size * nmemb;
}
/**
  * @brief 验证经纬度
  * @param latitude
  * @param longtitude
  * @retval bool
  */
bool VerifyCoord(double latitude, double longitude) {
    if (latitude < -90.0 || latitude > 90.0) {
        std::cout << "纬度值无效，范围应在[-90, 90]之间。" << std::endl;
        return false;
    }
    if (longitude < -180.0 || longitude > 180.0) {
        std::cout << "经度值无效，范围应在[-180, 180]之间。" << std::endl;
        return false;
    }
    return true;
}

/**
  * @brief 百度示例
  * @param ak 
  * @param sk
  * @param latitude
  * @param longtitude
  * @retval 0 成功 -1 失败
  */
int BaiduReverseGeocode_Offical(std::string ak, double latitude, double longitude, IMAGE_INFO_T& imageInfo) {

    // 服务地址
    // libcurl默认是不支持SSL的, 如果需要支持https，请在libcurl的编译选项中设置
    std::string host = "http://api.map.baidu.com";

    // 接口地址
    std::string uri = "/reverse_geocoding/v3/";

    // 设置请求参数
    std::map<std::string, std::string> params;
        params["ak"]        = ak;
        params["output"]     = "json";
        params["coordtype"]     = "bd09ll";
        params["extensions_poi"]     = "1";
        params["location"]     = std::to_string(latitude) + "," + std::to_string(longitude);

    std::string query_str = "";
    for (auto& i : params) {
        query_str += "&" + i.first + "=" + i.second; 
    }
    query_str = uri + "?" + query_str.substr(1);

    // 发送请求
    CURL *curl;
    CURLcode result_code;
    int error_code = 0;
    curl = curl_easy_init();
    if (curl) {
        std::string curl_result;
        std::string url = host + query_str; 

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &curl_result);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);
        result_code = curl_easy_perform(curl);
        if (result_code != CURLE_OK) {
            std::cout << "curl_easy_perform() failed: " << curl_easy_strerror(result_code) << std::endl;
            return -1;
        }
        curl_easy_cleanup(curl);
        // deal with http response error code
        if (result_code != CURLE_OK) {
            std::cout << "curl_easy_perform() failed: " << curl_easy_strerror(result_code) << std::endl;
            return -1;
        }
        try
        {
            nlohmann::json json_result = nlohmann::json::parse(curl_result);
            imageInfo.locationInfo.province = json_result["result"]["addressComponent"]["province"];
            imageInfo.locationInfo.city = json_result["result"]["addressComponent"]["city"];
            imageInfo.locationInfo.district = json_result["result"]["addressComponent"]["district"];
            imageInfo.locationInfo.street = json_result["result"]["addressComponent"]["street"];
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
            std::cout << "JSON解析失败，返回结果为：" << curl_result << std::endl;
            return -1;
        }
    }
    return 0;
}