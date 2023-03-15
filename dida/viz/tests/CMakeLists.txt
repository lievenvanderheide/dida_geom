cmake_minimum_required(VERSION 3.16.3)

find_package(Catch2 2 REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

add_executable(dida_viz_tests
    primitives_tree_view_tests.cpp
    scene_tests.cpp)

target_link_libraries(dida_viz_tests PRIVATE dida_viz_lib dida Catch2::Catch2WithMain Qt6::Core)
