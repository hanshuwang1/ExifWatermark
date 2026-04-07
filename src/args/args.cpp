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

    program.add_argument("-v", "--verbose")
        .default_value(false)
        .implicit_value(true)
        .help("enable verbose output");

    program.add_argument("--reverse-geocode")
        .help("enable baidu map reverse geocode")
        .default_value(false)
        .implicit_value(true);
    
    program.add_argument("--ak")
        .help("baidu map ak, visit https://lbsyun.baidu.com/apiconsole/key")
        .default_value(std::string{""});
    
    program.add_argument("--sk")
        .help("baidu map sk corresponding to ak")
        .default_value(std::string{""});

    program.add_argument("--add-logo")
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
    args.reverse_geocode = program["--reverse-geocode"] == true;
    args.ak           = program.get<std::string>("--ak");
    args.sk           = program.get<std::string>("--sk");
    args.add_logo     = program["--add-logo"] == true;

    return args;
}
