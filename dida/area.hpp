#pragma once

#include "dida/convex_polygon2.hpp"

namespace dida
{

/// Returns the area of @c polygon.
///
/// @param polygon The polygon.
/// @return The area of @c polygon.
double area(ConvexPolygonView2 polygon);

/// Returns the area of the intersection of polygons @c a and @c b.
///
/// @param a The first polygon.
/// @param b The second polygon.
/// @return The area of the intersection of polygons @c a and @c b.
double intersection_area(ConvexPolygonView2 a, ConvexPolygonView2 b);

} // namespace dida