#pragma once

#include <optional>
#include <ostream>
#include "cli/Options.hpp"

namespace cli {

class Parser {

public:
    static std::optional<Options> parse(int argc, char** argv, std::ostream& err);

    static void print_usage(std::ostream& os, const char* prog);
    Parser() = delete;
    
};

} //namespace cli