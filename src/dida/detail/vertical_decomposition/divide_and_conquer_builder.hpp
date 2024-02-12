#pragma once

#include "dida/detail/vertical_decomposition/vertical_decomposition.hpp"

namespace dida::detail::vertical_decomposition
{

/// Computes the vertical decomposition of the given region of the polygon formed by @c vertices, using a divide and
/// conquer based algorithm.
///
/// The returned node is some node of the vertical decomposition. All other nodes can be reached by following the
/// pointers in the nodes' @c neighbors array. The nodes are allocated using the provided @c node_pool, so the vertical
/// decomposition remains alive as long as the node pool stays alive, while deallocating the node pool is enough to
/// deallocate the vertical decomposition.
Node* interior_decomposition_with_divide_and_conquer_builder(VerticesView vertices, NodePool& node_pool);

struct ExteriorDecomposition
{
  Node* leftmost_node;
  Node* rightmost_node;
};

ExteriorDecomposition exterior_decomposition_with_divide_and_conquer_builder(VerticesView vertices,
                                                                             NodePool& node_pool);

} // namespace dida::detail::vertical_decomposition