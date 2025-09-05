#include <gtest/gtest.h>
#include <scanner/Hasher.hpp>
#include "TestUtils.hpp"

TEST(Hasher, LargeFile) {
    testutil::TempDir tmp;
    auto file = tmp.path("billion_a.bin");

    std::ofstream o(file, std::ios::binary | std::ios::app);
    for (size_t i = 0; i < 100'000'000; ++i) {
        std::vector<char> v(300, 'a');
        o.write(v.data(), static_cast<std::streamsize>(v.size()));
    }
    o.close();
    scanner::Hasher hasher;
    EXPECT_EQ(hasher.md5_file(file), "8b1f964b719f95a0a345fad7417a07d2");
}

// c9fad513774bd938134c288576bd93cc -> 1000000000
// 8b1f964b719f95a0a345fad7417a07d2 -> 30000000000