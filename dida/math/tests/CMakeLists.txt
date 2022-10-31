cmake_minimum_required(VERSION 3.16.3)

find_package(Catch2 2 REQUIRED)

add_executable(dida_math_tests
    big_int_utils_tests.cpp
    int128_tests.cpp)

target_link_libraries(dida_math_tests PRIVATE dida Catch2::Catch2WithMain)
