cmake_minimum_required(VERSION 3.16.3)

find_package(Catch2 2 REQUIRED)

add_executable(is_within_benchmark
    is_within_benchmark.cpp
    stock_objects.cpp
    stock_objects.hpp)
target_link_libraries(is_within_benchmark PRIVATE dida Catch2::Catch2)

add_executable(convex_polygons_intersection_area_benchmark convex_polygons_intersection_area_benchmark.cpp)
target_link_libraries(convex_polygons_intersection_area_benchmark PRIVATE dida Catch2::Catch2)

add_executable(polygons_intersect_shootout polygons_intersect_shootout.cpp)
target_link_libraries(polygons_intersect_shootout PRIVATE dida Catch2::Catch2 geos_c)

add_executable(side_vertices_benchmark side_vertices_benchmark.cpp)
target_link_libraries(side_vertices_benchmark PRIVATE dida Catch2::Catch2)
