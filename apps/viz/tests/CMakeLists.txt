find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

add_executable(dida_viz_tests
    scene_selection_tests.cpp
    scene_tests.cpp
    scene_tree_view_tests.cpp)

target_link_libraries(dida_viz_tests PRIVATE dida_viz_lib dida Catch2::Catch2WithMain Qt6::Core)
target_include_directories(dida_viz_tests PRIVATE)
catch_discover_tests(dida_viz_tests)