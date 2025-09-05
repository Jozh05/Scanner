#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <string>
#include <scanner/Scanner.hpp>
#include <scanner/Store.hpp>
#include <scanner/Logger.hpp>
#include <scanner/Hasher.hpp>
#include "TestUtils.hpp"

namespace fs = std::filesystem;

static std::string md5_of(const fs::path& p) {
    scanner::Hasher h;
    return h.md5_file(p);
}

TEST(Scanner, ScansDirectoryAndLogsDetections) {
    testutil::TempDir tmp;

    auto scanDir = tmp.path("scan");
    std::filesystem::create_directories(scanDir);

    auto f_hit  = scanDir / "in_base.txt";
    auto f_miss = scanDir / "other.txt";
    testutil::write_text(f_hit,  "abc");
    testutil::write_text(f_miss, "hello world");

    auto csv = tmp.path("base.csv");
    testutil::write_text(csv, md5_of(f_hit) + ";malware\n");

    scanner::Store store;
    store.load(csv, scanner::Store::DuplicatePolicy::KeepLast);

    auto log = tmp.path("detections.log");
    scanner::Logger logger(log);

    scanner::ScanConfig cfg{};
    scanner::Scanner sc{cfg};

    scanner::ScanStats stats{};
    sc.scan(scanDir, store, &logger, stats);
    logger.flush();

    EXPECT_EQ(stats.files.load(), 2u);
    EXPECT_EQ(stats.malicious.load(), 1u);
    EXPECT_EQ(stats.errors.load(), 0u);

    std::ifstream in(log);
    ASSERT_TRUE(in);
    std::string line;
    ASSERT_TRUE(static_cast<bool>(std::getline(in, line)));
    EXPECT_NE(line.find("malware"), std::string::npos);
    EXPECT_NE(line.find(std::filesystem::absolute(f_hit).generic_string()), std::string::npos);

    std::string extra;
    EXPECT_FALSE(static_cast<bool>(std::getline(in, extra)));
}

TEST(Scanner, OneFileInDirectory) {
    testutil::TempDir tmp;

    auto scanDir = tmp.path("scan");
    std::filesystem::create_directories(scanDir);

    auto f = scanDir / "single.bin";
    testutil::write_text(f, "abc");

    auto csv = tmp.path("base.csv");
    testutil::write_text(csv, md5_of(f) + ";bad\n");

    scanner::Store store;
    store.load(csv, scanner::Store::DuplicatePolicy::KeepLast);

    scanner::ScanConfig cfg{};
    scanner::Scanner sc{cfg};

    scanner::ScanStats stats{};
    sc.scan(scanDir, store, nullptr, stats);

    EXPECT_EQ(stats.files.load(), 1u);
    EXPECT_EQ(stats.malicious.load(), 1u);
    EXPECT_EQ(stats.errors.load(), 0u);
}

TEST(Scanner, EmptyDirectoryNoFilesNoErrors) {
    testutil::TempDir tmp;

    auto csv = tmp.path("base.csv");
    testutil::write_text(csv, "");

    scanner::Store store;
    store.load(csv, scanner::Store::DuplicatePolicy::KeepLast);

    auto scanDir = tmp.path("scan");
    std::filesystem::create_directories(scanDir);

    scanner::ScanConfig cfg{};
    scanner::Scanner sc{cfg};

    scanner::ScanStats stats{};
    sc.scan(scanDir, store, nullptr, stats);

    EXPECT_EQ(stats.files.load(), 0u);
    EXPECT_EQ(stats.malicious.load(), 0u);
    EXPECT_EQ(stats.errors.load(), 0u);
}

TEST(Scanner, RecursesIntoNestedDirectories) {
    testutil::TempDir tmp;

    const auto scanDir = tmp.path("scan");
    ASSERT_TRUE(fs::create_directories(scanDir / "level1" / "level2"));
    ASSERT_TRUE(fs::create_directories(scanDir / "dummy"));

    const auto f_top     = scanDir / "top.txt";
    const auto f_l1      = scanDir / "level1" / "l1.txt";
    const auto f_hit     = scanDir / "level1" / "level2" / "hit.bin";
    const auto f_s1      = scanDir / "dummy" / "s1.txt";
    const auto f_s2      = scanDir / "dummy" / "s2.txt";

    testutil::write_text(f_top, "top");
    testutil::write_text(f_l1,  "l1");
    testutil::write_text(f_hit, "abc");
    testutil::write_text(f_s1,  "s1");
    testutil::write_text(f_s2,  "s2");

    const auto csv = tmp.path("base.csv");
    {
        const auto md5 = md5_of(f_hit);
        testutil::write_text(csv, md5 + ";malware\n");
    }

    scanner::Store store;
    store.load(csv, scanner::Store::DuplicatePolicy::KeepLast);

    const auto logPath = tmp.path("detections.log");
    scanner::Logger logger(logPath);

    scanner::ScanConfig cfg{};
    cfg.recursive = true;
    scanner::Scanner sc{cfg};

    scanner::ScanStats stats{};
    sc.scan(scanDir, store, &logger, stats);
    logger.flush();

    EXPECT_EQ(stats.files.load(), 5u);
    EXPECT_EQ(stats.malicious.load(), 1u);
    EXPECT_EQ(stats.errors.load(), 0u);

    std::ifstream in(logPath);
    ASSERT_TRUE(in) << "cannot open log file";
    std::vector<std::string> lines;
    for (std::string s; std::getline(in, s); )
        if (!s.empty()) lines.push_back(std::move(s));

    ASSERT_EQ(lines.size(), 1u);
    const auto abs_hit = fs::absolute(f_hit).generic_string();
    EXPECT_NE(lines[0].find("malware"), std::string::npos);
    EXPECT_NE(lines[0].find(abs_hit),   std::string::npos);
}