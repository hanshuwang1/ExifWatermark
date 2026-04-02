#include <openssl/md5.h>   
#include <openssl/evp.h>   
#include <curl/curl.h> 
#include <nlohmann/json.hpp> 
#include <iostream>
#include "BaiduReverseGeocode.hpp"

/**
  * @brief 对给定字符串进行url编码
  * @param str，待编码的字符串
  * @retval 编码后的字符串
  */
std::string urlencode(const std::string& str) {
    std::string strRes = "";
    size_t length = str.length();

    auto ToHex = [] (unsigned char x) {
        return  x > 9 ? x + 55 : x + 48;
    };

    for (size_t i = 0; i < length; i++) {
        if (isalnum((unsigned char)str[i]) ||
                (str[i] == '-') ||
                (str[i] == '_') ||
                (str[i] == '.') ||
                (str[i] == '~')) {
            strRes += str[i];
        } else if (str[i] == ' ') {
            strRes += "+";
        } else {
            strRes += '%';
            strRes += ToHex((unsigned char)str[i] >> 4);
            strRes += ToHex((unsigned char)str[i] % 16);
        }
    }
    return strRes;
}

/**
  * @brief 计算md5 EVP
  * @param str, 待加密的字符串
  * @retval 加密后的字符串
  */
std::string md5sum_ovp(const std::string& str) {
    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(ctx, EVP_md5(), nullptr);
    EVP_DigestUpdate(ctx, str.c_str(), str.size());

    unsigned char res[EVP_MAX_MD_SIZE] = {0};
    unsigned int res_len = 0;
    EVP_DigestFinal_ex(ctx, res, &res_len);
    EVP_MD_CTX_free(ctx);

    std::string res_str = "";
    char buf[4] = {0};
    for (unsigned int i = 0; i < res_len; i++) {
        memset(buf, 0x00, sizeof(buf));
        snprintf(buf, sizeof(buf), "%02x", res[i]);
        res_str += buf;
    }
    return res_str;
}
/**
  * @brief 计算md5
  * @param str，待加密的字符串
  * @retval 加密后的字符串
  */
// std::string md5sum(const std::string& str) {
//     MD5_CTX ctx;
//     MD5_Init(&ctx);
//     MD5_Update(&ctx, str.c_str(), str.size());

//     unsigned char res[16] = {0};
//     char buf[4] = {0};

//     MD5_Final(res, &ctx);
    
//     std::string res_str = "";
//     for (int i = 0; i < 16; i++) {
//         memset(buf, 0x00, sizeof(buf));
//         snprintf(buf, sizeof(buf), "%02x", res[i]);
//         res_str += buf;
//     }

//     return res_str;
// }

/**
  * @brief 根据请求字符串计算sn
  * @param url，编码后的请求字符串
  * @param sk，控制台中应用的sk
  * @retval 计算出的sn字符串
  */
std::string calculate_sn(const std::string& query_str, const std::string& sk) {
    std::string str = query_str + sk;
    return  md5sum_ovp(str);
}

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
int BaiduReverseGeocode_Offical(std::string ak, std::string sk, double latitude, double longitude, IMAGE_INFO_T& imageInfo) {

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
    
    // 计算sn
    std::string sn = calculate_sn(urlencode(query_str), sk);
    // 发送请求
    CURL *curl;
    CURLcode result_code;
    int error_code = 0;
    curl = curl_easy_init();
    if (curl) {
        std::string curl_result;
        std::string url = host + query_str + "&sn=" + sn;

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