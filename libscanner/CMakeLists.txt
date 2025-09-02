
find_package(OpenSSL REQUIRED)

add_library(scanner SHARED src/Hasher.cpp)

target_include_directories(scanner PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(scanner PUBLIC OpenSSL::Crypto)
target_compile_features(scanner PUBLIC cxx_std_20)