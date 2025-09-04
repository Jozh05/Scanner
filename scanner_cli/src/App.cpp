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
        const int64_t time) const {
        os << "Base:   " << opts_.base_csv << "\n"
                  << "Path:   " << opts_.root_path << "\n";
        if (opts_.log_path) os << "Log:    " << opts_.log_path.value() << "\n";
        std::cout << "Files:  " << stats.files.load() << "\n"
                  << "Hits:   " << stats.malicious.load() << "\n"
                  << "Errors: " << stats.errors.load() << "\n"
                  << "Time:   " << time << " ms\n";
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
            const auto t1 = std::chrono::steady_clock::now();
            const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();

            print_stats(std::cout, stats, ms);
            return 0;

        } catch (const std::exception& ex) {
            std::cerr << "fatal: " << ex.what() << "\n";
            return 3;
        }
    }
} // namespace cli