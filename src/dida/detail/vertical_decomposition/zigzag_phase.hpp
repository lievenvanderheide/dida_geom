#include "dida/detail/vertical_decomposition/vertical_decomposition.hpp"

namespace dida::detail::vertical_decomposition
{

/// Decomposes the interior of the polygon formed by @c vertices into a set of chain decompositions, using the "zigzag"
/// algorithm. These chain decompositions can then be merged in a subsequent merge phase to produce the final
/// decomposition of the polygon.
std::vector<ChainDecomposition> interior_zigzag_phase(VerticesView vertices, Winding winding, NodePool& node_pool);

struct ExteriorChainDecompositions
{
    Node* leftmost_node;
    Node* rightmost_node;
    
    std::vector<ChainDecomposition> lower_chain_decompositions;
    std::vector<ChainDecomposition> upper_chain_decompositions;
};

/// Decomposes the exterior of the polygon formed by @c vertices into a set of chain decompositions, using the "zigzag"
/// algorithm. These chain decompositions can then be merged in a subsequent merge phase to produce the final
/// decomposition of the polygon.
ExteriorChainDecompositions exterior_zigzag_phase(VerticesView vertices, Winding winding, NodePool& node_pool);

} // namespace dida::detail::vertical_decomposition