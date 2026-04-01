// #include "./external/exiv2/include/exiv2/exiv2.hpp"
#include "headfile.hpp"

static std::string formatFloat(double value, int digits) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(digits) << value;
    return oss.str();
}

int main(int argc, char* argv[]) {
    /* (1) get image path */
    std::string path;
    if (argc == 2) {
        path = argv[1];
    } else {
        std::cout << "please enter the image path: ";
        std::getline(std::cin, path);
    }
    Exiv2::ExifData exifData = readExifData(path);

    /* (2) print and analyze exif data */
    IMAGE_INFO_T imageInfo;
    imageInfo.date = exifData["Exif.Image.DateTime"].toString();
    imageInfo.cameraInfo.make = exifData["Exif.Image.Make"].toString();
    imageInfo.cameraInfo.model = exifData["Exif.Image.Model"].toString();
    imageInfo.cameraInfo.software = exifData["Exif.Image.Software"].toString();
    imageInfo.cameraInfo.ExposureTime = "1/" + formatFloat(1.0 / exifData["Exif.Photo.ExposureTime"].toFloat(), 0);
    imageInfo.cameraInfo.FNumber = "f/" + formatFloat(exifData["Exif.Photo.FNumber"].toFloat(), 1);
    imageInfo.cameraInfo.ISO = exifData["Exif.Photo.ISOSpeedRatings"].toString();
    imageInfo.cameraInfo.FocalLength = formatFloat(exifData["Exif.Photo.FocalLength"].toFloat(), 0) + "mm";
    //rational to float
    double degrees = exifData["Exif.GPSInfo.GPSLongitude"].toFloat();
    double minutes = exifData["Exif.GPSInfo.GPSLongitude"].toFloat(1);
    double seconds = exifData["Exif.GPSInfo.GPSLongitude"].toFloat(2);
    imageInfo.locationInfo.longitude = degrees + minutes / 60.0 + seconds / 3600.0;
    degrees = exifData["Exif.GPSInfo.GPSLatitude"].toFloat();
    minutes = exifData["Exif.GPSInfo.GPSLatitude"].toFloat(1);
    seconds = exifData["Exif.GPSInfo.GPSLatitude"].toFloat(2);
    imageInfo.locationInfo.latitude = degrees + minutes / 60.0 + seconds / 3600.0;

    /* (3) GPS to Real Position */
    std::string baiduMap_ak = "your baidu map ak"; // visit https://lbsyun.baidu.com/apiconsole/key
    BaiduReverseGeocode_Offical(baiduMap_ak, 
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