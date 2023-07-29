cmake_minimum_required(VERSION 3.16.3)

find_package(Catch2 2 REQUIRED)

add_executable(dida_detail_tests
    convex_polygons_intersection_tests.cpp
    polygon_sweep_tests.cpp
    tmp_object_pool_tests.cpp
    vertical_decomposition_zigzag_phase_tests.cpp
    vertical_decomposition_sweep_line_builder_tests.cpp
    vertical_decomposition_test_utils_tests.cpp
    vertical_decomposition_test_utils.cpp
    vertical_decomposition_test_utils.hpp
    vertical_decomposition_tests.cpp)

target_link_libraries(dida_detail_tests PRIVATE dida Catch2::Catch2WithMain)