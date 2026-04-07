add_library(MyProject_Args
    args.cpp
)
add_library(MyProject::Args ALIAS MyProject_Args)

target_include_directories(MyProject_Args PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
