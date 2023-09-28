cmake_minimum_required(VERSION 3.16.3)

add_executable(dida_detail_tests
    convex_polygons_intersection_tests.cpp
    polygon_sweep_tests.cpp
    tmp_object_pool_tests.cpp)

target_link_libraries(dida_detail_tests PRIVATE dida Catch2::Catch2WithMain)