#pragma once

#include <iostream>

#include "dida/array_view.hpp"
#include "dida/detail/tmp_object_pool.hpp"
#include "dida/math/fraction.hpp"
#include "dida/point2.hpp"
#include "dida/segment2.hpp"
#include "dida/utils.hpp"

namespace dida::detail::vertical_decomposition
{

using VerticesView = ArrayView<const Point2>;
using VertexIt = VerticesView::const_iterator;
using YOnEdge = math::Fraction<ScalarDeg2, ScalarDeg1>;

/// Returns the y-coordinate of the point on @c edge which has x-coordinate @c x.
///
/// @pre @c edge should not be vertical.
/// @param edge The edge.
/// @param x The x-coordinate.
/// @return The y-coordinate of the point with x-coordinate @c x.
inline YOnEdge y_on_edge_for_x(Segment2 edge, ScalarDeg1 x);

/// A horizontal direction.
enum class HorizontalDirection : uint8_t
{
  /// The direction to the left.
  left,

  /// The direction to the right.
  right,
};

/// The type of a node.
enum class NodeType : uint8_t
{
  /// A leaf node of the vertical decomposition. The region on the incoming direction ends in the vertex of this node.
  leaf,

  /// A node where the vertical decomposition branches into two. The vertical extension of this node has one region on
  /// its incoming side and 2 regions on its outgoing side.
  branch,

  /// This type of node only occurs in an incomplete decomposition, and indicates the location where a region with both
  /// an upper and lower boundary branches into two unbounded regions: The lower outgoing branch does not have an upper
  /// boundary and the upper outgoing branch does not have a lower boundary.
  outer_branch,
};

/// Returns the other direction of @c direction.
///
/// @param direction The original direction.
/// @return The other direction.
constexpr HorizontalDirection other_direction(HorizontalDirection direction);

/// Returns whether point @c a comes before point @c b, when going in the given direction.
///
/// @tparam direction The direction.
/// @param a The first operand.
/// @param b The second operand.
/// @return True iff @c a comes before @c b.
template <HorizontalDirection direction>
bool lex_less_than_with_direction(Point2 a, Point2 b);

/// An edge of the input polygon.
struct Edge
{
  /// An iterator pointing towards the start vertex of the edge, or @c nullptr if the edge is invalid.
  VertexIt start_vertex_it;

  /// An iterator pointing towards the end vertex of the edge, or @c nullptr if the edge is invalid.
  ///
  /// If <tt>start_vertex_it != nullptr</tt>, then this will always be equal to <tt>next_cyclic(vertices,
  /// start_vertex_it)</tt>.
  VertexIt end_vertex_it;

  /// Returns the edge with the vertex at @c index as its start vertex.
  ///
  /// @param vertices The vertices.
  /// @param index The index of the start vertex.
  /// @return The edge.
  static inline Edge edge_from_index(VerticesView vertices, size_t index);

  /// Returns the invalid edge.
  ///
  /// @return The invalid edge.
  static inline Edge invalid();

  /// Returns whether this edge is valid.
  ///
  /// @return True iff the edge is valid.
  inline bool is_valid() const;

  /// Returns this edge as a @c Segment2.
  ///
  /// @return The segment.
  inline Segment2 segment() const;

  /// Returns whether @c point is strictly on the interior side of (the line through) this edge.
  ///
  /// @param point The point.
  /// @return True iff @c point is on the interior side of this edge.
  inline bool on_interior_side(Point2 point) const;

  /// Returns whether @c point is strictly on the exterior side of (the line through) this edge.
  ///
  /// @param point The point.
  /// @return True iff @c point is on the exterior side of this edge.
  inline bool on_exterior_side(Point2 point) const;

  /// Compares two edges for equality.
  ///
  /// @param b The second operand.
  /// @return True iff this edge and @c b are equal.
  inline bool operator==(const Edge b) const;

