target_sources(dida PRIVATE
    convex_polygons_intersection.hpp
    convex_polygons_intersection.inl
    polygon_sweep.hpp
    polygon_sweep.inl
    tmp_object_pool.hpp
    tmp_object_pool.inl)

add_subdirectory(tests)
add_subdirectory(vertical_decomposition)