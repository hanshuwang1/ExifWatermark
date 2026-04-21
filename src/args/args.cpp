// args.cpp
#include "args.hpp"
#include <iostream>
#include <stdexcept>

ProgramArgs parse_arguments(int argc, char* argv[])
{
    argparse::ArgumentParser program("myprog", "1.0");

    program.add_argument("input")
        .help("image file path")
        .required();

    program.add_argument("--verbose")
        .default_value(false)
        .implicit_value(true)
        .help("enable verbose output");
    
    program.add_argument("--reverse_geocode_local")
        .help("enable local reverse geocode using GeoNames data")
        .default_value(false)
        .implicit_value(true);

    program.add_argument("--reverse_geocode_online")
        .help("enable baidu map reverse geocode")
        .default_value(false)
        .implicit_value(false);
    
    program.add_argument("--ak")
        .help("baidu map ak, visit https://lbsyun.baidu.com/apiconsole/key")
        .default_value(std::string{""});
    
    program.add_argument("--sk")
        .help("baidu map sk corresponding to ak")
        .default_value(std::string{""});

    program.add_argument("--add_logo")
        .help("enable adding camera logo to image")
        .default_value(false)
        .implicit_value(true);

    try {
        program.parse_args(argc, argv);
    }
    catch (const std::runtime_error& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program;   // 自动打印帮助信息
        std::exit(1);           // 或 throw，根据需要
    }

    ProgramArgs args;
    args.input_file = program.get<std::string>("input");
    args.verbose    = program["--verbose"] == true;
    args.reverse_geocode_online = program["--reverse_geocode_online"] == true;
    args.reverse_geocode_local = program["--reverse_geocode_local"] == true;
    args.ak           = program.get<std::string>("--ak");
    args.sk           = program.get<std::string>("--sk");
    args.add_logo     = program["--add_logo"] == true;

    return args;
}

/**
  * @brief clip before first space substr
  * @param origin
  * @param find_str
  * @retval clip_str
  */
std::string clip_before_first(const std::string& origin, const std::string& find_str) {
    size_t pos = origin.find(find_str);
    if (pos != std::string::npos) {
        return origin.substr(0, pos);
    }
    return origin;
}
