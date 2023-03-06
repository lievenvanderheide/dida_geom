cmake_minimum_required(VERSION 3.16.3)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(dida_viz
    main_window.cpp
    main_window.hpp
    main.cpp
    primitives_view.cpp
    primitives_view.hpp
    primitives_tree_view.hpp
    primitives_tree_view.cpp
    scene.cpp
    scene.hpp)

target_link_libraries(dida_viz PRIVATE dida Qt6::Widgets)