#pragma once

#include "dida/array_view.hpp"
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
YOnEdge y_on_edge_for_x(Segment2 edge, ScalarDeg1 x);

/// A horizontal direction.
enum class HorizontalDirection : uint8_t
{
  /// The direction to the left.
  left,

  /// The direction to the right.
  right,
};

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

  /// Compares two edges for equality.
  ///
  /// @param b The second operand.
  /// @return True iff this edge and @c b are equal.
  inline bool operator==(const Edge b) const;
};

/// A node in the vertical decomposition graph.
struct Node
{
  /// The direction of this node. This indicate the direction into which the vertical decomposition splits into two,
  /// that is
  ///
  ///  - If this is @c HorizontalDirection::left then it has one region on its left and 2 regions on its right.
  ///  - If this is @c HorizontalDirection::right then it has one region on its right and 2 regions on its left.
  ///
  HorizontalDirection direction;

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
};

/// A vertical decomposition of a polygon.
struct VerticalDecomposition
{
  /// The vertical decomposition nodes.
  std::vector<Node> nodes;
};

} // namespace dida::detail::vertical_decomposition

#include "dida/detail/vertical_decomposition.inl"