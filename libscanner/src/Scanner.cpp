#include "scanner/Scanner.hpp"
#include "scanner/detail/BoundedQueue.hpp"
#include "scanner/detail/Producer.hpp"
#include "scanner/detail/Worker.hpp"

#include <thread>

namespace scanner {

    ScanConfig Scanner::sanitize_config(ScanConfig config) {
        if (config.threads == 0)
            config.threads = std::thread::hardware_concurrency();
        if (config.queue_capacity == 0)
            config.queue_capacity = 1024;
        if (config.chunk_size == 0)
            config.chunk_size = 1 << 20;
        return config;
    }

    Scanner::Scanner(ScanConfig config) 
        : config_(sanitize_config(std::move(config))){}

    void Scanner::scan(const std::filesystem::path& root,
                        const Store& store,
                        Logger* logger,
                        ScanStats& stats) const {
    
        std::error_code err;
        const auto status = std::filesystem::status(root, err);
        if (err || !std::filesystem::exists(status) || !std::filesystem::is_directory(status)) {
            throw std::runtime_error("scan root is not a directory: " + root.generic_string());
        }

        BoundedQueue<std::filesystem::path> queue(config_.queue_capacity);

        detail::Producer producer(root, config_, queue, stats);
        std::jthread producer_thread([&] {
            producer.run();
        });

        std::vector<std::jthread> workers;
        workers.reserve(config_.threads);
        for (std::size_t i = 0; i < config_.threads; ++i) {
            workers.emplace_back([&, logger] {
                detail::Worker(config_, queue, store, logger, stats).run();
            });
        }
    }
} // namespace scanner