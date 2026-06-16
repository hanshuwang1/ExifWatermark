cmake_minimum_required(VERSION 3.25)
project(ExifWatermarker VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# colorfy log
string(ASCII 27 ESC)
set(CMAKE_COLOR_MAKEFILE ON)
set(CMAKE_COLOR_OUTPUT ON)
set(RED "${ESC}[31m")
set(GREEN "${ESC}[32m")
set(YELLOW "${ESC}[33m")
set(BLUE "${ESC}[34m")
set(MAGENTA "${ESC}[35m")
set(CYAN "${ESC}[36m")
set(WHITE "${ESC}[37m")
set(RESET "${ESC}[0m")

# find dependencies
function(require_package pkg)
    find_package(${pkg} QUIET)

    if(${pkg}_FOUND)
        message(STATUS "${GREEN}[OK] ${pkg} found${RESET}")
    else()
        message(WARNING "${YELLOW}[WARN] ${pkg} not found${RESET}")
        message(FATAL_ERROR "${RED}[ERR] ${pkg} is required${RESET}")
    endif()
endfunction()
require_package(Exiv2)
require_package(OpenSSL)
require_package(CURL)
require_package(nlohmann_json)

add_definitions(-Dcimg_use_jpeg)   # 启用 JPEG 支持
require_package(JPEG)
require_package(OpenCV)
require_package(Freetype)


# include headers
include_directories(${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/external
)


# add subdirectory
function(add_subdir subdir)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${subdir}/CMakeLists.txt")
        add_subdirectory(${subdir})
        message(STATUS "${GREEN}[OK] ${subdir} added${RESET}")
    else()
        message(WARNING "${YELLOW}[WARN] ${subdir} not added: CMakeLists.txt missing${RESET}")
    endif()
endfunction()
add_subdir(src/AddExif)
add_subdir(src/app)
add_subdir(src/Baidu)
add_subdir(src/ReadExif)
add_subdir(src/args)
add_subdir(src/GeoNamesCN)
# add_subdirectory(external)

# install and release
install(TARGETS ExifWatermarker
    RUNTIME DESTINATION .
    COMPONENT Runtime
)
message(STATUS "${GREEN}[OK] Installation completed${RESET}")
# install dependency exiv2
if(WIN32)
    message(STATUS "${GREEN}[OK] Installing exiv2 dependency${RESET}")
    install(FILES "external/exiv2/bin/libexiv2.dll"
         DESTINATION . 
         COMPONENT Runtime)
endif()

# install assets
install(DIRECTORY assets
    DESTINATION .
    COMPONENT Runtime
)
message(STATUS "${GREEN}[OK] Assets installed${RESET}")

# conditionally include tests
option(BUILD_TESTS "Build tests" OFF)
if(BUILD_TESTS)
    message(STATUS "${YELLOW}Building tests${RESET}")
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
