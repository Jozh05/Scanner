#pragma once

#include <fstream>
#include <filesystem>
#include <mutex>
#include <string_view>

namespace scanner {
class Logger {

public:
    explicit Logger(const std::filesystem::path& file);
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