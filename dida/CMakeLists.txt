cmake_minimum_required(VERSION 3.16.3)
project(dida)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(..)

add_library(dida
    detail/convex_polygons_intersection.hpp
    detail/convex_polygons_intersection.inl
    detail/vertical_decomposition_sweep_line_builder.cpp
    detail/vertical_decomposition_sweep_line_builder.hpp
    detail/vertical_decomposition.cpp
    detail/vertical_decomposition.hpp
    detail/vertical_decomposition.inl
    math/big_int_utils.hpp
    math/big_int_utils.inl
    math/fraction.hpp
    math/fraction.inl
    math/int128.cpp
    math/int128.hpp
    math/int128.inl
    area.cpp
    area.hpp
    array_view.hpp
    array_view.inl
    box2.cpp
    box2.hpp
    box2.inl
    assert.hpp
    convex_polygon2_utils.cpp
    convex_polygon2_utils.hpp
    convex_polygon2.cpp
    convex_polygon2.hpp
    convex_polygon2.inl
    parser.cpp
    parser.hpp
    parser.inl
    perturbation.hpp
    perturbation.inl
    scalar.cpp
    scalar.hpp
    scalar.inl
    segment2.cpp
    segment2.hpp
    segment2.inl
    vector2.cpp
    vector2.hpp
    vector2.inl)

add_subdirectory(benchmarks)
add_subdirectory(detail/tests)
add_subdirectory(math/tests)
add_subdirectory(tests)
add_subdirectory(viz)