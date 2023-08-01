#include "dida/detail/vertical_decomposition.hpp"

namespace dida::detail::vertical_decomposition
{

ChainDecomposition merge_chain_decompositions(VerticesView vertices, NodePool& node_pool, const ChainDecomposition& a,
                                              const ChainDecomposition& b);
}