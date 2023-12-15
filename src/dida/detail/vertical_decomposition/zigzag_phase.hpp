#include "dida/detail/vertical_decomposition/vertical_decomposition.hpp"

namespace dida::detail::vertical_decomposition
{

/// Decomposes the polygon formed by @c vertices into a set of chain decompositions, using the "zigzag" algorithm. These
/// chain decompositions can then be merged in a subsequent merge phase to produce the final decomposition of the
/// polygon.
std::vector<ChainDecomposition> vertical_decomposition_zigzag_phase(VerticesView vertices, Winding winding,
                                                                    NodePool& node_pool);

} // namespace dida::detail::vertical_decomposition