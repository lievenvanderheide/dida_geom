#pragma once

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

} // namespace dida

#include "dida/polygon2_utils.inl"