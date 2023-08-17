#include "dida/detail/vertical_decomposition/vertical_decomposition.hpp"

namespace dida::detail::vertical_decomposition
{

/// Merges two chain decompositions.
///
/// The last vertex of @c a must be equal to the first vertex of chain @c b, and this vertex must be a convex side vertex
/// of the combined chain (so the vertex will be the location of a leaf node).
ChainDecomposition merge_chain_decompositions(VerticesView vertices, NodePool& node_pool, const ChainDecomposition& a,
                                              const ChainDecomposition& b);
}