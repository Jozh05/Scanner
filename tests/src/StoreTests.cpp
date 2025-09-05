#include <gtest/gtest.h>
#include "TestUtils.hpp"
#include <scanner/Store.hpp>


TEST(Store, LoadTrimAndLowercase) {
    testutil::TempDir tmp;
    auto csv = tmp.path("base.csv");
    testutil::write_text(csv,
        "D41D8CD98F00B204E9800998ECF8427E ;  clean\n"
        "900150983CD24FB0D6963F7D28E17F72 ;  malware\n");

    scanner::Store s;
    s.load(csv, scanner::Store::DuplicatePolicy::KeepLast);
    EXPECT_EQ(s.size(), 2u);

    auto v1 = s.get("d41d8cd98f00b204e9800998ecf8427e");
    ASSERT_TRUE(v1.has_value());
    EXPECT_EQ(*v1, "clean");

    auto v2 = s.get("900150983cd24fb0d6963f7d28e17f72");
    ASSERT_TRUE(v2.has_value());
    EXPECT_EQ(*v2, "malware");
}

class StoreDuplicatesTest : public ::testing::Test {
protected:
    testutil::TempDir tmp;
    static constexpr char kSep = ';';

    std::filesystem::path csv_conflict() const { return tmp.path("dups_conflict.csv"); }
    std::filesystem::path csv_same() const     { return tmp.path("dups_same.csv"); }

    void write_conflict_csv() const {
        testutil::write_text(csv_conflict(),
            "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" + std::string(1, kSep) + " first\n"
            "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA" + std::string(1, kSep) + " last\n");
    }

    void write_same_csv() const {
        testutil::write_text(csv_same(),
            "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb" + std::string(1, kSep) + " ok\n"
            "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB" + std::string(1, kSep) + " ok\n");
    }
};

TEST_F(StoreDuplicatesTest, KeepLast) {
    write_conflict_csv();

    scanner::Store s;
    s.load(csv_conflict(), scanner::Store::DuplicatePolicy::KeepLast);

    auto v = s.get("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
    ASSERT_TRUE(v.has_value());
    EXPECT_EQ(*v, "last");
}

TEST_F(StoreDuplicatesTest, KeepFirst) {
    write_conflict_csv();

    scanner::Store s;
    s.load(csv_conflict(), scanner::Store::DuplicatePolicy::KeepFirst);

    auto v = s.get("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
    ASSERT_TRUE(v.has_value());
    EXPECT_EQ(*v, "first");
}

TEST_F(StoreDuplicatesTest, ErrorOnConflict) {
    write_conflict_csv();
    {
        scanner::Store s;
        EXPECT_THROW(s.load(csv_conflict(), scanner::Store::DuplicatePolicy::ErrorOnConflict),
                     std::runtime_error);
    }

    write_same_csv();
    {
        scanner::Store s;
        EXPECT_NO_THROW(s.load(csv_same(), scanner::Store::DuplicatePolicy::ErrorOnConflict));
        auto v = s.get("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb");
        ASSERT_TRUE(v.has_value());
        EXPECT_EQ(*v, "ok");
    }
}


TEST_F(StoreDuplicatesTest, AlwaysError) {
    write_same_csv();

    scanner::Store s;
    EXPECT_THROW(s.load(csv_same(), scanner::Store::DuplicatePolicy::AlwaysError),
                 std::runtime_error);
}