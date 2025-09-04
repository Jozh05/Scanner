#include "scanner/Store.hpp"

#include <rapidcsv.h>

#include <vector>



namespace scanner {

    void Store::to_lower(std::string& str) noexcept {
        for (char& ch : str) {
            if (ch >= 'A' && ch <= 'Z')
                ch = static_cast<char>(ch - 'A' + 'a');
        }
    }



    bool Store::is_hex32(std::string_view str) noexcept {
        if (str.size() != 32)
            return false;
        
        for (unsigned char ch : str) {
            const bool is_digit = (ch >= '0' && ch <= '9');
            const bool is_hex_lower = (ch >= 'a' && ch <= 'f');
            const bool is_hex_upper = (ch >= 'A' && ch <= 'F');
            if (!(is_digit || is_hex_lower || is_hex_upper))
                return false;
        }
        return true;
    }



    void Store::normalize_hash(std::string& hash, size_t line_num) {
        if (!is_hex32(hash))
                throw std::runtime_error("CSV format error at line " + std::to_string(line_num + 1) +
                                     ": MD5 must be exactly 32 hex characters");

        to_lower(hash);
    }

    

    void Store::insert(std::string hash, 
        std::string verdict,
        DuplicatePolicy dupPolicy, 
        size_t line_num) {
        
        auto it = data_.find(hash);
        if (it == data_.end()) {
            data_.emplace(std::move(hash), std::move(verdict));
            return;
        }
        
        switch (dupPolicy) {
            case DuplicatePolicy::ErrorOnConflict:
                if (it->second != verdict)
                    throw std::runtime_error("CSV duplicate conflict at line " + std::to_string(line_num + 1) +
                                     ": MD5 '" + hash + "' has different verdicts");
                break;
            case DuplicatePolicy::KeepFirst:
                break;
            case DuplicatePolicy::KeepLast:
                it->second = std::move(verdict);
                break;
            case DuplicatePolicy::AlwaysError:
                throw std::runtime_error("CSV duplicate at line " + std::to_string(line_num + 1) +
                                 ": MD5 '" + hash + "' appears more than once");
        }
        
    }

    void Store::preflight_csv(const std::filesystem::path& csv) const {
        std::error_code ec;
        std::filesystem::file_status st = std::filesystem::status(csv, ec);
        if (ec) {
            throw std::runtime_error("cannot access base CSV \"" + csv.generic_string() + "\": " + ec.message());
        }
        if (!std::filesystem::exists(st)) {
            throw std::runtime_error("base CSV not found: \"" + csv.generic_string() + "\"");
        }
        if (!std::filesystem::is_regular_file(st)) {
            throw std::runtime_error("base CSV is not a regular file: \"" + csv.generic_string() + "\"");
        }
    
        std::ifstream probe(csv, std::ios::in | std::ios::binary);
        if (!probe.good()) {
            throw std::runtime_error("cannot open base CSV for reading: \"" + csv.generic_string() + "\"");
        }
    }

    void Store::load(const std::filesystem::path& path, DuplicatePolicy dupPolicy) {
        
        preflight_csv(path);
        
        data_.clear();

        rapidcsv::Document doc(
            path.generic_string(),
            rapidcsv::LabelParams(-1, -1),
            rapidcsv::SeparatorParams(';', /*trim*/ true),
            rapidcsv::ConverterParams(),
            rapidcsv::LineReaderParams(/*skipComments=*/false, /*commentPrefix=*/'#', /*skipEmpty=*/true)
        );

        const size_t rows = doc.GetRowCount();
        for (size_t r = 0; r < rows; ++r) {
            std::vector<std::string> row = doc.GetRow<std::string>(r);
            
            if (row.size() != 2)
                throw std::runtime_error("CSV format error at line " + std::to_string(r + 1) +
                                     ": expected exactly 2 columns (md5;verdict)");
            
            std::string hash = std::move(row[0]);
            std::string verdict = std::move(row[1]);

            normalize_hash(hash, r);
            insert(std::move(hash), std::move(verdict), dupPolicy, r);
        }
    }


    std::optional<std::string> Store::get(std::string_view hash) const {
        if (hash.size() != 32)
            return std::nullopt;
        std::string key(hash);
        to_lower(key);
        auto it = data_.find(key);
        if (it == data_.end())
            return std::nullopt;
        return it->second;
    }
} // namespace scanner