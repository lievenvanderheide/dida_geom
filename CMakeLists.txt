cmake_minimum_required(VERSION 3.16.3)
project(dida LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Catch2 3 REQUIRED)

include(CTest)
include(Catch)

add_library(dida)
target_include_directories(dida PRIVATE src PUBLIC include)

add_subdirectory(include)
add_subdirectory(src)

add_subdirectory(benchmarks)
add_subdirectory(apps)