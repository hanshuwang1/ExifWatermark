add_library(MyProject_ReadExif
    ReadExif.cpp
)

add_library(MyProject::ReadExif ALIAS MyProject_ReadExif)

target_include_directories(MyProject_ReadExif PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
# list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/external/exiv2")

target_link_libraries(MyProject_ReadExif PUBLIC
    Exiv2::exiv2lib
)