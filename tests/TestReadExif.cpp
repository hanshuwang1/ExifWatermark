#include <gtest/gtest.h>
#include "ReadExif/ReadExif.hpp"
#include <filesystem>
#include <fstream>


// Fixture 示例
class ReadExifTest : public ::testing::Test {
protected:
    std::string test_image_path;
    std::string no_exif_image;

    void SetUp() override {
        test_image_path = "tests/data/test_with_exif.jpg";
        no_exif_image   = "tests/data/test_no_exif.jpg";

        ASSERT_TRUE(std::filesystem::exists(test_image_path)) << "测试图片不存在: " << test_image_path;
        ASSERT_TRUE(std::filesystem::exists(no_exif_image)) << "无 EXIF 图片不存在: " << no_exif_image;

    }
};

TEST_F(ReadExifTest, ReadExifDataFromImage) {
    Exiv2::ExifData exifData;
    ASSERT_NO_THROW(exifData = readExifData(test_image_path));
    EXPECT_FALSE(exifData.empty());
}

TEST_F(ReadExifTest, ReadGPSFromImage) {
    Exiv2::ExifData exifData;
    ASSERT_NO_THROW(exifData = readExifData(test_image_path));

    const auto gps = getGPS(exifData);
    EXPECT_NEAR(gps.first, 41.5276, 0.001);
    EXPECT_NEAR(gps.second, 115.918, 0.001);
}

TEST_F(ReadExifTest, ReadNoGPSImage) {
    Exiv2::ExifData exifData;
    ASSERT_NO_THROW(exifData = readExifData(no_exif_image));
    const auto gps = getGPS(exifData);
    EXPECT_EQ(gps.first, 0.0);
    EXPECT_EQ(gps.second, 0.0);
}


// 参数化测试：不同图片格式或不同 EXIF 值
// class ImageFormatTest : public ReadExifTest,
//                         public ::testing::WithParamInterface<std::string> {};

// TEST_P(ImageFormatTest, CanReadBasicExif) {
//     std::string img = GetParam();
//     ReadExif reader(img);
//     EXPECT_NO_THROW(reader.readAllExif());   // 或具体检查
// }

// INSTANTIATE_TEST_SUITE_P(DifferentFormats, ImageFormatTest,
//     ::testing::Values(
//         "tests/data/test.jpg",
//         "tests/data/test.png",
//         "tests/data/test.tiff"
//     ));
