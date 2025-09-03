#pragma once

#include <filesystem>

#include "scanner/Types.hpp"
#include "scanner/detail/BoundedQueue.hpp"

namespace scanner::detail {

class Producer {

public:
    Producer(const std::filesystem::path& root,
            const ScanConfig& config,
            BoundedQueue<std::filesystem::path>& queue,
            ScanStats& stats);
    
    void run();

private:
        void traverse();
        void process_entry(const std::filesystem::directory_entry& dir_entry);

private:
    const std::filesystem::path& root_;
    const ScanConfig& config_;
    BoundedQueue<std::filesystem::path>& queue_;
    ScanStats& stats_;
};
} // namespace scanner::detail