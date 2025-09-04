#pragma once

#include "cli/Options.hpp"

#include <scanner/Types.hpp>

#include <ostream>
#include <cstddef>

namespace cli {

class App {
public:
    explicit App(Options opts);
    int run();

private:
    std::ostream& print_stats(std::ostream& os,
        const scanner::ScanStats& stats,
        const int64_t time) const;
private:
    Options opts_;
};
} // namespace cli