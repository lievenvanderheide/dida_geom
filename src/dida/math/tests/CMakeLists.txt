add_executable(dida_math_tests
    big_int_utils_tests.cpp
    fraction_tests.cpp
    int128_tests.cpp)

target_link_libraries(dida_math_tests PRIVATE dida Catch2::Catch2WithMain)
catch_discover_tests(dida_math_tests)