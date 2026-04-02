#ifndef _EXIFWATERMARK_INCLUDE_READEXIF_HPP
#define _EXIFWATERMARK_INCLUDE_READEXIF_HPP
#include <iostream>
#include <exiv2/exiv2.hpp>
template<typename Stream = std::ostream>
void printExifDatum(const Exiv2::Exifdatum& datum, Stream& os = std::cout, int indent = 0)
{
    std::string key   = datum.key();        
    std::string label = datum.tagLabel();     
    std::string value = datum.toString();
    std::string type  = Exiv2::TypeInfo::typeName(datum.typeId());

    os << std::string(indent * 2, ' ') 
       << std::left << std::setw(40) << key 
       << " | " << std::left << std::setw(28) << label
       << " | " << std::setw(10) << type
       << " | " << value << "\n";
}

template<typename Stream = std::ostream>
void printExifData(const Exiv2::ExifData& exifData, Stream& os = std::cout, int indent = 0)
{
    if (exifData.empty()) {
        os << std::string(indent * 2, ' ') << "No EXIF data found.\n";
        return;
    }

    os << std::string(indent * 2, ' ') 
       << "=================== EXIF Data ===================\n";
    os << std::string(indent * 2, ' ')
       << std::left << std::setw(40) << "Key"
       << " | " << std::left << std::setw(28) << "Tag Label"
       << " | " << std::setw(10) << "Type"
       << " | Value\n";
    os << std::string(indent * 2, ' ') 
       << std::string(120, '-') << "\n";

    for (const auto& datum : exifData) {
        printExifDatum(datum, os, indent);
    }

    os << std::string(indent * 2, ' ') 
       << "=================================================\n";
}

Exiv2::ExifData readExifData(const std::string& imagePath);

#endif // _EXIFWATERMARK_INCLUDE_READEXIF_HPP
