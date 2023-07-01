#pragma once

#include "dida/detail/vertical_decomposition.hpp"
#include "dida/polygon2.hpp"

namespace dida::detail::vertical_decomposition
{

/// Returns whether polygon @c a and @c b intersect.
///
/// @param a The first polygon,
/// @param a_exterior_decomposition The decomposition of the exterior of @c a.
/// @param b The second polygon.
bool polygons_intersect(PolygonView2 a, const VerticalDecomposition& a_exterior_decomposition, PolygonView2 b);

}