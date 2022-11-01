cmake_minimum_required(VERSION 3.16.3)
project(dida)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(..)

add_library(dida
    math/int128.cpp
    math/int128.hpp
    math/int128.inl
    assert.hpp
    perturbation.hpp
    perturbation.inl
    scalar.cpp
    scalar.hpp
    scalar.inl
    segment2.hpp
    segment2.inl
    vector2.cpp
    vector2.hpp
    vector2.inl)

add_subdirectory(math/tests)
add_subdirectory(tests)
