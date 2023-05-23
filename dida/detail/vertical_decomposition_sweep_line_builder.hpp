#pragma once

#include "dida/detail/vertical_decomposition.hpp"

namespace dida::detail::vertical_decomposition
{

/// The type of vertical decomposition to compute.
enum class VerticalDecompositionType
{
  /// The decomposition of the interior of a polygon.
  interior_decomposition,

  /// The decomposition of the exterior of a polygon.
  exterior_decomposition,
};

/// Computes the vertical decomposition of the given region of the polygon formed by @c vertices.
///
/// @param vertices The vertices of the polygon.
/// @param decomposition_type The type of the vertical decomposition.
/// @return The decomposition.
VerticalDecomposition vertical_decomposition_with_sweep_line_builder(ArrayView<const Point2> vertices,
                                                                     VerticalDecompositionType decomposition_type);

} // namespace dida::detail::vertical_decomposition