#pragma once

#include <atomic>
#include <cstdint>
#include <thread>

namespace scanner {

    struct ScanConfig {
        bool recursive = true;
        bool follow_symlinks = false;
        std::size_t threads = std::thread::hardware_concurrency();
        std::size_t chunk_size = 1 << 20;
        std::size_t queue_capacity = 1024;
    };

    struct ScanStats {
        std::atomic<std::uint64_t> files = 0;
        std::atomic<std::uint64_t> malicious = 0;
        std::atomic<std::uint64_t> errors = 0;
    };
} // namespace scanner