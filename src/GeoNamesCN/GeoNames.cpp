#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cmath>
#include <limits>
#include <iomanip>
#include <unordered_map>
#include "GeoNames.hpp"
#include "../Baidu/BaiduReverseGeocode.hpp"
#define M_PI 3.14159265358979323846

struct Location {
    double lat;
    double lon;
    std::string county_name;   // 县/区名称（ADM3 的原始 name）
    std::string country;
    std::string admin1_code;   // 省代码
    std::string admin2_code;   // 市代码
    std::string feature;
};;

class ReverseGeocoder {
private:
    std::vector<Location> locations;

    // Haversine 公式计算球面距离（单位：米）
    static double haversine(double lat1, double lon1, double lat2, double lon2) {
        constexpr double R = 6371000.0; // 地球半径（米）
        double dlat = (lat2 - lat1) * M_PI / 180.0;
        double dlon = (lon2 - lon1) * M_PI / 180.0;
        lat1 = lat1 * M_PI / 180.0;
        lat2 = lat2 * M_PI / 180.0;

        double a = std::sin(dlat / 2) * std::sin(dlat / 2) +
                   std::cos(lat1) * std::cos(lat2) *
                   std::sin(dlon / 2) * std::sin(dlon / 2);
        double c = 2 * std::atan2(std::sqrt(a), std::sqrt(1 - a));
        return R * c;
    }

public:
    std::unordered_map<std::string, std::string> admin1_names;
    std::unordered_map<std::string, std::string> admin2_names;  
    // 加载 GeoNames cities500.txt
    bool load(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) return false;

        locations.clear();
        admin1_names.clear();
        admin2_names.clear();

        std::string line;
        while (std::getline(file, line)) {
            if (line.empty() || line[0] == '#') continue;  // 跳过空行和注释

            std::vector<std::string> fields;
            std::istringstream iss(line);
            std::string field;
            while (std::getline(iss, field, '\t')) {
                fields.push_back(field);
            }
            if (fields.size() < 12) continue;  // 字段不够，跳过

            std::string fclass = fields[6];
            std::string fcode  = fields[7];
            std::string country = fields[8];

            // ==================== 县级 ADM3 ====================
            if (fclass == "A") {
                if (fcode == "ADM1") {                    // 省级
                    std::string key = country + "." + fields[10];
                    admin1_names[key] = fields[1];
                }
                else if (fcode == "ADM2") {               // 市级
                    std::string key = country + "." + fields[10] + "." + fields[11];
                    admin2_names[key] = fields[1];
                }
                else if (fcode == "ADM3") {               // 县级（搜索用）
                    Location loc;
                    loc.lat         = std::stod(fields[4]);
                    loc.lon         = std::stod(fields[5]);
                    loc.county_name = fields[1];
                    loc.country     = country;
                    loc.admin1_code = fields[10];
                    loc.admin2_code = fields[11];
                    loc.feature     = "ADM3";
                    locations.push_back(loc);
                }
            }
        }
        std::cout << "加载完成，共 " << locations.size() << " 个县级行政区划（ADM3）\n";
        return true;
    }

    // 查询最近地点
    Location findNearest(double query_lat, double query_lon) const {
        Location nearest{};
        double min_dist = std::numeric_limits<double>::max();

        for (const auto& loc : locations) {
            double dist = haversine(query_lat, query_lon, loc.lat, loc.lon);
            if (dist < min_dist) {
                min_dist = dist;
                nearest = loc;
            }
        }
        return nearest;
    }
};

void LocalReverseGeocode(IMAGE_INFO_T& imageInfo, std::string dataFile) {
    ReverseGeocoder geocoder;
    if (!geocoder.load(dataFile)) {
        std::cerr << "无法加载数据文件！\n";
        return;
    }

    Location result = geocoder.findNearest(imageInfo.locationInfo.latitude, imageInfo.locationInfo.longitude);
    imageInfo.locationInfo.province = geocoder.admin1_names.at(result.country + "." + result.admin1_code);
    imageInfo.locationInfo.province = imageInfo.locationInfo.province.substr(0, imageInfo.locationInfo.province.find_last_of("sheng") - 4);
    imageInfo.locationInfo.city = geocoder.admin2_names.at(result.country + "." + result.admin1_code + "." + result.admin2_code);
    imageInfo.locationInfo.city = imageInfo.locationInfo.city.substr(0, imageInfo.locationInfo.city.find_last_of("shi") - 2);
    imageInfo.locationInfo.district = result.county_name.substr(0, result.county_name.find_last_of("xian") - 3);
    std::cout << std::fixed << std::setprecision(6);
    std::cout << "查询坐标: (" << imageInfo.locationInfo.latitude << ", " << imageInfo.locationInfo.longitude << ")\n";
    std::cout << "最近位置: " << result.county_name << " (" << result.country << ", " << result.admin1_code << ")\n";
    std::cout << "省市县" << geocoder.admin1_names.at(result.country + "." + result.admin1_code) << "-"
              << geocoder.admin2_names.at(result.country + "." + result.admin1_code + "." + result.admin2_code) << "-"
              << result.county_name << "\n";
    std::cout << "坐标: (" << result.lat << ", " << result.lon << ")\n";

    return;
}

// int main() {
//     ReverseGeocoder geocoder;
//     if (!geocoder.load("../../assets/GeoNames/allCountries/china_no_ppl.txt")) {
//         std::cerr << "无法加载数据文件！\n";
//         return 1;
//     }

//     std::cout << "请输入查询坐标（纬度 经度）：";
//     double lat, lon;
//     std::cin >> lat >> lon;
//     Location result = geocoder.findNearest(lat, lon);

//     std::cout << "查询坐标: (" << lat << ", " << lon << ")\n";
//     std::cout << "最近位置: " << result.county_name
//               << " (" << result.country << ", " << result.admin1_code << ")\n";
//     std::cout << "省市县乡: " << result.country << "-" 
//               << geocoder.admin1_names.at(result.country + "." + result.admin1_code) << "-"
//               << geocoder.admin2_names.at(result.country + "." + result.admin1_code + "." + result.admin2_code) << "-"
//               << result.county_name << "\n";
//     std::cout << "特征码: " << result.feature << "\n";
//     std::cout << "坐标: (" << result.lat << ", " << result.lon << ")\n";
//     return 0;
// }
