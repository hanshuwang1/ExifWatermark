#ifndef _SRC_ARGS_ARGS_HPP
#define _SRC_ARGS_ARGS_HPP
#pragma once

#include <string>
#include <optional>
#include "argparse/argparse.hpp"

struct ProgramArgs {
    std::string input_file;
    bool verbose = false;
    bool reverse_geocode = false;
    std::string ak;
    std::string sk;
    bool add_logo = false;
};

// 声明解析函数
ProgramArgs parse_arguments(int argc, char* argv[]);


#endif // _SRC_ARGS_ARGS_HPP
