#ifndef _EXIFWATERMARK_INCLUDE_ADDEXIFOPENCV_HPP
#define _EXIFWATERMARK_INCLUDE_ADDEXIFOPENCV_HPP
#include <string>
void addExifInfoToBottom_OpenCV(
    const std::string& inputPath,
    const std::string& outputPath,
    const std::string& dateTime,
    const std::string& cameraLens,
    const std::string& exposure,
    const std::string& location);

#endif // _EXIFWATERMARK_INCLUDE_ADDEXIFOPENCV_HPP
