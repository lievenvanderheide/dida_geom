#pragma once

#include "dida/detail/vertical_decomposition.hpp"

namespace dida::detail::vertical_decomposition
{

/// The region to compute the decomposition for.
enum class VerticalDecompositionRegion
{
  /// The interior of the polygon.
  interior,

  /// The exterior of the polygon.
  exterior,
};

/// Computes the vertical decomposition of the given region of the polygon formed by @c vertices.
///
/// @param vertices The vertices of the polygon.
/// @param region The region to decompose.
/// @return The decomposition.
VerticalDecomposition vertical_decomposition_with_sweep_line_builder(ArrayView<const Point2> vertices,
                                                                     VerticalDecompositionRegion region);

} // namespace dida::detail::vertical_decomposition