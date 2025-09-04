add_executable(scanner_cli
        src/main.cpp
        src/Parser.cpp
        src/App.cpp)

target_include_directories(scanner_cli PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(scanner_cli PRIVATE scanner)
target_compile_features(scanner_cli PRIVATE cxx_std_20)