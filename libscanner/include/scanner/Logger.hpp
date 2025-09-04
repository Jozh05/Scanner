#pragma once

#include <fstream>
#include <filesystem>
#include <mutex>
#include <string_view>

namespace scanner {
class Logger {

public:
    
    enum class OpenMode {Truncate, Append};

    explicit Logger(const std::filesystem::path& file, OpenMode mode = OpenMode::Truncate);
    ~Logger();

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    void log_detection(std::string_view hash,
                        std::string_view verdict,
                        const std::filesystem::path& path);

    
    void flush();

private:
    std::mutex mtx_;
    std::ofstream out_;
};
}