cmake_minimum_required(VERSION 3.16.3)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

add_library(dida_viz_lib
    main_window.cpp
    main_window.hpp
    scene_tree_view.cpp
    scene_tree_view.hpp
    scene.cpp
    scene.hpp
    scene.inl)

target_link_libraries(dida_viz_lib PRIVATE dida Qt::Core Qt6::Widgets)

add_executable(dida_viz
    main.cpp)

target_link_libraries(dida_viz PRIVATE dida_viz_lib dida Qt6::Widgets)

add_subdirectory(tests)