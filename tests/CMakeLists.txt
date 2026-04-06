add_executable(ExifWatermarkTests
    TestReadExif.cpp
    TestAddExif.cpp
    TestBaiduApi.cpp
    # 如果以后测试文件增多，可以继续添加
)

target_link_libraries(ExifWatermarkTests
    PRIVATE 
        gtest 
        gtest_main
        # link to the main library if needed, e.g. MyProject::AddExif
        MyProject::AddExif
        MyProject::ReadExif
        MyProject::BaiduReverseGeocode
)

target_include_directories(ExifWatermarkTests PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)

# 自动发现所有 TEST() / TEST_F() 并注册到 ctest
gtest_discover_tests(ExifWatermarkTests)
