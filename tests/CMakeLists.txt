enable_testing()

find_package(GTest CONFIG REQUIRED)

add_executable(tests src/HasherTests.cpp
                    src/ScannerLoggerTests.cpp
                    src/StoreTests.cpp)

target_include_directories(tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(tests PRIVATE scanner GTest::gtest GTest::gtest_main)

add_executable(huge_file_test src/HasherLargeFileTest.cpp)

target_include_directories(huge_file_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(huge_file_test PRIVATE scanner GTest::gtest GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(tests
    PROPERTIES LABELS "general"
)
gtest_discover_tests(huge_file_test
    PROPERTIES LABELS "huge_file"
)