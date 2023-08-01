#pragma once

#include "dida/detail/vertical_decomposition.hpp"

namespace dida::detail::vertical_decomposition
{

Node* vertical_decomposition_with_dnc_builder(VerticesView vertices, NodePool& node_pool,
                                              VerticalDecompositionType decomposition_type);

}