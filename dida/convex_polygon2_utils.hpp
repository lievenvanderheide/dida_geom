#pragma once

#include "dida/convex_polygon2.hpp"

namespace dida
{

/// Returns the leftmost vertex of @c polygon. The leftmost vertex is the vertex which according to @c lex_less_than is
/// less than all other vertices.
///
/// @param polygon The polygon.
/// @return An iterator pointing to the leftmost vertex of @c polygon.
ConvexPolygonView2::const_iterator leftmost_vertex(ConvexPolygonView2 polygon);

/// Returns the rightmost vertex of @c polygon. The rightmost vertex is the vertex which according to @c
/// lex_greater_than is greater than all other vertices.
///
/// @param polygon The polygon.
/// @return An iterator pointing to the right vertex of @c polygon.
ConvexPolygonView2::const_iterator rightmost_vertex(ConvexPolygonView2 polygon);

} // namespace dida