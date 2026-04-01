#include "headfile.hpp" 
#include <opencv4/opencv2/opencv.hpp>
#include <opencv4/opencv2/core.hpp>
#include <opencv4/opencv2/imgproc.hpp>
#include <opencv4/opencv2/imgcodecs.hpp>
#include <opencv4/opencv2/freetype.hpp>
#include <string>
#include <iostream>
#include <freetype2/ft2build.h>

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
    std::string fontPath = "./file/msyhl.ttc";  
    std::string fontPath_bold = "./file/SourceHanSansCN-Bold.otf";

    // draw text
    ft2->loadFontData(fontPath_bold.c_str(), 0); 
    ft2->putText(result, cameraLens, cv::Point(40, h + 30), 42, cv::Scalar(0, 0, 0), -1, cv::LINE_AA, false);
    ft2->putText(result, exposure, cv::Point(w - 600, h + 30), 42, cv::Scalar(0, 0, 0), -1, cv::LINE_AA, false);
    ft2->putText(result, location, cv::Point(w - 600, h + 90), 30, cv::Scalar(0, 0, 0), -1, cv::LINE_AA, false);
    ft2->loadFontData(fontPath.c_str(), 0);
    ft2->putText(result, dateTime, cv::Point(40, h + 90), 30, cv::Scalar(0, 0, 0), -1, cv::LINE_AA, false);

    cv::imwrite(outputPath, result);
    std::cout << "opencv add Exif info to: " << outputPath << std::endl;
}
