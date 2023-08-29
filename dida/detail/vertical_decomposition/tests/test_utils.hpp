#pragma once

#include "dida/detail/vertical_decomposition/vertical_decomposition.hpp"

#include <set>

namespace dida::detail::vertical_decomposition
{

/// A location on the boundary of a polygon.
struct PolygonLocation
{
  /// The index of the edge containing the location.
  size_t edge_index;

  /// The x-coordinate of the location.
  ScalarDeg1 x;
};

/// A function object for comparing polygon locations. The () operator returns true iff the first operand is encountered
/// before the second operand, when traversing the polygon's boundary while starting from its first vertex.
struct PolygonLocationLessThan
{
  /// The vertices of the polygon.
  VerticesView vertices;

  /// Compares two @c PolygonLocation instances.
  bool operator()(const PolygonLocation& a, const PolygonLocation& b) const;
};

/// A range of the boundary of a polygon.
///
/// The start and end points of the range are considered part of the range (ie. it's a closed set).
struct PolygonRange
{
  /// The first of the range.
  ///
  /// The first edge should not be a vertical edge.
  size_t first_edge_index;

  /// The number of edges (full or partial) in this range.
  ///
  /// The edge at 'first_edge_index + num_edges - 1' should not be a vertical edge.
  size_t num_edges;

  /// The x-coordinate of the point on the first edge which is the start point of the range.
  ScalarDeg1 start_point_x;

  /// The x-coordinate of the point on the last edge which is the end point of the range.
  ScalarDeg1 end_point_x;
};

/// Casts a ray upwards from @c ray_origin, until it hits an edge in the given polygon range. If it hits an edge from
/// the interior side, then that edge is returned, if it hits an edge from the exterior side, or if no edge is hit, then
/// @c Edge::invalid() is returned.
///
/// The special case where the ray hits the boundary on a vertex is resolved by shifting that vertex an infinitisemal
/// offset to the right.
Edge ray_cast_up(VerticesView vertices, const PolygonRange& range, Point2 ray_origin);

/// Casts a ray downwards from @c ray_origin, until it hits an edge in the given polygon range. If it hits an edge from
/// the interior side, then that edge is returned, if it hits an edge from the exterior side, or if no edge is hit, then
/// @c Edge::invalid() is returned.
///
/// The special case where the ray hits the boundary on a vertex is resolved by shifting that vertex an infinitisemal
/// offset to the left.
Edge ray_cast_down(VerticesView vertices, const PolygonRange& range, Point2 ray_origin);

/// Gathers all nodes which are reachable from @c node through @c neighbors connections (this includes @c node itself).
std::set<const Node*> gather_nodes(const Node* node);

/// Validates the @c lower_opp_edge and @c upper_opp_edge members of @c node.
///
/// If @c node is a non-leaf node, then it's checked whether the edges match the result of a ray cast from @c
/// *node->vertex_it in the downward and upward direction respectively. If @c node is a leaf node, then it's checked if
/// the edges are the two edges adjacent to @c *node->vertex_it.
///
/// @param vertices The vertices of the polygon.
/// @param range The range of the part of the polygon's boundary we should ray cast against.
/// @param node The node to validate.
/// @return True iff validation succeeded.
bool validate_node_opp_edges(VerticesView vertices, const PolygonRange& range, const Node* node);

struct NodeBranchBoundaryVertices
{
  VertexIt lower_boundary_vertex_it;
  VertexIt upper_boundary_vertex_it;
};

NodeBranchBoundaryVertices node_branch_boundary_vertices(const ChainDecomposition& chain_decomposition,
                                                         const Node* node, uint8_t branch_index);

/// Validates the neighbors of @c node. This function validates whether
///
///  - A neighbor exists for a branch iff it should exist according to @c node_should_have_neighbor.
///  - Neighboring nodes link back to this node.
///  - If this node is to the left of its neighbor, then @c validate_neighboring_nodes_pair is called (note that if this
///    node is to the right of its neighbor then it's left to the @c validate_node_neigbors call of the neighboring node
///    to call @c validate_neighboring_nodes_pair for this pair.
///
/// @param vertices The vertices of the polygon.
/// @param chain_decomposition The chain decomposition @c node belongs to.
/// @param node The node.
/// @return True iff validation succeeded.
bool validate_node_neighbors(VerticesView vertices, const ChainDecomposition& chain_decomposition, const Node* node);

bool validate_chain_decomposition(VerticesView vertices, const ChainDecomposition& chain_decomposition);

/// Prints the given nodes as C++.
///
/// @param vertices The vertices of the polygon.
/// @param nodes The nodes to print.
void print_nodes(VerticesView vertices, ArrayView<const Node> nodes);

/// Decomposes the polygon formed by @c vertices into a set of chain decompositions, by starting a new chain
/// decomposition at each convex side vertex.
///
/// @param vertices The vertices of the polygon.
/// @param node_pool The pool used to allocate the nodes in the resulting chain decompositions.
std::vector<ChainDecomposition> initial_chain_decompositions(VerticesView vertices, NodePool& node_pool);

} // namespace dida::detail::vertical_decomposition
