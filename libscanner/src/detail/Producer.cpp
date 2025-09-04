#include "scanner/detail/Producer.hpp"
#include <system_error>

namespace scanner::detail {

    Producer::Producer(const std::filesystem::path& root,
            const ScanConfig& config,
            BoundedQueue<std::filesystem::path>& queue,
            ScanStats& stats)
        : root_(root), config_(config), queue_(queue), stats_(stats){}



    
    void Producer::process_entry(const std::filesystem::directory_entry& dir_entry) {
        std::error_code err;
        const bool is_file = dir_entry.is_regular_file(err);
        
        if (err) {
            stats_.on_error();
            return;
        }
        if (!is_file)
            return;
        
        auto path = std::filesystem::absolute(dir_entry.path());
        queue_.push(path);

    }


    void Producer::traverse() {
        const auto opts = 
            (config_.follow_symlinks ? 
            std::filesystem::directory_options::follow_directory_symlink
            : std::filesystem::directory_options::none);
        
        std::error_code err;

        auto loop = [&](auto& it, const auto& end) {
            for (; it != end; it.increment(err)) {
                if (err) {
                    stats_.on_error();
                    err.clear();
                    continue;
                }
                process_entry(*it);
            }
        };

        if (config_.recursive) {
            std::filesystem::recursive_directory_iterator it(root_, opts, err), end;
            if (err) {
                stats_.on_error();
                return;
            }
            loop(it, end);
        } else {
            std::filesystem::directory_iterator it(root_, opts, err), end;
            if (err) {
                stats_.on_error();
                return;
            }
            loop(it, end);
        }
    }

    void Producer::run() {
        try {
            traverse();
        } catch (...) {
            stats_.on_error();
        }
        queue_.close();
    }
} //namespace scanner::detail