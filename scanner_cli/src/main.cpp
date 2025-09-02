#include <scanner/Hasher.hpp>

#include <filesystem>
#include <iostream>

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << (argc > 0 ? argv[0] : "hasher_cli") << " <file>\n";
        return 2;
    }
    try {
        Hasher h;
        std::cout << h.md5_file(std::filesystem::path(argv[1])) << std::endl;
        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << "\n";
        return 1;
    }
}