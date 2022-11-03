cmake_minimum_required(VERSION 3.16.3)

find_package(Catch2 2 REQUIRED)

add_executable(side_vertices_benchmark side_vertices_benchmark.cpp)
target_link_libraries(side_vertices_benchmark PRIVATE dida Catch2::Catch2)
