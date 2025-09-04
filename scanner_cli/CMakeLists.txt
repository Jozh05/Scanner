add_executable(scanner_cli
        src/main.cpp
        src/Parser.cpp
        src/App.cpp)

if (WIN32)
  find_package(OpenSSL REQUIRED)
  target_link_libraries(scanner_cli PRIVATE OpenSSL::Crypto)

  add_custom_command(TARGET scanner_cli POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:scanner>
            $<TARGET_FILE_DIR:scanner_cli>)

  add_custom_command(TARGET scanner_cli POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_RUNTIME_DLLS:scanner_cli>
            $<TARGET_FILE_DIR:scanner_cli>
    COMMAND_EXPAND_LISTS)
endif()

target_include_directories(scanner_cli PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(scanner_cli PRIVATE scanner)
target_compile_features(scanner_cli PRIVATE cxx_std_20)