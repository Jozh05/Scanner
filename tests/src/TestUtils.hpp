#pragma once
#include <filesystem>
#include <fstream>
#include <string>
#include <string_view>
#include <vector>
#include <chrono>

namespace testutil {
namespace fs = std::filesystem;

struct TempDir {
    fs::path dir;
    explicit TempDir(std::string_view prefix = "scanner_ut_") {
        auto base = fs::temp_directory_path();
        auto stamp = std::chrono::high_resolution_clock::now().time_since_epoch().count();
        dir = base / (std::string(prefix) + std::to_string(stamp));
        fs::create_directories(dir);
    }
    ~TempDir() {
        std::error_code ec;
        fs::remove_all(dir, ec);
    }
    fs::path path(std::string_view name) const { return dir / std::string(name); }
};

inline void write_text(const std::filesystem::path& p, std::string_view s) {
    std::ofstream o(p, std::ios::binary | std::ios::trunc);
    o.write(s.data(), static_cast<std::streamsize>(s.size()));
    o.close();
}

inline void write_bytes(const std::filesystem::path& p, const std::vector<char>& v) {
    std::ofstream o(p, std::ios::binary | std::ios::trunc);
    o.write(v.data(), static_cast<std::streamsize>(v.size()));
    o.close();
}
} // namespace testutil
