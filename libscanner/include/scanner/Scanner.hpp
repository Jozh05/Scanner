#pragma once

#include <filesystem>
#include <functional>
#include <vector>

#include "scanner/Types.hpp"
#include "scanner/Store.hpp"
#include "scanner/Logger.hpp"

namespace scanner {
    
class Scanner {
public:
    explicit Scanner(ScanConfig config);

    void scan(const std::filesystem::path& root,
                const Store& store,
                Logger* logger,
                ScanStats& stats) const;
private:
    static ScanConfig sanitize_config(ScanConfig config);

private:
    ScanConfig config_;
};    

} // namespace scanner