#pragma once

#include "dida/detail/vertical_decomposition/vertical_decomposition.hpp"

namespace dida::detail::vertical_decomposition
{

/// Computes the vertical decomposition of the given region of the polygon formed by @c vertices.
///
/// @param vertices The vertices of the polygon.
/// @param decomposition_type The type of the vertical decomposition.
/// @return The decomposition.
VerticalDecomposition vertical_decomposition_with_sweep_line_builder(VerticesView vertices,
                                                                     VerticalDecompositionType decomposition_type);

} // namespace dida::detail::vertical_decomposition