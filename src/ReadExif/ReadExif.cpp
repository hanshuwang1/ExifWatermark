#include <iostream>
#include <iomanip>
#include <cassert>
#include <exiv2/exiv2.hpp>
#include "ReadExif/ReadExif.hpp"


Exiv2::ExifData readExifData(const std::string& imagePath)
{
    Exiv2::Image::UniquePtr image = Exiv2::ImageFactory::open(imagePath);
    assert(image.get() != nullptr);
    image->readMetadata();
    return image->exifData();
}
/**
  * @brief post dot digits
  * @param value input
  * @param digits 
  * @retval string 
  */
static std::string formatFloat(double value, int digits) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(digits) << value;
    return oss.str();
}

/**
  * @brief read camera info
  * @param exifData Exif data read from image
  * @param ImageInfo_t 
  * @retval None
  */
void getCameraInfo(const Exiv2::ExifData& exifData, IMAGE_INFO_T& imageInfo)
{
    // Implementation for reading camera info
    Exiv2::ExifKey dateKey("Exif.Image.DateTime");
    Exiv2::ExifKey makeKey("Exif.Image.Make");
    Exiv2::ExifKey modelKey("Exif.Image.Model");
    Exiv2::ExifKey softwareKey("Exif.Image.Software");
    Exiv2::ExifKey exposureTimeKey("Exif.Photo.ExposureTime");
    Exiv2::ExifKey fNumberKey("Exif.Photo.FNumber");
    Exiv2::ExifKey isoKey("Exif.Photo.ISOSpeedRatings");
    Exiv2::ExifKey focalLengthKey("Exif.Photo.FocalLength");

    imageInfo.date = exifData.findKey(dateKey) != exifData.end() ? exifData.findKey(dateKey)->toString() : "Unknown";
    imageInfo.cameraInfo.make = exifData.findKey(makeKey) != exifData.end() ? exifData.findKey(makeKey)->toString() : "Unknown";
    imageInfo.cameraInfo.model = exifData.findKey(modelKey) != exifData.end() ? exifData.findKey(modelKey)->toString() : "Unknown";
    imageInfo.cameraInfo.software = exifData.findKey(softwareKey) != exifData.end() ? exifData.findKey(softwareKey)->toString() : "Unknown";
    imageInfo.cameraInfo.ExposureTime = "1/" + formatFloat(1.0 / exifData.findKey(exposureTimeKey)->toFloat(), 0);
    imageInfo.cameraInfo.FNumber = "f/" + formatFloat(exifData.findKey(fNumberKey)->toFloat(), 1);
    imageInfo.cameraInfo.ISO = exifData.findKey(isoKey) != exifData.end() ? exifData.findKey(isoKey)->toString() : "Unknown";
    imageInfo.cameraInfo.FocalLength = formatFloat(exifData.findKey(focalLengthKey)->toFloat(), 0) + "mm";
}

/**
  * @brief Read GPS, rational to float
  * @param exifData Exif data read from the image
  * @retval latitude, longitude
  */
std::pair<double, double> getGPS(const Exiv2::ExifData& exifData)
{
    double longitude = 0.0;
    double latitude = 0.0;
    Exiv2::ExifKey gpsLongitudeKey("Exif.GPSInfo.GPSLongitude");
    Exiv2::ExifKey gpsLatitudeKey("Exif.GPSInfo.GPSLatitude");
    const auto& gpsLongitudeIt = exifData.findKey(gpsLongitudeKey);
    const auto& gpsLatitudeIt = exifData.findKey(gpsLatitudeKey);
    if (gpsLongitudeIt != exifData.end() &&
        gpsLatitudeIt != exifData.end()) {
        // rational to float
        const double degrees = gpsLongitudeIt->toFloat();
        const double minutes = gpsLongitudeIt->toFloat(1);
        const double seconds = gpsLongitudeIt->toFloat(2);
        longitude = degrees + minutes / 60.0 + seconds / 3600.0;
        const double latDegrees = gpsLatitudeIt->toFloat();
        const double latMinutes = gpsLatitudeIt->toFloat(1);
        const double latSeconds = gpsLatitudeIt->toFloat(2);
        latitude = latDegrees + latMinutes / 60.0 + latSeconds / 3600.0;
    }
    return {latitude, longitude};
}
