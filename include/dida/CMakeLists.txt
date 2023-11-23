set(DIDA_PUBLIC_INCLUDE_FILES
    area.hpp
    array_view.hpp
    array_view.inl
    box2.hpp
    box2.inl
    assert.hpp
    convex_polygon2_utils.hpp
    convex_polygon2.hpp
    convex_polygon2.inl
    parser.hpp
    parser.inl
    perturbation.hpp
    perturbation.inl
    point2.hpp
    point2.inl
    polygon2_utils.hpp
    polygon2_utils.inl
    polygon2.hpp
    polygon2.inl
    predicates.hpp
    scalar.hpp
    scalar.inl
    segment2.hpp
    segment2.inl
    vector2.hpp
    utils.hpp
    utils.inl
    vector2.inl)

target_sources(dida PUBLIC ${DIDA_PUBLIC_INCLUDE_FILES})

install(FILES ${DIDA_PUBLIC_INCLUDE_FILES} DESTINATION include/dida)

add_subdirectory(math)