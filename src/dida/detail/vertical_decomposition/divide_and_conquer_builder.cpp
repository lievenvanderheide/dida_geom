#include "dida/detail/vertical_decomposition/divide_and_conquer_builder.hpp"

#include "dida/detail/vertical_decomposition/merge.hpp"
#include "dida/detail/vertical_decomposition/zigzag_phase.hpp"

namespace dida::detail::vertical_decomposition
{

ChainDecomposition merge_chain_decompositions_rec(VerticesView vertices, Winding winding, NodePool& node_pool,
                                                  ChainDecomposition* chain_decompositions,
                                                  size_t num_chain_decompositions)
{
  if (num_chain_decompositions == 1)
  {
    return chain_decompositions[0];
  }
  else
  {
    size_t mid = num_chain_decompositions / 2;
    ChainDecomposition a = merge_chain_decompositions_rec(vertices, winding, node_pool, chain_decompositions, mid);
    ChainDecomposition b = merge_chain_decompositions_rec(vertices, winding, node_pool, chain_decompositions + mid,
                                                          num_chain_decompositions - mid);
    return merge_chain_decompositions(vertices, winding, node_pool, a, b);
  }
}

Node* interior_decomposition_with_divide_and_conquer_builder(VerticesView vertices, NodePool& node_pool)
{
  std::vector<ChainDecomposition> chain_decompositions = interior_zigzag_phase(vertices, Winding::ccw, node_pool);

  ChainDecomposition merged = merge_chain_decompositions_rec(vertices, Winding::ccw, node_pool,
                                                             chain_decompositions.data(), chain_decompositions.size());
  return merged.first_node;
}

ExteriorDecomposition exterior_decomposition_with_divide_and_conquer_builder(VerticesView vertices, NodePool& node_pool)
{
  ExteriorChainDecompositions chain_decompositions = exterior_zigzag_phase(vertices, Winding::cw, node_pool);

  merge_chain_decompositions_rec(vertices, Winding::cw, node_pool,
                                 chain_decompositions.lower_chain_decompositions.data(),
                                 chain_decompositions.lower_chain_decompositions.size());

  merge_chain_decompositions_rec(vertices, Winding::cw, node_pool,
                                 chain_decompositions.upper_chain_decompositions.data(),
                                 chain_decompositions.upper_chain_decompositions.size());

  return ExteriorDecomposition{
      chain_decompositions.leftmost_node,
      chain_decompositions.rightmost_node,
  };
}

} // namespace dida::detail::vertical_decomposition