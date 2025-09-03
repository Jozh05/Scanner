#include "scanner/Logger.hpp"

#include <stdexcept>

namespace scanner {
    
    Logger::Logger(const std::filesystem::path& file) {
        out_.open(file, std::ios::out | std::ios::app);
        if (!out_)
            throw std::runtime_error("cannot open log file: " + file.generic_string());
    }


    Logger::~Logger() {
        try {
            flush();
        } catch (...) {}
    }


    void Logger::flush() {
        std::lock_guard lock(mtx_);
        out_.flush();
    }


    void Logger::log_detection(std::string_view hash,
                                std::string_view verdict,
                                const std::filesystem::path& path) {
        
        std::lock_guard lock(mtx_);
        out_ << hash << ";" << verdict << ";" << path.generic_string() << '\n';
    }
}