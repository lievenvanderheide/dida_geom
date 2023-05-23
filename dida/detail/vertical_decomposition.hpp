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

/// An iterator which iterates over the regions of a vertical decomposition.
///
/// The order in which the regions are returned is as follows:
///
/// Image that we start at the vertex of the @c first_node passed to the constructor, and then follow the boundary of
/// the decomposition's polygon all the way around until we reach the first vertex again. While doing this, we encounter
/// all regions of the vertical decomposition in some order, the only problem is that most regions are encountered
/// twice, once while traversing their lower boundary and once while're traversing their upper boundary. The exceptions
/// are the infinite regions of an external decomposition and leaf regions. Since we only want to include each region
/// once, we'll only include these regions when traversing their lower boundary while skipping them when traversing
/// their upper boundary.
///
/// We refer to the point which traverses the boundary as the "traversal point". The traversal point is used throughout
/// the documentation of this class, but is never actually computed during runtime.
class VerticalDecompositionRegionsIterator
{
public:
  /// Constructs a @c VerticalDecompositionRegionsIterator for a traversal starting with traversal point @c
  /// first_node->vertex_it.
  ///
  /// The first region will be available immediately after construction. Use @c move_next to move to subsequent regions.
  ///
  /// @param first_node The first node.
  VerticalDecompositionRegionsIterator(const Node* first_node);

  /// Moves to the next region of this iteration.
  ///
  /// If the end of the iteration was reached, @c false is returned. The iterator should not be used anymore after this.
  ///
  /// @return True iff the iterator successfully advanced to the next region, false if the end of the iteration was
  /// reached.
  bool move_next();

  /// A struct with information about the current region.
  ///
  /// One of @c left_node, @c right_node can be @c nullptr (but not both), which indicates that the current region is
  /// a "leaf" region.
  struct Region
  {
    /// The vertical decomposition node on the left side of this region, or @c nullptr if there's no left node.
    const Node* left_node;

    /// The vertical decomposition node on the right side of this region, or @c nullptr if there's no right node.
    const Node* right_node;

    /// The index of the branch of @c left_node which connects to this region. If there's no left node, then this value
    /// is undefined.
    uint8_t left_node_branch_index;

    /// The index of the branch of @c right_node which connects to this region. If there's no right node, then this
    /// value is undefined.
    uint8_t right_node_branch_index;

    /// Compares two @c Region instances for equality. Note that the @c leaf_region_branch_index fields are only
    /// compared when one of @c left_node, @c right_node is @c nullptr.
    ///
    /// @param b The second operand of the comparison.
    /// @return True iff the two regions are equal.
    inline bool operator==(const Region& b) const;
  };

  /// Returns the current region.
  ///
  /// @return
  inline Region region() const;

private:
  /// Returns true if the current region should be skipped.
  bool should_skip_current_region() const;

  /// The first node, passed to the @c VerticalDecompositionRegionsIterator constructor.
  const Node* first_node_;

  /// The node where the current traversal point is at (that is, the current traversal point is either
  /// cur_node_->vertex_it, or one of the two points at the end of the vertical extension of cur_node_->vertex_it.
  const Node* cur_node_;

  /// The next node which will be reached, or @c nullptr if we're currently in a leaf region.
  const Node* next_node_;

  uint8_t cur_node_branch_index_;

  uint8_t next_node_branch_index_;

  // The direction of the boundary at traversal point. If the traversal point is a reflex vertex, then this is the
  // direction of the outgoing part.
  HorizontalDirection direction_;
};

} // namespace dida::detail::vertical_decomposition

#include "dida/detail/vertical_decomposition.inl"