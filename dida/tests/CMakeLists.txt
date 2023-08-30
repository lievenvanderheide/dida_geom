cmake_minimum_required(VERSION 3.16.3)

find_package(Catch2 2 REQUIRED)

add_executable(dida_tests
    area_tests.cpp
    array_view_tests.cpp
    box2_tests.cpp
    convex_polygon2_tests.cpp
    convex_polygon2_utils_tests.cpp
    parser_tests.cpp
    perturbation_tests.cpp
    point2_tests.cpp
    polygon2_tests.cpp
    polygon2_utils_tests.cpp
    predicates_tests.cpp
    scalar_tests.cpp
    segment2_tests.cpp
    utils_tests.cpp
    vector2_tests.cpp)

target_link_libraries(dida_tests PRIVATE dida Catch2::Catch2WithMain)
