#include <iostream>
#include <iomanip>
#include <cassert>
#include <exiv2/exiv2.hpp>

Exiv2::ExifData readExifData(const std::string& imagePath)
{
    Exiv2::Image::UniquePtr image = Exiv2::ImageFactory::open(imagePath);
    assert(image.get() != nullptr);
    image->readMetadata();
    return image->exifData();
}
