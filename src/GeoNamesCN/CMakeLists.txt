add_library(MyProject_GeoNamesCN
    GeoNames.cpp
)

add_library(MyProject::GeoNamesCN ALIAS MyProject_GeoNamesCN)

target_include_directories(MyProject_GeoNamesCN PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
