cmake_minimum_required(VERSION 3.16.3)

find_package(Catch2 2 REQUIRED)

add_executable(dida_tests
    perturbation_tests.cpp
    point2_tests.cpp
    scalar_tests.cpp
    segment2_tests.cpp
    vector2_tests.cpp)

target_link_libraries(dida_tests PRIVATE dida Catch2::Catch2WithMain)
