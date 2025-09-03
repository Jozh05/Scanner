#include <scanner/Store.hpp>


#include <algorithm>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << (argc ? argv[0] : "scanner_cli") << " <base.csv>\n";
        return 2;
    }

    try {
        const std::filesystem::path csvPath = argv[1];

        scanner::Store store;
        store.load(csvPath);  // по умолчанию DuplicatePolicy::ErrorOnConflict

        // Соберём все ключи (MD5) и отсортируем для стабильного вывода.
        std::vector<std::string> keys;
        keys.reserve(store.size());
        for (const auto& kv : store.data()) {    // data() — const ref к внутренней карте
            keys.push_back(kv.first);
        }
        std::sort(keys.begin(), keys.end());

        // Печатаем через get(), как просили: md5;verdict
        for (const auto& md5 : keys) {
            if (auto verdict = store.get(md5)) {
                std::cout << md5 << ';' << *verdict << '\n';
            } else {
                // Теоретически не должно случиться, но на всякий оставим защиту
                std::cerr << "[WARN] missing verdict for key: " << md5 << '\n';
            }
        }

        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << '\n';
        return 1;
    }
}
