#pragma once

#include <set>

#include "dida/detail/vertical_decomposition/vertical_decomposition.hpp"

namespace dida::detail::vertical_decomposition
{

/// Gathers all nodes which are reachable from @c node through @c neighbors connections (this includes @c node itself).
std::set<const Node*> gather_nodes(const Node* node);

/// The leftmost or rightmost vertices of the lower and upper boundary of a region.
///
/// If the node this @c NodeBranchBoundaryVertices is associated with is on the left side of the region, then the two
/// vertices are the left vertices of the leftmost edges of each boundary which is at least partially adjacent to the
/// region. Similarly for nodes on the right side of the region.
struct NodeBranchBoundaryVertices
{
  /// The vertex of the lower boundary on the side of the region the node is on, or @c nullptr if the region has no
  /// lower boundary.
  VertexIt lower_boundary_vertex_it;

  /// The vertex of the upper boundary on the side of the region the node is on, or @c nullptr if the region has no
  /// upper boundary.
  VertexIt upper_boundary_vertex_it;
};

/// Returns the @c NodeBranchBoundaryVertices of the branch of @c node with index @c branch_index.
///
/// The @c chain_decomposition parameter is used to check whether @c node is the first or last node of its chain, and is
/// not otherwise used. If @c node can't be the first or last node, then this can be set to
/// <tt>ChainDecomposition{nullptr, nullptr}</tt>.
NodeBranchBoundaryVertices node_branch_boundary_vertices(const ChainDecomposition& chain_decomposition, Winding winding,
                                                         const Node* node, uint8_t branch_index);

/// Validates the neighbors of @c node. This function validates whether
///
///  - A neighbor exists for a branch iff it should exist according to @c node_should_have_neighbor.
///  - Neighboring nodes link back to this node.
///  - If this node is to the left of its neighbor, then @c validate_neighboring_nodes_pair is called (note that if this
///    node is to the right of its neighbor then it's left to the @c validate_node_neigbors call of the neighboring node
///    to call @c validate_neighboring_nodes_pair for this pair.
///
/// A return value of @c true indicates that validation succeeded. If there were errors, then extra information about
/// these errors was logged using @c UNSCOPED_INFO.
bool validate_node_neighbors(VerticesView vertices, Winding winding, const ChainDecomposition& chain_decomposition,
                             const Node* node);

/// Validates a chain decomposition.
///
/// A return value of @c true indicates that validation succeeded. If there were errors, then extra information about
/// these errors was logged using @c UNSCOPED_INFO.
bool validate_chain_decomposition(VerticesView vertices, Winding winding,
                                  const ChainDecomposition& chain_decomposition);

/// Validates the vertical decomposition of a polygon.
///
/// @c root_node should be some node of the vertical decomposition. All other nodes are found by following the @c
/// neighbors pointers.
///
/// A return value of @c true indicates that validation succeeded. If there were errors, then extra information about
/// these errors was logged using @c UNSCOPED_INFO.
bool validate_polygon_decomposition(VerticesView vertices, Winding winding, const Node* root_node);

/// Inverts the x-coordinate of the given vertices.
///
/// This function can be used to turn a polygon with counter clockwise winding into one with clockwise winding and vice
/// versa, in such a way that often, in an algorithm this new polygon will result in the same branches being taken as
/// with the original polygon, if the winding of the algorithm is inverted.
void flip_horizontally(ArrayView<Point2> vertices);

/// Inverts the direction of each of the given nodes.
///
/// This function can be used to update the nodes of a vertical decomposition after the vertices of the polygon have
/// been flipped using @c flip_horizontally.
void flip_horizontally(ArrayView<Node> nodes);

/// Returns the given node type as a string.
std::string_view node_type_to_string(NodeType node_type);

/// Prints the given nodes as C++.
void print_nodes(VerticesView vertices, ArrayView<const Node> nodes);

/// Decomposes the polygon formed by @c vertices into a set of chain decompositions, by starting a new chain
/// decomposition at each convex side vertex.
std::vector<ChainDecomposition> initial_chain_decompositions(VerticesView vertices, Winding winding,
                                                             NodePool& node_pool);

} // namespace dida::detail::vertical_decomposition
