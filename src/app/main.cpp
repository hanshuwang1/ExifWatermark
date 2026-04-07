#include <filesystem>
#include "AddExif/AddExifCImg.hpp"
#include "AddExif/AddExifOpencv.hpp"
#include "Baidu/BaiduReverseGeocode.hpp"
#include "ReadExif/ReadExif.hpp"
namespace fs = std::filesystem;


int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "用法: " << argv[0] << " <图片路径>\n";
        return 1;
    }
    /* (1) get image path */
    std::string path;
    if (argc == 2) {
        path = argv[1];
    } else {
        std::cout << "please enter the image path: ";
        std::getline(std::cin, path);
    }
    // Exiv2::ExifData exifData = readExifData(path);
    Exiv2::Image::UniquePtr image = Exiv2::ImageFactory::open(path);
    assert(image.get() != nullptr);
    image->readMetadata();
    Exiv2::ExifData exifData = image->exifData();

    /* (2) print and analyze exif data */
    IMAGE_INFO_T imageInfo;
    getCameraInfo(exifData, imageInfo);
    std::tie(imageInfo.locationInfo.latitude, imageInfo.locationInfo.longitude) = getGPS(exifData);
    

    /* (3) GPS to Real Position */
    std::string baiduMap_ak = "your_baidu_map_ak"; // visit https://lbsyun.baidu.com/apiconsole/key
    std::string baiduMap_sk = "your_baidu_map_sk";
    BaiduReverseGeocode_Offical(baiduMap_ak, baiduMap_sk,
        imageInfo.locationInfo.latitude, 
        imageInfo.locationInfo.longitude,
        imageInfo);
    std::cout << "图片信息：" << std::endl;
    std::cout << "日期: " << imageInfo.date << std::endl;
    std::cout << "相机制造商: " << imageInfo.cameraInfo.make << std::endl;
    std::cout << "相机型号: " << imageInfo.cameraInfo.model << std::endl;
    std::cout << "快门速度: " << imageInfo.cameraInfo.ExposureTime << std::endl;
    std::cout << "光圈大小: " << imageInfo.cameraInfo.FNumber << std::endl;
    std::cout << "感光度ISO: " << imageInfo.cameraInfo.ISO << std::endl;
    std::cout << "焦距: " << imageInfo.cameraInfo.FocalLength << std::endl;
    std::cout << "经度: " << imageInfo.locationInfo.longitude << std::endl;
    std::cout << "纬度: " << imageInfo.locationInfo.latitude << std::endl;
    std::cout << "省份: " << imageInfo.locationInfo.province << std::endl;
    std::cout << "城市: " << imageInfo.locationInfo.city << std::endl;
    std::cout << "区县: " << imageInfo.locationInfo.district << std::endl;
    std::cout << "街道: " << imageInfo.locationInfo.street << std::endl;

    /* (4) add EXIF info to image bottom */
    addExifInfoToBottom_OpenCV(
        path,
        path.substr(0, path.find_last_of('.')) + "opencv.jpg",
        imageInfo.date,
        imageInfo.cameraInfo.make + " " + imageInfo.cameraInfo.model,
        imageInfo.cameraInfo.FocalLength + " " + imageInfo.cameraInfo.ExposureTime + " " + imageInfo.cameraInfo.FNumber + " ISO" + imageInfo.cameraInfo.ISO,
        imageInfo.locationInfo.province + " " + imageInfo.locationInfo.city + " " + imageInfo.locationInfo.district
    );
    std::string logoPath = logoDir + "Apple_Black.png"; // default logo
    std::string cameraMake = toLowerCopy(imageInfo.cameraInfo.make);
    std::string cameraModel = toLowerCopy(imageInfo.cameraInfo.model);
    for(const auto& logo : cameraLogos) {
        if (toLowerCopy(logo).find(cameraMake) != std::string::npos || 
            toLowerCopy(logo).find(cameraModel) != std::string::npos) {
            logoPath = logoDir + logo;
            break;
        }
    }
    addLogo_OpenCV(
        path.substr(0, path.find_last_of('.')) + "opencv.jpg",
        path.substr(0, path.find_last_of('.')) + "final.jpg",
        logoPath // TODO: 根据相机型号选择对应的logo
    );
    // addExifInfoToBottom_CImg(
    //     path,
    //     path.substr(0, path.find_last_of('.')) + "CImg.jpg",
    //     imageInfo.date,
    //     imageInfo.cameraInfo.make + "  " + imageInfo.cameraInfo.model,
    //     imageInfo.cameraInfo.FocalLength + "  " + imageInfo.cameraInfo.ExposureTime + "  " + imageInfo.cameraInfo.FNumber + "  ISO " + imageInfo.cameraInfo.ISO,
    //     imageInfo.locationInfo.province + "  " + imageInfo.locationInfo.city + "  " + imageInfo.locationInfo.district
    // );
    return 0;
}