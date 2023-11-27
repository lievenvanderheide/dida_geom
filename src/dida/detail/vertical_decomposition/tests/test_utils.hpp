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

/// A contact point where a vertical extension meets its chain.
struct VerticalExtensionContactPoint
{
  /// The type of contact point.
  enum class Type
  {
    /// The contact point is the vertex of @c node.
    vertex,

    /// The contact point is the point where the vertical extension meets @c node->lower_opp_edge.
    lower_opp_edge,

    /// The contact point is the point where the vertical extension meets @c node->upper_opp_edge.
    upper_opp_edge,

    /// The contact point is the vertex of @c node, and @c node is a leaf node.
    leaf,
  };

  /// The type of this @c contact point.
  Type type;

  /// The node.
  const Node* node;
};

/// Returns the @c VerticalExtensionContactPoint of the given chain decomposition, in the order they're encountered when
/// following the chain from beginning to end.
std::vector<VerticalExtensionContactPoint>
vertical_extension_contact_points(const ChainDecomposition& chain_decomposition);

/// Validates the @c lower_opp_edge and @c upper_opp_edge members of @c node.
///
/// If @c node is a non-leaf node, then it's checked whether the edges match the result of a ray cast from @c
/// *node->vertex_it in the downward and upward direction respectively. If @c node is a leaf node, then it's checked if
/// the edges are the two edges adjacent to @c *node->vertex_it.
///
/// A return value of @c true indicates that validation succeeded. If there were errors, then extra information about
/// these errors was logged using @c UNSCOPED_INFO.
bool validate_node_opp_edges(VerticesView vertices, const PolygonRange& range, const Node* node);

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
/// A return value of @c true indicates that validation succeeded. If there were errors, then extra information about
/// these errors was logged using @c UNSCOPED_INFO.
bool validate_node_neighbors(VerticesView vertices, const ChainDecomposition& chain_decomposition, const Node* node);

/// Validates a chain decomposition.
///
/// A return value of @c true indicates that validation succeeded. If there were errors, then extra information about
/// these errors was logged using @c UNSCOPED_INFO.
bool validate_chain_decomposition(VerticesView vertices, const ChainDecomposition& chain_decomposition);

/// Validates the vertical decomposition of a polygon.
///
/// @c root_node should be some node of the vertical decomposition. All other nodes are found by following the @c
/// neighbors pointers.
///
/// A return value of @c true indicates that validation succeeded. If there were errors, then extra information about
/// these errors was logged using @c UNSCOPED_INFO.
bool validate_polygon_decomposition(VerticesView vertices, const Node* root_node);

/// Prints the given nodes as C++.
void print_nodes(VerticesView vertices, ArrayView<const Node> nodes);

/// Decomposes the polygon formed by @c vertices into a set of chain decompositions, by starting a new chain
/// decomposition at each convex side vertex.
std::vector<ChainDecomposition> initial_chain_decompositions(VerticesView vertices, NodePool& node_pool);

} // namespace dida::detail::vertical_decomposition
