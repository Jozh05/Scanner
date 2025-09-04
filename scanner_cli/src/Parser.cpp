#include "cli/Parser.hpp"

#include <iostream>
#include <string_view>

namespace cli {


    void Parser::print_usage(std::ostream& os, const char* prog) {
        os << "Usage:\n"
       << "  " << prog << " --base <hashes.csv> --path <root_dir> [--log <detections.log>]\n\n"
       << "Flags:\n"
       << "  --base   Path to CSV with MD5 base (required)\n"
       << "  --path   Root directory to scan (required)\n"
       << "  --log    Path to detections log file (optional, overwritten on each run)\n"
       << "  -h, --help  Show this help\n";
    }



    static std::string_view take_value(int& i, int argc, char** argv, std::ostream& err, std::string_view flag) {
        if (i + 1 >= argc) {
            err << "missing value for " << flag << "\n";
            throw std::runtime_error("bad args");
        }
        return std::string_view(argv[++i]);
    }


    std::optional<Options> Parser::parse(int argc, char** argv, std::ostream& err) {
        if (argc == 1) {
            print_usage(err, argv[0]);
            return std::nullopt;
        }

        Options opt;
        bool have_base = false, have_path = false;

        try {
            for (int i = 1; i < argc; ++i) {
                std::string_view arg = argv[i];
                if (arg == "--base") {
                    opt.base_csv = std::string(take_value(i, argc, argv, err, arg));
                    have_base = true;
                } else if (arg == "--path") {
                    opt.root_path = std::string(take_value(i, argc, argv, err, arg));
                    have_path = true;
                } else if (arg == "--log") {
                    opt.log_path = std::string(take_value(i, argc, argv, err, arg));
                } else if (arg == "-h" || arg == "--help") {
                    print_usage(std::cout, argv[0]);
                } else if (!arg.empty() && arg[0] == '-') {
                    err << "unknown flag: " << arg << "\n";
                    return std::nullopt;
                } else {
                    err << "unexpected positional arg: " << arg << "\n";
                    return std::nullopt;
                }
            }
        } catch (...) {
            return std::nullopt;
        }

        if (!have_base) {
            err << "--base is required\n";
            return std::nullopt;
        }
        if (!have_path) {
            err << "--path is required\n";
            return std::nullopt;
        }
        return opt;
    }
} // namespace cli