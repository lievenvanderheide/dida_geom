#include "dida/detail/vertical_decomposition/vertical_decomposition.hpp"

namespace dida::detail::vertical_decomposition
{

/// Decomposes the polygon formed by @c vertices into a set of chain decompositions, using the "zigzag" algorithm. These
/// chain decompositions can then be merged in a subsequent merge phase to produce the final decomposition of the
/// polygon.
///
/// @param vertices The vertices of the polygon.
/// @param node_pool The pool used to allocate the nodes in the resulting chain decompositions.
/// @return The chain decompositions.
std::vector<ChainDecomposition> vertical_decomposition_zigzag_phase(VerticesView vertices, NodePool& node_pool);

} // namespace dida::detail::vertical_decomposition