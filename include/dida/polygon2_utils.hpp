#pragma once

#include "dida/convex_polygon2.hpp"
#include "dida/polygon2.hpp"

namespace dida
{

/// Flips @c polygon horizontally by negating the x-coordinate of each vertices, and reversing the order of the vertices
/// to account for the inverted winding.
template <class Storage>
void flip_horizontally(Polygon2T<Storage>& polygon);

/// Flips @c polygon vertically by negating the y-coordinate of each vertices, and reversing the order of the vertices
/// to account for the inverted winding.
template <class Storage>
void flip_vertically(Polygon2T<Storage>& polygon);

bool geometrically_equal(PolygonView2 a, PolygonView2 b);

/// Triangulates the given polygon.
std::vector<Triangle2> triangulate(PolygonView2 polygon);

} // namespace dida

#include "dida/polygon2_utils.inl"