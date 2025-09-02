#include <openssl/evp.h>
#include <openssl/md5.h>

#include <cstddef>
#include <array>
#include <string>
#include <filesystem>

class Hasher {

public:
    explicit Hasher(std::size_t chunk_size = (1u << 20));
    
    ~Hasher();

    Hasher(const Hasher&) = delete;
    Hasher& operator = (const Hasher&) = delete;

    Hasher(Hasher&&) noexcept;
    Hasher& operator = (Hasher&&) noexcept;

    void reset();
    void update(const void* data, size_t len);

    std::array<unsigned char, 16> finish_raw();
    std::string finish_hex();

    std::string md5_file(const std::filesystem::path&);

    std::size_t chunk_size() const noexcept {
        return chunk_size_;
    }

    void set_chunk_size(std::size_t bytes) {
        chunk_size_ = bytes ? bytes : (1u<<20);
    }

private:
    
    std::string to_hex(const unsigned char* data, size_t len);

    EVP_MD_CTX* ctx_ = nullptr;
    std::size_t chunk_size_;
    bool inited_ = false;
};