set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

add_library(dida_viz_lib
    main_window.cpp
    main_window.hpp
    scene_selection.cpp
    scene_selection.hpp
    scene_tree_view.cpp
    scene_tree_view.hpp
    scene_view.cpp
    scene_view.hpp
    scene.cpp
    scene.hpp
    scene.inl
    zoom_pan_view.cpp
    zoom_pan_view.hpp)

target_link_libraries(dida_viz_lib PRIVATE dida Qt::Core Qt6::Widgets)

add_executable(dida_viz
    main.cpp)

target_link_libraries(dida_viz PRIVATE dida_viz_lib dida Qt6::Widgets)

add_subdirectory(tests)