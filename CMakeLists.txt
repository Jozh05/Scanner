cmake_minimum_required(VERSION 3.15)
project(scanner LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)

enable_testing()

find_package(OpenSSL REQUIRED)
#target_link_libraries(main PRIVATE OpenSSL::SSL)
#target_link_libraries(main PRIVATE OpenSSL::Crypto)