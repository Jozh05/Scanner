
find_package(OpenSSL REQUIRED)

add_library(scanner SHARED src/Hasher.cpp src/Store.cpp)

find_path(RAPIDCSV_INCLUDE_DIRS "rapidcsv.h")
target_include_directories(scanner PRIVATE ${RAPIDCSV_INCLUDE_DIRS})
target_include_directories(scanner PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(scanner PUBLIC OpenSSL::Crypto)
target_compile_features(scanner PUBLIC cxx_std_20)