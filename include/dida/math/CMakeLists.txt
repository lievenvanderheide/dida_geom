set(DIDA_MATH_PUBLIC_INCLUDE_FILES
    big_int_utils.hpp
    big_int_utils.inl
    fraction.hpp
    fraction.inl
    int128.hpp
    int128.inl)

target_sources(dida PUBLIC ${DIDA_MATH_PUBLIC_INCLUDE_FILES})

install(FILES ${DIDA_MATH_PUBLIC_INCLUDE_FILES} DESTINATION include/dida/math)