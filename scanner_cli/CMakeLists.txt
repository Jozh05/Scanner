add_executable(scanner_cli src/main.cpp)

target_link_libraries(scanner_cli PRIVATE scanner)
target_compile_features(scanner_cli PRIVATE cxx_std_20)