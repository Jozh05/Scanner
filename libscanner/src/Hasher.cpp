#include "scanner/Hasher.hpp"

#include <openssl/evp.h>
#include <openssl/md5.h>

#include <stdexcept>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <vector>

namespace scanner {

Hasher::Hasher(size_t chunk_size) : chunk_size_(chunk_size ? chunk_size : (1u << 20)) {
    ctx_ = EVP_MD_CTX_new();
    if (!ctx_)
        throw std::runtime_error("Creation context failed");
    reset();
}



Hasher::~Hasher() {
    if (ctx_) {
        EVP_MD_CTX_free(ctx_);
        ctx_ = nullptr;
    }
}



Hasher::Hasher(Hasher&& other) noexcept : ctx_(other.ctx_), chunk_size_(other.chunk_size_), inited_(other.inited_){
    other.ctx_ = nullptr;
    other.inited_ = false;
}



Hasher& Hasher::operator=(Hasher&& other) noexcept {
    if (this != &other) {
        ctx_ = other.ctx_;
        chunk_size_ = other.chunk_size_;
        inited_ = other.inited_;
        other.inited_ = false;
        other.ctx_ = nullptr;
    }
    return *this;
}



void Hasher::reset() {
    if (EVP_DigestInit_ex(ctx_, EVP_md5(), nullptr) != 1)
        throw std::runtime_error("EVP_DigestInit_ex failed");
    inited_ = true;
}



void Hasher::update(const void* data, size_t len) {
    if (!inited_)
        reset();
    if (len == 0)
        return;
    if (EVP_DigestUpdate(ctx_, data, len) != 1)
        throw std::runtime_error("Update failed");
}



std::array<unsigned char, 16> Hasher::finish_raw() {
    if (!inited_)
        reset();
    std::array<unsigned char, 16> out;
    unsigned int md_len;

    if (EVP_DigestFinal_ex(ctx_, out.data(), &md_len) != 1 || md_len != out.size())
        throw std::runtime_error("EVP_DigestFinal_ex failed");
    
    inited_ = false;
    return out;
}


std::string Hasher::finish_hex() {
    auto raw = finish_raw();
    return to_hex(raw.data(), raw.size());
}



std::string Hasher::md5_file(const std::filesystem::path& path) {
    std::ifstream in(path, std::ios::binary);
    if (!in) 
        throw std::runtime_error("cannot open file: " + path.generic_string());
    
    reset();
    std::vector<unsigned char> buf(chunk_size_);
    while (in) {
        in.read(reinterpret_cast<char*>(buf.data()),
            static_cast<std::streamsize>(buf.size()));
        std::streamsize got = in.gcount();
        if (got > 0)
            update(buf.data(), static_cast<size_t>(got));
    }

    if (!in.eof() && in.fail())
        throw std::runtime_error("I/O error during read: " + path.generic_string());
    
    return finish_hex();
}



std::string Hasher::to_hex(const unsigned char* data, size_t len) {
    std::ostringstream oss;
    oss << std::hex << std::setfill('0');
    for (size_t i = 0; i < len; ++i)
        oss << std::setw(2) << static_cast<int>(data[i]);
    return oss.str();
}

} // namespace scanner