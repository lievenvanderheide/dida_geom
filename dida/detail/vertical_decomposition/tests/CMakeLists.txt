cmake_minimum_required(VERSION 3.16.3)

find_package(Catch2 2 REQUIRED)

add_executable(dida_detail_vertical_decomposition_tests
    merge_tests.cpp
    sweep_line_builder_tests.cpp
    test_utils_tests.cpp
    test_utils.cpp
    test_utils.hpp
    vertical_decomposition_tests.cpp
    zigzag_phase_tests.cpp)

target_link_libraries(dida_detail_vertical_decomposition_tests PRIVATE dida Catch2::Catch2WithMain)