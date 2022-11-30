#pragma once

#include "dida/convex_polygon2.hpp"

namespace dida
{

/// Returns the area of @c polygon.
///
/// @param polygon The polygon.
/// @return The area of @c polygon.
double area(ConvexPolygonView2 polygon);

}