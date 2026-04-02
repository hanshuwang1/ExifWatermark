add_library(MyProject_AddExif
    AddExifCImg.cpp
    AddExifOpencv.cpp
)

add_library(MyProject::AddExif ALIAS MyProject_AddExif)

target_include_directories(MyProject_AddExif PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(MyProject_AddExif PUBLIC
    Freetype::Freetype
    JPEG::JPEG
    nlohmann_json::nlohmann_json
    OpenSSL::Crypto
    ${OpenCV_LIBS}
)
