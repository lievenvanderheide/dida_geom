#pragma once

#include "dida/detail/vertical_decomposition.hpp"

#include <set>

namespace dida::detail::vertical_decomposition
{

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
///
/// @param vertices The vertices of the polygon.
/// @param range The range.
/// @param ray_origin The point the ray originates from.
/// @return The edge.
Edge ray_cast_up(VerticesView vertices, const PolygonRange& range, Point2 ray_origin);

/// Casts a ray downwards from @c ray_origin, until it hits an edge in the given polygon range. If it hits an edge from
/// the interior side, then that edge is returned, if it hits an edge from the exterior side, or if no edge is hit, then
/// @c Edge::invalid() is returned.
///
/// The special case where the ray hits the boundary on a vertex is resolved by shifting that vertex an infinitisemal
/// offset to the left.
///
/// @param vertices The vertices of the polygon.
/// @param range The range.
/// @param ray_origin The point the ray originates from.
/// @return The edge.
Edge ray_cast_down(VerticesView vertices, const PolygonRange& range, Point2 ray_origin);

/// Gathers all nodes which are reachable from @c node through @c neighbors connections (this includes @c node itself).
///
/// @param node The node.
/// @return The set of all nodes reachable from @c node.
std::set<const Node*> gather_nodes(const Node* node);

/// Validates whether two nodes which are neighbors according to their @c neighbors pointers should be neighbors
/// according to the geometry of the polygon formed by @c vertices.
///
/// The nodes must be ordered such that <tt>lex_less_than(*left_node->vertex_it, *right_node->vertex_it)</tt>.
///
/// @param vertices The vertices of the polygon.
/// @param left_node The left node of the node pair.
/// @param left_node_branch_index The index of the branch in @c left_node which connects to @c right_node.
/// @param right_node The right node of the node pair.
/// @param right_node_branch_index The index of the branch in @c right_node which connects to @c left_node.
/// @return True iff validation succeeded.
bool validate_neighboring_nodes(VerticesView vertices, const Node* left_node, uint8_t left_node_branch_index,
                                const Node* right_node, uint8_t right_node_branch_index);

bool validate_chain_decomposition(VerticesView vertices, const ChainDecomposition& chain_decomposition);

} // namespace dida::detail::vertical_decomposition
