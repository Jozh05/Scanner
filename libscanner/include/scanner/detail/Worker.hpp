#pragma once

#include "scanner/detail/BoundedQueue.hpp"
#include "scanner/Types.hpp"
#include "scanner/Logger.hpp"
#include "scanner/Store.hpp"

namespace scanner::detail {

class Worker {
public:  
    Worker (const ScanConfig& config,
            BoundedQueue<std::filesystem::path>& queue,
            const Store& store,
            Logger* logger,
            ScanStats& stats);
    

    void run();
private:
    const ScanConfig& config_;
    BoundedQueue<std::filesystem::path>& queue_;
    const Store& store_;
    Logger* logger_ = nullptr;
    ScanStats& stats_;
};


} // namespcae scanner::detail