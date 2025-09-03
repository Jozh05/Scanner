#pragma once

#include <cstddef>
#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>

#include <rapidcsv.h>

namespace scanner {

class Store {
public:
    
    enum class DuplicatePolicy {
        ErrorOnConflict,
        KeepFirst,
        KeepLast,
        AlwaysError
    };

    void load(const std::filesystem::path& path, 
                DuplicatePolicy dupPolicy = DuplicatePolicy::ErrorOnConflict);

    std::optional<std::string> get(std::string_view hash) const;

    size_t size() const noexcept {
        return data_.size();
    }

private:
    
    static bool is_hex32(std::string_view str) noexcept;
    static void to_lower(std::string& str) noexcept;

    static void normalize_hash(std::string& hash, size_t line_num);

    void insert(std::string hash, 
                std::string verdict,
                DuplicatePolicy dupPolicy, 
                size_t line_num);

    std::unordered_map<std::string, std::string> data_;
};

} // namespace scanner