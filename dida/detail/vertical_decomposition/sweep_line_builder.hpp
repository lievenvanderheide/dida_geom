#pragma once

#include "dida/detail/vertical_decomposition/vertical_decomposition.hpp"

namespace dida::detail::vertical_decomposition
{

/// Computes the vertical decomposition of the given region of the polygon formed by @c vertices, using a sweep line
/// based algorithm.
///
/// A consequence of the fact that this function uses a sweep line based algorithm is that the nodes in the resulting @c VerticalDecomposition
/// occur from left to right (that is, they're ordered using @c lex_less_than on the node's vertex).
VerticalDecomposition vertical_decomposition_with_sweep_line_builder(VerticesView vertices,
                                                                     VerticalDecompositionType decomposition_type);

} // namespace dida::detail::vertical_decomposition