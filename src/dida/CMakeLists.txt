target_sources(dida PRIVATE
    area.cpp
    box2.cpp
    convex_polygon2_utils.cpp
    convex_polygon2.cpp
    parser.cpp
    polygon2_utils.cpp
    polygon2.cpp
    predicates.cpp
    scalar.cpp
    segment2.cpp
    vector2.cpp)

add_subdirectory(detail)
add_subdirectory(math)
add_subdirectory(tests)