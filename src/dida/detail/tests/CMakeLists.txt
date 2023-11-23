add_executable(dida_detail_tests
    convex_polygons_intersection_tests.cpp
    polygon_sweep_tests.cpp
    tmp_object_pool_tests.cpp)

target_link_libraries(dida_detail_tests PRIVATE dida Catch2::Catch2WithMain)

get_target_property(DIDA_INCLUDE_DIRS dida INCLUDE_DIRECTORIES)
target_include_directories(dida_detail_tests PRIVATE ${DIDA_INCLUDE_DIRS})

catch_discover_tests(dida_detail_tests)