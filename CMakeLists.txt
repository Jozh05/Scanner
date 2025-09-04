cmake_minimum_required(VERSION 3.21)
project(scanner LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)


add_subdirectory(libscanner)
add_subdirectory(scanner_cli)
#add_subdirectory(tests)