  /// Compares two edges for inequality.
  ///
  /// @param b The second operand.
  /// @return True iff this edge and @c b are distinct.
  inline bool operator!=(const Edge b) const;
};

/// Writes @c edge to the output stream @c s.
inline std::ostream& operator<<(std::ostream& s, Edge edge);

/// A node in the vertical decomposition graph.
struct Node
{
  /// The direction of this node.
  ///
  /// For non-leaf nodes, This indicate the direction into which the vertical decomposition splits into two, that is
  ///
  ///  - If this is @c HorizontalDirection::left then it has one region on its left and 2 regions on its right.
  ///  - If this is @c HorizontalDirection::right then it has one region on its right and 2 regions on its left.
  ///
  /// For leaf nodes, this indicates the incoming direction at the convex side vertex corresponding to this node.
  HorizontalDirection direction;

  /// The type of this node.
  NodeType type;

  /// An iterator pointing to the vertex from which the vertical extensions corresponding to this node extend.
  VertexIt vertex_it;

  /// The edge which contains the opposite contact point of the vertical extension extending downwards from the vertex
  /// at @c vertex_it, or @c Edge::invalid() if there's no opposite contact point.
  Edge lower_opp_edge;

  /// The edge which contains the opposite contact point of the vertical extension extending upwards from the vertex at
  /// @c vertex_it, or @c Edge::invalid() if there's no opposite contact point.
  Edge upper_opp_edge;

  /// The neighboring nodes of this node.
  ///
  ///  - neighbors[0] is the neighbor on the incoming side.
  ///  - neighbors[1] is the lower node on the outgoing side.
  ///  - neighbors[2] is the upper node on the outgoing side.
  ///
  /// Each neighbor pointer can be nullptr.
  Node* neighbors[3];

  /// Returns the index of the given neighbor in this node's @c neighbors array.
  inline uint8_t neighbor_branch_index(const Node* neighbor) const;

  /// Replaces the entry in this node's @c neighbors array which is currently equal to @c neighbor with @c new_neighbor.
  inline void replace_neighbor(Node* neighbor, Node* new_neighbor);
};

using NodePool = TmpObjectPool<Node, 64>;

/// A range of edges of the input polygon.
struct EdgeRange
{
  /// An iterator pointing to the start vertex of the first edge.
  VertexIt start_vertex_it;

  /// An iterator pointing to the end vertex of the last edge. This should not be equal to @c start_vertex_it.
  VertexIt end_vertex_it;

  /// Returns the invalid edge range.
  ///
  /// @return The invalid edge range.
  static inline EdgeRange invalid();

  /// Returns whether this edge range is valid.
  ///
  /// @return True iff the edge range is valid.
  bool inline is_valid() const;
};

/// Returns the edge in the given monotone edge range for which <tt>edge_range.start_vertex <= point <
/// edge_range.end_vertex</tt>, where the ordering used is @c lex_less_than_with_direction<direction>.
///
/// The edge range must be a monotone edge range, which means that all edges in it should satisfy
/// <tt>edge_range.start_vertex < edge_range.end_vertex</tt>.
///
/// @tparam direction The direction of monotonicity.
/// @param vertices The vertices.
/// @param edge_range The edge range to search.
/// @param point The query point.
/// @return The edge in @c edge_range corresponding to @c point.
template <HorizontalDirection direction>
Edge edge_for_point_with_monotone_edge_range(VerticesView vertices, EdgeRange edge_range, Point2 point);

/// The type of a vertical decomposition.
enum class VerticalDecompositionType
{
  /// The decomposition of the interior of a polygon.
  interior_decomposition,

  /// The decomposition of the exterior of a polygon.
  exterior_decomposition,
};

/// A vertical decomposition of a polygon.
struct VerticalDecomposition
{
  /// The vertical decomposition nodes.
  std::vector<Node> nodes;

  /// The leftmost node of this vertical decomposition. This is the node with the lexicographically least vertex.
  Node* leftmost_node;

  /// The rightmost node of this vertical decomposition. This is the node with the lexicographically greatest vertex.
  Node* rightmost_node;
};

/// A struct which holds pointers to the nodes at the beginning and end of the vertical decompisition of a polygonal
/// chain. All other nodes of the chain can be reached by following the @c neighbors pointers in either of these nodes.
///
/// A special case is the chain which consists of all the edges of the input polygon. In this case, @c first_node can be
/// any leaf node of the vertical decomposition and @c last_node is undefined.
struct ChainDecomposition
{
  /// In the general case, this is the node at the first vertex of this chain.
  ///
  /// In the special case when this decomposition is the decomposition of the full polygon, then this is some leaf node
  /// of the vertical decomposition.
  Node* first_node;

