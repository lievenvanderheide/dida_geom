#pragma once

#include "dida/detail/vertical_decomposition/vertical_decomposition.hpp"
#include "dida/polygon2.hpp"

namespace dida::detail::vertical_decomposition
{

/// Returns whether two polygons intersect, while taking using their exterior vertical decompositions to speed-up these
/// tests.
bool intersect(PolygonView2 a, const VerticalDecomposition& a_exterior_decomposition, PolygonView2 b,
               const VerticalDecomposition& b_exterior_decomposition);

} // namespace dida::detail::vertical_decomposition