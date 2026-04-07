cmake_minimum_required(VERSION 3.25)
project(ExifWatermarker VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# find dependencies 
find_package(Exiv2 REQUIRED CONFIG)
find_package(OpenSSL REQUIRED)
find_package(CURL REQUIRED)
find_package(nlohmann_json REQUIRED)

add_definitions(-Dcimg_use_jpeg)   # 启用 JPEG 支持
find_package(JPEG REQUIRED)
find_package(OpenCV REQUIRED)
find_package(Freetype REQUIRED)


# include headers
include_directories(${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/external
)


# add subdirectory
add_subdirectory(src/AddExif)
add_subdirectory(src/app)
add_subdirectory(src/Baidu)
add_subdirectory(src/ReadExif)
# add_subdirectory(external)

# install and release
install(TARGETS ExifWatermarker
    RUNTIME DESTINATION .
    COMPONENT Runtime
)
# install dependency exiv2
if(WIN32)
    install(FILES "external/exiv2/bin/libexiv2.dll"
         DESTINATION . 
         COMPONENT Runtime)
endif()

# install assets
install(DIRECTORY assets
    DESTINATION .
    COMPONENT Runtime
)

# conditionally include tests
option(BUILD_TESTS "Build tests" OFF)
if(BUILD_TESTS)
    # ==================== GoogleTest 配置 ====================
    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
        # GIT_REPOSITORY https://github.com/google/googletest.git
        # GIT_TAG release-1.15.2
    )
    # For Windows: prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    enable_testing()
    include(GoogleTest)

    # 可选：让 ctest 输出更友好
    list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")

    enable_testing()
    add_subdirectory(tests)
endif()
