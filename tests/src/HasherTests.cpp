#include <gtest/gtest.h>
#include <scanner/Hasher.hpp>
#include "TestUtils.hpp"

TEST(Hasher, EmptyFile) {
    testutil::TempDir tmp;
    auto f = tmp.path("empty.bin");
    testutil::write_bytes(f, {});
    scanner::Hasher h;
    EXPECT_EQ(h.md5_file(f), "d41d8cd98f00b204e9800998ecf8427e");
}

TEST(Hasher, SmallFile) {
    testutil::TempDir tmp;
    auto f = tmp.path("abc.txt");
    testutil::write_text(f, "abc");
    scanner::Hasher h;
    EXPECT_EQ(h.md5_file(f), "900150983cd24fb0d6963f7d28e17f72");
}
