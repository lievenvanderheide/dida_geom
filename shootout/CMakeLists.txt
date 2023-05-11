cmake_minimum_required(VERSION 3.16.3)
project(shootout)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-O3 -DNDEBUG")

find_package(Catch2 2 REQUIRED)
#find_package(Boost COMPONENTS geometry REQUIRED)

add_executable(shootout
    polygon_intersection_shootout.cpp)

target_link_libraries(shootout PRIVATE Catch2::Catch2WithMain geos_c)