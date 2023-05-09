#pragma once

#include "dida/array_view.hpp"
#include "dida/math/fraction.hpp"
#include "dida/point2.hpp"
#include "dida/segment2.hpp"

namespace dida::detail::vertical_decomposition
{

using VerticesView = ArrayView<const Point2>;
using VertexIt = VerticesView::const_iterator;
using YOnEdge = math::Fraction<ScalarDeg2, ScalarDeg1>;

/// Returns the y-coordinate of the point on @c edge which has x-coordinate @c x.
///
/// @pre @c edge should not be vertical.
/// @param edge The edge.
/// @param x The x-coordinate.
/// @return The y-coordinate of the point with x-coordinate @c x.
YOnEdge y_on_edge_for_x(Segment2 edge, ScalarDeg1 x);

} // namespace dida::detail::vertical_decomposition

#include "dida/detail/vertical_decomposition.hpp"