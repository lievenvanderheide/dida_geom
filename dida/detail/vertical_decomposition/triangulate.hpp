#pragma once

#include "dida/convex_polygon2.hpp"
#include "dida/detail/vertical_decomposition/vertical_decomposition.hpp"

namespace dida::detail::vertical_decomposition
{

/// Triangulates the polygon with the given vertices and vertical decomposition.
///
/// @c root_node should be some leaf node of the vertical decomposition. All other nodes are reached by following @c
/// neighbors pointers.
std::vector<Triangle2> triangulate(VerticesView vertices, const Node* root_node);

} // namespace dida::detail::vertical_decomposition