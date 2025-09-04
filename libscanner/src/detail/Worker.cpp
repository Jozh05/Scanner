#include "scanner/detail/Worker.hpp"
#include "scanner/Hasher.hpp"

namespace scanner::detail {

    Worker::Worker(const ScanConfig& config,
                BoundedQueue<std::filesystem::path>& queue,
                const Store& store,
                Logger* logger,
                ScanStats& stats)
        : config_(config), queue_(queue), store_(store), logger_(logger), stats_(stats) {}

    
    void Worker::run() {
        scanner::Hasher hasher(config_.chunk_size);

        std::filesystem::path path;
        while (queue_.pop(path)) {       
            try {
                const std::string hash = hasher.md5_file(path);

                stats_.on_file();
                if (auto verdict = store_.get(hash)) {
                    if (logger_) logger_->log_detection(hash, verdict.value(), path);
                    stats_.on_malicious();
                }

            } catch (const std::exception&) {
                stats_.on_error();
            }
        }
    }
} // namespace scanner::detail
