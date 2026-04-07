#include <string>
#include <iostream>
#include <algorithm>
#include <cctype>
#include <opencv4/opencv2/opencv.hpp>
#include <opencv4/opencv2/core.hpp>
#include <opencv4/opencv2/imgproc.hpp>
#include <opencv4/opencv2/imgcodecs.hpp>
#include <opencv4/opencv2/freetype.hpp>
#include <freetype2/ft2build.h>

/**
  * @brief add text info
  * @param None
  * @retval None
  */
void addExifInfoToBottom_OpenCV(
    const std::string& inputPath,
    const std::string& outputPath,
    const std::string& dateTime,
    const std::string& cameraLens,
    const std::string& exposure,
    const std::string& location)
{
    cv::Mat img = cv::imread(inputPath);
    if (img.empty()) {
        std::cerr << "无法读取图片: " << inputPath << std::endl;
        return;
    }

    int w = img.cols;
    int h = img.rows;

    // bottom bar height
    const int barHeight = 180;
    cv::Mat result(h + barHeight, w, CV_8UC3);

    // copy original image
    img.copyTo(result(cv::Rect(0, 0, w, h)));

    // draw white bar
    cv::rectangle(result, cv::Point(0, h), cv::Point(w, h + barHeight), 
                  cv::Scalar(255, 255, 255), -1);   // -1 表示填充

    // use FreeType2 to draw text supporting CN
    cv::Ptr<cv::freetype::FreeType2> ft2 = cv::freetype::createFreeType2();
    std::string fontPath = "./assets/fonts/msyhl.ttc";  
    std::string fontPath_bold = "./assets/fonts/SourceHanSansCN-Bold.otf";

    // draw text
    ft2->loadFontData(fontPath_bold.c_str(), 0); 
    ft2->putText(result, cameraLens, cv::Point(40, h + 30), 42, cv::Scalar(0, 0, 0), -1, cv::LINE_AA, false);
    ft2->putText(result, exposure, cv::Point(w - 600, h + 30), 42, cv::Scalar(0, 0, 0), -1, cv::LINE_AA, false);
    ft2->putText(result, location, cv::Point(w - 600, h + 90), 30, cv::Scalar(0, 0, 0), -1, cv::LINE_AA, false);
    ft2->loadFontData(fontPath.c_str(), 0);
    ft2->putText(result, dateTime, cv::Point(40, h + 90), 30, cv::Scalar(0, 0, 0), -1, cv::LINE_AA, false);

    // draw seperator line
    cv::line(result, cv::Point(w - 650, h + 25), cv::Point(w - 650, h + 150), 
             cv::Scalar(200, 200, 200), 1, cv::LINE_AA);

    cv::imwrite(outputPath, result);
    std::cout << "opencv add Exif info to: " << outputPath << std::endl;
}

/**
  * @brief add camera logo
  * @param None
  * @retval None
  */
void addLogo_OpenCV(
    const std::string& inputPath,
    const std::string& outputPath,
    const std::string& logoPath)
{
    cv::Mat img = cv::imread(inputPath);
    if (img.empty()) {
        std::cerr << "无法读取图片: " << inputPath << std::endl;
        return;
    }

    cv::Mat logo = cv::imread(logoPath, cv::IMREAD_UNCHANGED);
    if (logo.empty()) {
        std::cerr << "无法读取logo图片: " << logoPath << std::endl;
        return;
    }

    int w = img.cols;
    int h = img.rows;

    // resize logo to fit ,just adjust height, width auto scale
    int logoHeight = 100;
    int logoWidth = static_cast<int>(logo.cols * (static_cast<float>(logoHeight) / logo.rows));
    cv::resize(logo, logo, cv::Size(logoWidth, logoHeight));
    // int logoWidth = 100;
    // int logoHeight = static_cast<int>(logo.rows * (static_cast<float>(logoWidth) / logo.cols));
    // cv::resize(logo, logo, cv::Size(logoWidth, logoHeight));

    // create result image
    cv::Mat result(h, w, CV_8UC3);
    img.copyTo(result);

    // overlay logo on bottom right corner
    for (int y = 0; y < logo.rows; ++y) {
        for (int x = 0; x < logo.cols; ++x) {
            cv::Vec4b pixel = logo.at<cv::Vec4b>(y, x);
            if (pixel[3] > 0) { // alpha channel check
                int posX = w - logo.cols + x - 700; // 700px padding from right edge
                int posY = h - logo.rows + y - 40; // 40px padding from bottom edge
                if (posX >= 0 && posY >= 0 && posX < w && posY < h) {
                    result.at<cv::Vec3b>(posY, posX) = cv::Vec3b(pixel[0], pixel[1], pixel[2]);
                }
            }
        }
    }

    cv::imwrite(outputPath, result);
    std::cout << "opencv add Logo to: " << outputPath << std::endl;
}

std::string toLowerCopy(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
        [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return s;
}