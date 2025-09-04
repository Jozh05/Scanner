#pragma once

#include <filesystem>
#include <optional>

namespace cli {

    struct Options {
        std::filesystem::path base_csv;
        std::filesystem::path root_path;
        std::optional<std::filesystem::path> log_path;

    };
} // namespace cli