  /// In the general case, this is the  node at the last vertex of this chain.
  ///
  /// In the special case when this decomposition is the decomposition of the full polygon, then this value is
  /// undefined.
  Node* last_node;
};

/// A region in a vertical decomposition. A region is bounded on the left and the right by the vertical extensions
/// corresponding to two nodes, and bounded above and below by two x-monotone edge ranges of the input polygon.
struct Region
{
  /// The vertical decomposition node on the left side of this region.
  const Node* left_node;

  /// The vertical decomposition node on the right side of this region.
  const Node* right_node;

  /// The index of the branch of @c left_node which connects to this region.
  uint8_t left_node_branch_index;

  /// The index of the branch of @c right_node which connects to this region.
  uint8_t right_node_branch_index;

  /// Compares two @c Region instances for equality.
  ///
  /// @param b The second operand of the comparison.
  /// @return True iff the two regions are equal.
  inline bool operator==(const Region& b) const;

  /// Returns the @c EdgeRange of the lower boundary of this region, or @c EdgeRange::invalid if there's no lower
  /// boundary.
  ///
  /// The resulting @c EdgeRange includes all edges which are fully or partially adjacent to the region from below.
  ///
  /// @param vd_type The type of the vertical decomposition this region belongs to.
  /// @return The edge range.
  inline EdgeRange lower_boundary(VerticalDecompositionType vd_type) const;

  /// Returns the @c EdgeRange of the upper boundary of this region, or @c EdgeRange::invalid if there's no upper
  /// boundary.
  ///
  /// The resulting @c EdgeRange includes all edges which are fully or partially adjacent to the region from above.
  ///
  /// @param vd_type The type of the vertical decomposition this region belongs to.
  /// @return The edge range.
  inline EdgeRange upper_boundary(VerticalDecompositionType vd_type) const;
};

/// An iterator which iterates over the regions of a vertical decomposition.
///
/// The order in which the regions are returned is as follows:
///
/// Imagine that we start at the vertex of the @c first_node passed to the constructor, and then follow the boundary of
/// the decomposition's polygon all the way around until we reach the first vertex again. While doing this, we encounter
/// all regions of the vertical decomposition in some order, the only problem is that most regions are encountered
/// twice, once while traversing their lower boundary and once while're traversing their upper boundary. The exceptions
/// are the infinite regions of an external decomposition and leaf regions. Since we only want to include each region
/// once, we only include the finite regions when traversing the region's lower boundary, while skipping it when
/// traversing their upper boundary.
///
/// We refer to the point which traverses the boundary as the "traversal point". The traversal point is used throughout
/// the documentation of this class, but is never actually computed during runtime.
class RegionIterator
{
public:
  /// Constructs a @c RegionIterator for a traversal starting with traversal point @c first_node->vertex_it. The first
  /// node should not be a node at infinity of an external decomposition.
  ///
  /// The first region will be available immediately after construction. Use @c move_next to move to subsequent regions.
  ///
  /// @param first_node The first node.
  inline RegionIterator(const Node* first_node);

  /// Moves to the next region of this iteration.
  ///
  /// If the end of the iteration was reached, @c false is returned. The iterator should not be used anymore after this.
  ///
  /// @return True iff the iterator successfully advanced to the next region, false if the end of the iteration was
  /// reached.
  inline bool move_next();

  /// Returns the current region.
  ///
  /// @return
  inline Region region() const;

private:
  /// Returns true if the current region should be skipped.
  inline bool should_skip_current_region() const;

  /// The first node, passed to the @c RegionIterator constructor.
  const Node* first_node_;

  /// The node where the current traversal point is at, that is, the current traversal point is either this node's
  /// vertex, or the contact point on one of the two opposite edges of the node.
  const Node* cur_node_;

  /// The next node.
  const Node* next_node_;

  // The direction of the boundary at traversal point. If the traversal point is a side vertex, then this is the
  // direction of the outgoing part.
  HorizontalDirection direction_;
};

} // namespace dida::detail::vertical_decomposition

#include "dida/detail/vertical_decomposition/vertical_decomposition.inl"
