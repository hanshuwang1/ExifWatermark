#include <filesystem>
#include "AddExif/AddExifCImg.hpp"
#include "AddExif/AddExifOpencv.hpp"
#include "Baidu/BaiduReverseGeocode.hpp"
#include "ReadExif/ReadExif.hpp"
#include "args/args.hpp"
namespace fs = std::filesystem;


int main(int argc, char* argv[]) {
    ProgramArgs args = parse_arguments(argc, argv);
    std::string path = args.input_file;
    Exiv2::Image::UniquePtr image = Exiv2::ImageFactory::open(path);
    assert(image.get() != nullptr);
    image->readMetadata();
    Exiv2::ExifData exifData = image->exifData();

    /* (2) print and analyze exif data */
    IMAGE_INFO_T imageInfo;
    getCameraInfo(exifData, imageInfo);
    std::tie(imageInfo.locationInfo.latitude, imageInfo.locationInfo.longitude) = getGPS(exifData);
    

    /* (3) GPS to Real Position */
    std::string baiduMap_ak = args.ak; // visit https://lbsyun.baidu.com/apiconsole/key
    std::string baiduMap_sk = args.sk;
    if(args.reverse_geocode) {
        if (baiduMap_ak.empty() || baiduMap_sk.empty()) {
            std::cerr << "Baidu Map AK and SK are required for reverse geocoding.\n";
            return 1;
        }
        if(BaiduReverseGeocode_Offical(baiduMap_ak, baiduMap_sk, imageInfo) != 0) {
            std::cerr << "Failed to reverse geocode the location.\n";
            return 1;
        }
    }
    
    std::cout << "info" << std::endl;
    std::cout << "date: " << imageInfo.date << std::endl;
    std::cout << "manufacturer: " << imageInfo.cameraInfo.make << std::endl;
    std::cout << "model: " << imageInfo.cameraInfo.model << std::endl;
    std::cout << "shutter speed: " << imageInfo.cameraInfo.ExposureTime << std::endl;
    std::cout << "aperture: " << imageInfo.cameraInfo.FNumber << std::endl;
    std::cout << "ISO: " << imageInfo.cameraInfo.ISO << std::endl;
    std::cout << "focal length: " << imageInfo.cameraInfo.FocalLength << std::endl;
    std::cout << "longitude: " << imageInfo.locationInfo.longitude << std::endl;
    std::cout << "latitude: " << imageInfo.locationInfo.latitude << std::endl;
    std::cout << "province: " << imageInfo.locationInfo.province << std::endl;
    std::cout << "city: " << imageInfo.locationInfo.city << std::endl;
    std::cout << "district: " << imageInfo.locationInfo.district << std::endl;

    /* (4) add EXIF info to image bottom */
    addExifInfoToBottom_OpenCV(
        path,
        path.substr(0, path.find_last_of('.')) + "opencv.jpg",
        imageInfo,
        args.add_logo
    );
    if(args.add_logo) {
        std::string logoPath = logoDir + "Apple_Black.png"; // default logo
        std::string cameraMake = toLowerCopy(imageInfo.cameraInfo.make);
        std::string cameraModel = toLowerCopy(imageInfo.cameraInfo.model);
        for(const auto& logo : cameraLogos) {
            if (toLowerCopy(logo).find(cameraMake) != std::string::npos || 
                toLowerCopy(logo).find(cameraModel) != std::string::npos ||
                toLowerCopy(logo).find(clip_before_first(cameraModel, " ")) != std::string::npos) {
                logoPath = logoDir + logo;
                std::cout << "Matched camera logo: " << logo << std::endl;
                break;
            }
        }
        addLogo_OpenCV(
            path.substr(0, path.find_last_of('.')) + "opencv.jpg",
            path.substr(0, path.find_last_of('.')) + "final.jpg",
            logoPath // TODO: 根据相机型号选择对应的logo
        );
    }
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