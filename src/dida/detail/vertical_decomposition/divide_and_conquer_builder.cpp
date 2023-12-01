#include "dida/detail/vertical_decomposition/divide_and_conquer_builder.hpp"

#include "dida/detail/vertical_decomposition/merge.hpp"
#include "dida/detail/vertical_decomposition/zigzag_phase.hpp"

namespace dida::detail::vertical_decomposition
{

ChainDecomposition merge_chain_decompositions_rec(VerticesView vertices, NodePool& node_pool,
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
    ChainDecomposition a = merge_chain_decompositions_rec(vertices, node_pool, chain_decompositions, mid);
    ChainDecomposition b =
        merge_chain_decompositions_rec(vertices, node_pool, chain_decompositions + mid, num_chain_decompositions - mid);
    return merge_chain_decompositions(vertices, node_pool, a, b);
  }
}

Node* vertical_decomposition_with_divide_and_conquer_builder(VerticesView vertices, NodePool& node_pool,
                                                             VerticalDecompositionType decomposition_type)
{
  // VerticalDecompositionType::exterior_decomposition is not yet supported.
  DIDA_ASSERT(decomposition_type == VerticalDecompositionType::interior_decomposition);

  std::vector<ChainDecomposition> chain_decompositions = vertical_decomposition_zigzag_phase(vertices, node_pool);

  ChainDecomposition merged =
      merge_chain_decompositions_rec(vertices, node_pool, chain_decompositions.data(), chain_decompositions.size());
  return merged.first_node;
}

} // namespace dida::detail::vertical_decomposition