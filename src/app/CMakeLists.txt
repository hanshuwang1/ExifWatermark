add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        MyProject::AddExif
        MyProject::ReadExif
        MyProject::BaiduReverseGeocode
        MyProject::Args
)

if(WIN32 AND CMAKE_VERSION VERSION_GREATER_EQUAL 3.21)
    add_custom_command(TARGET ExifWatermarker POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_RUNTIME_DLLS:ExifWatermarker>
            $<TARGET_FILE_DIR:ExifWatermarker>
        COMMAND_EXPAND_LISTS
        COMMENT "Copying runtime DLLs to executable directory"
    )
endif()
