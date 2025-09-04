#include "cli/Parser.hpp"
#include "cli/App.hpp"
#include <iostream>


int main (int argc, char** argv) {
    
    auto opts = cli::Parser::parse(argc, argv, std::cerr);
    if (!opts) {
        cli::Parser::print_usage(std::cerr, argv[0]);
        return 3;
    }
    cli::App app(opts.value());
    return app.run();
}