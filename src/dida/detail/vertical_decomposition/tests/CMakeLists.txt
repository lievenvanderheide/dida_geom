add_executable(dida_detail_vertical_decomposition_tests
    divide_and_conquer_builder_tests.cpp
    intersect_tests.cpp
    merge_tests.cpp
    sweep_line_builder_tests.cpp
    test_utils_tests.cpp
    test_utils.cpp
    test_utils.hpp
    triangulate_tests.cpp
    vertical_decomposition_tests.cpp
    zigzag_phase_tests.cpp)

target_link_libraries(dida_detail_vertical_decomposition_tests PRIVATE dida Catch2::Catch2WithMain)
target_include_directories(dida_detail_vertical_decomposition_tests PRIVATE ${CMAKE_SOURCE_DIR}/src)

catch_discover_tests(dida_detail_vertical_decomposition_tests)