#include "cli/App.hpp"
#include "cli/Parser.hpp"

#include <filesystem>
#include <iostream>
#include <memory>
#include <system_error>
#include <chrono>

#include <scanner/Store.hpp>
#include <scanner/Scanner.hpp>
#include <scanner/Logger.hpp>

namespace cli {

    App::App(Options opts) : opts_(std::move(opts)){}

    std::ostream& App::print_stats(std::ostream& os,
        const scanner::ScanStats& stats,
        std::chrono::milliseconds elapsed) const {
        os << "Base:   " << opts_.base_csv << "\n"
                  << "Path:   " << opts_.root_path << "\n";
        if (opts_.log_path) os << "Log:    " << opts_.log_path.value() << "\n";
        os << "Files:  " << stats.files.load() << "\n"
                  << "Hits:   " << stats.malicious.load() << "\n"
                  << "Errors: " << stats.errors.load() << "\n"
                  << "Time:   ";
        print_duration(os, elapsed);
        os << "\n";
        return os;
    }

    void App::print_duration(std::ostream& os, std::chrono::milliseconds ms) {
        auto total_ms = ms.count();
        auto minutes  = total_ms / 60'000;
        total_ms     %= 60'000;
        auto seconds  = total_ms / 1'000;
        auto millis   = total_ms % 1'000;

        bool wrote = false;
        if (minutes > 0) {
            os << minutes << " min";
            wrote = true;
        }
        if (seconds > 0 || minutes > 0) {
            if (wrote) os << ' ';
            os << seconds << " s";
            wrote = true;
        }
        if (millis > 0 || (!minutes && !seconds)) {
            if (wrote) os << ' ';
            os << millis << " ms";
        }
    }

    int App::run() {
        try {
            scanner::Store store;
            store.load(opts_.base_csv, scanner::Store::DuplicatePolicy::ErrorOnConflict);

            std::unique_ptr<scanner::Logger> logger;
            if (opts_.log_path) {
                logger = std::make_unique<scanner::Logger>(opts_.log_path.value(), 
                                                    scanner::Logger::OpenMode::Truncate);
            }
            
            scanner::ScanConfig cfg{};
            scanner::Scanner scanner(cfg);


            scanner::ScanStats stats;
            const auto t0 = std::chrono::steady_clock::now();
            scanner.scan(opts_.root_path, store, logger.get(), stats);
            const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - t0);

            print_stats(std::cout, stats, elapsed);
            return 0;

        } catch (const std::exception& ex) {
            std::cerr << "fatal: " << ex.what() << "\n";
            return 3;
        }
    }
} // namespace cli