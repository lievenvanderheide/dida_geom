target_sources(dida PRIVATE
    divide_and_conquer_builder.cpp
    divide_and_conquer_builder.hpp
    intersect.cpp
    intersect.hpp
    merge.cpp
    merge.hpp
    sweep_line_builder.cpp
    sweep_line_builder.hpp
    triangulate.cpp
    triangulate.hpp
    vertical_decomposition.hpp
    vertical_decomposition.inl
    zigzag_phase.cpp
    zigzag_phase.hpp)

add_subdirectory(tests)