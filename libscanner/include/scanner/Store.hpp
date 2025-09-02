#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>

class Store {

    void load(const std::filesystem::path& path);

    std::optional<std::string> verdict_for(std::string_view hash) const;


};