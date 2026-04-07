#ifndef _EXIFWATERMARK_INCLUDE_ADDEXIFOPENCV_HPP
#define _EXIFWATERMARK_INCLUDE_ADDEXIFOPENCV_HPP
#include <string>
#include <vector>
// logo 
const std::string logoDir = "./assets/logo/Camera-Logos-SVG/PNG/";
const std::vector<std::string> cameraLogos = {
    "Apple_Black.png",
    "Canon_Black.png",
    "Canon_Color.png",
    "DJI_Black.png",
    "Fujifilm_BLack.png",
    "Fujifilm_Color.png",
    "GoPro_Black.png",
    "hasselblad-Black.png",
    "Huawei_Black.png",
    "Huawei_Color.png",
    "Insta360_Black.png",
    "IQOO_Color.png",
    "Leica_Black.png",
    "Leica_Color.png",
    "Lumix_Black.png",
    "Meizu-Black.png",
    "Meizu_Color.png",
    "Nikon_Black.png",
    "Nikon_Color.png",
    "Oneplus_Black.png",
    "Oneplus_Color.png",
    "OPPO_Black.png",
    "OPPO_Color.png",
    "Osmo_Action_Black.png",
    "phaseone_Color.png",
    "Realme_Black.png",
    "Redmi_Black.png",
    "Redmi_Color.png",
    "Samsung_black.png",
    "SIGMA_Black.png",
    "SIGMA_Icon_Black.png",
    "SONYalpha_Black.png",
    "SONY_Black.png",
    "TAMRON_Black.png",
    "Tokina_Black.png",
    "Vivo_Black.png",
    "Vivo_Color.png",
    "Xiaomi_Black.png",
    "ZEISS_Black.png",
    "ZEISS_Color.png"
};
void addExifInfoToBottom_OpenCV(
    const std::string& inputPath,
    const std::string& outputPath,
    const std::string& dateTime,
    const std::string& cameraLens,
    const std::string& exposure,
    const std::string& location);

void addLogo_OpenCV(
    const std::string& inputPath,
    const std::string& outputPath,
    const std::string& logoPath);

std::string toLowerCopy(std::string s);

#endif // _EXIFWATERMARK_INCLUDE_ADDEXIFOPENCV_HPP
