add_library(MyProject_Baidu
    BaiduReverseGeocode.cpp
)

add_library(MyProject::BaiduReverseGeocode ALIAS MyProject_Baidu)

target_include_directories(MyProject_Baidu PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(MyProject_Baidu PUBLIC
    CURL::libcurl
    nlohmann_json::nlohmann_json
    
)
