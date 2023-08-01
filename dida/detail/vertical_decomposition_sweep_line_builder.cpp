#include "dida/detail/vertical_decomposition_sweep_line_builder.hpp"

#include <algorithm>

#include "dida/utils.hpp"

namespace dida::detail::vertical_decomposition
{

namespace
{

/// The state class of the @c vertical_decomposition_with_sweep_line_builder algorithm.
class SweepState
{
public:
  /// Constructs a sweep state for the given vertices and decomposition type.
  SweepState(ArrayView<const Point2> vertices, VerticalDecompositionType decomposition_type);

  /// Initializes the sweep state to the state it should have right before the first event is processed.
  ///
  ///  - It initializes the @c events_ list.
  ///  - If the region is @c VerticalDecompositionType::exterior, then @c active_edges_ is initialized with the
  ///    two edges at positive and negative infinity.
  ///
  void init_sweep();

  /// Handles all sweep events.
  void run_sweep();

  /// Moves the resulting nodes of the algorithms out of this @c SweepState.
  ///
  /// @return A vector containing the nodes.
  std::vector<Node>&& move_out_nodes();

private:
  /// A sweep line event.
  struct Event
  {
    /// The vertex corresponding to the event.
    VertexIt vertex_it;

    /// Whether the incoming edge at this event's vertex is a rightward edge the right. This is the case if
    /// <tt>lex_less_than(prev_vertex, vertex)</tt>.
    bool incoming_towards_right;

    /// Whether the outgoing edge at this event's vertex is a rightward edge the right. This is the case if
    /// <tt>lex_less_than(vertex, next_vertex)</tt>.
    bool outgoing_towards_right;

    /// If this event's vertex is a reflex corner (which is the case if <tt>incoming_towards_right !=
    /// outgoing_towards_right</tt>), then this indicates if it's a concave corner with respect to the region we're
    /// decomposing.
    ///
    /// This value is not set if the event's vertex isn't a reflex corner.
    bool is_concave_corner;
  };

  /// An edge which currently intersects the sweep line.
  struct ActiveEdge
  {
    /// The left vertex of this edge.
    VertexIt left_vertex_it;

    /// The right vertex of this edge.
    VertexIt right_vertex_it;

    /// A pointer to the node on the left side of the region above this edge, or @c nullptr
    ///
    /// This value is only defined for edges which have the region which we're decomposing above them.
    Node* region_left_node;

    /// The index of the branch of @c region_left_node corresponding to the region above this edge.
    ///
    /// This value is only defined when @c region_left_node is defined and not equal to @c nullptr.
    uint8_t region_left_node_branch_index;

    /// Returns the @c Edge corresponding to this @c ActiveEdge.
    Edge edge() const;
  };

  using ActiveEdgesIt = std::vector<ActiveEdge>::iterator;

  /// Handles an event where 2 new edges appear in the active edges list.
  ///
  /// @param event The event to handle.
  void handle_appear_event(const Event& event);

  /// Handles an event where 2 edges vanish from the active edges list.
  ///
  /// @param event The event to handle.
  void handle_vanish_event(const Event& event);

  /// Handles an event where one active edges transitions into another active edge.
  ///
  /// @param event The event to handle.
  void handle_transition_event(const Event& event);

  /// Returns an iterator to the location where an edge with the given vertex should be inserted.
  ///
  /// @param vertex The vertex.
  /// @return An iterator pointing to the insert location.
  ActiveEdgesIt insert_location(Point2 vertex);

  /// The vertices of the input polygon.
  ArrayView<const Point2> vertices_;

  /// The type of decomposition we're computing
  VerticalDecompositionType decomposition_type_;

  /// The list of events, sorted lexicographically by their vertex position.
  std::vector<Event> events_;

  /// The set of edges which currently intersect the sweep line, sorted from bottom to top by their intersection with
  /// the sweep line.
  std::vector<ActiveEdge> active_edges_;

  /// The set of output nodes. The number of output nodes is determined in @c init_sweep, so the vector is resized to
  /// its correct size in that function.
  std::vector<Node> nodes_;

  /// An iterator pointing to the next node to be written.
  std::vector<Node>::iterator nodes_it_;
};

SweepState::SweepState(ArrayView<const Point2> vertices, VerticalDecompositionType decomposition_type)
    : vertices_(vertices), decomposition_type_(decomposition_type)
{
}

void SweepState::init_sweep()
{
  events_.resize(vertices_.size());

  size_t num_nodes = 0;

  VertexIt prev_vertex_it = vertices_.end() - 2;
  VertexIt cur_vertex_it = vertices_.end() - 1;
  bool incoming_towards_right = lex_less_than(*prev_vertex_it, *cur_vertex_it);
  for (size_t i = 0; i < vertices_.size(); i++)
  {
    VertexIt next_vertex_it = vertices_.begin() + i;
    bool outgoing_towards_right = lex_less_than(*cur_vertex_it, *next_vertex_it);

    events_[i].vertex_it = cur_vertex_it;
    events_[i].incoming_towards_right = incoming_towards_right;
    events_[i].outgoing_towards_right = outgoing_towards_right;

    if (incoming_towards_right != outgoing_towards_right)
    {
      ScalarDeg2 orientation = cross(*cur_vertex_it - *prev_vertex_it, *next_vertex_it - *cur_vertex_it);
      if (decomposition_type_ == VerticalDecompositionType::interior_decomposition)
      {
        events_[i].is_concave_corner = orientation < 0;
      }
      else
      {
        events_[i].is_concave_corner = orientation > 0;
      }

      num_nodes++;
    }

    incoming_towards_right = outgoing_towards_right;
    prev_vertex_it = cur_vertex_it;
    cur_vertex_it = next_vertex_it;
  }

  std::sort(events_.begin(), events_.end(),
            [](const Event& a, const Event& b) { return lex_less_than(*a.vertex_it, *b.vertex_it); });

  if (decomposition_type_ == VerticalDecompositionType::exterior_decomposition)
  {
    // If we're decomposing the exterior, then there are regions which extend infinitely upwards and downwards. To make
    // these regions mostly behave like normal regions, we'll add two extra edges which indicate the edges at negative
    // and positive infinity.
    active_edges_ = {
        {nullptr, nullptr, nullptr, 0},
        {nullptr, nullptr, nullptr, 0},
    };
  }

  nodes_.resize(num_nodes);
  nodes_it_ = nodes_.begin();
}

void SweepState::run_sweep()
{
  for (const Event& event : events_)
  {
    if (event.incoming_towards_right == event.outgoing_towards_right)
    {
      handle_transition_event(event);
    }
    else if (event.outgoing_towards_right)
    {
      handle_appear_event(event);
    }
    else
    {
      handle_vanish_event(event);
    }
  }
}

std::vector<Node>&& SweepState::move_out_nodes()
{
  return std::move(nodes_);
}

void SweepState::handle_appear_event(const Event& event)
{
  ActiveEdgesIt insert_location_it = insert_location(*event.vertex_it);

  VertexIt prev_vertex_it = prev_cyclic(vertices_, event.vertex_it);
  VertexIt next_vertex_it = next_cyclic(vertices_, event.vertex_it);

  ActiveEdge lower_appearing_edge, upper_appearing_edge;

  if (event.is_concave_corner)
  {
    // This vertex is the point where a signle region is split into two other regions when going towards the right.
    //
    // The two new regions are the ones below the lower outgoing edge and the one above the upper outgoing edge. The
    // part between the two outgoing edges is not part of the region we're decomposing.

    ActiveEdge& lower_opp_edge = *(insert_location_it - 1);
    ActiveEdge& upper_opp_edge = *insert_location_it;

    Node& new_node = *(nodes_it_++);
    new_node.direction = HorizontalDirection::right;
    new_node.type = NodeType::branch;
    new_node.vertex_it = event.vertex_it;
    new_node.lower_opp_edge = lower_opp_edge.edge();
    new_node.upper_opp_edge = upper_opp_edge.edge();
    new_node.neighbors[0] = lower_opp_edge.region_left_node;
    new_node.neighbors[1] = nullptr;
    new_node.neighbors[2] = nullptr;

    if (lower_opp_edge.region_left_node)
    {
      lower_opp_edge.region_left_node->neighbors[lower_opp_edge.region_left_node_branch_index] = &new_node;
    }

    // The region above lower_opp_edge is now the lower outgoing region.
    lower_opp_edge.region_left_node = &new_node;
    lower_opp_edge.region_left_node_branch_index = 1;

    // The part above the lower outgoing edge is not part of the region we're decomposing, so its 'region_left_node' is
    // undefined (we just set it to nullptr). The part above the upper outgoing edge is the upper outgoing region, so we
    // set it accordingly.

    VertexIt lower_right_vertex_it, upper_right_vertex_it;
    if (decomposition_type_ == VerticalDecompositionType::interior_decomposition)
    {
      lower_right_vertex_it = prev_vertex_it;
      upper_right_vertex_it = next_vertex_it;
    }
    else
    {
      lower_right_vertex_it = next_vertex_it;
      upper_right_vertex_it = prev_vertex_it;
    }

    active_edges_.insert(insert_location_it, {{event.vertex_it, lower_right_vertex_it, nullptr, 0},
                                              {event.vertex_it, upper_right_vertex_it, &new_node, 2}});
  }
  else
  {
    VertexIt lower_right_vertex_it, upper_right_vertex_it;
    Edge lower_appearing_edge, upper_appearing_edge;
    if (decomposition_type_ == VerticalDecompositionType::interior_decomposition)
    {
      lower_right_vertex_it = next_vertex_it;
      upper_right_vertex_it = prev_vertex_it;

      lower_appearing_edge = Edge{event.vertex_it, next_vertex_it};
      upper_appearing_edge = Edge{prev_vertex_it, event.vertex_it};
    }
    else
    {
      lower_right_vertex_it = prev_vertex_it;
      upper_right_vertex_it = next_vertex_it;

      lower_appearing_edge = Edge{prev_vertex_it, event.vertex_it};
      upper_appearing_edge = Edge{event.vertex_it, next_vertex_it};
    }

    Node& new_node = *(nodes_it_++);
    new_node.direction = HorizontalDirection::left;
    new_node.type = NodeType::leaf;
    new_node.vertex_it = event.vertex_it;
    new_node.lower_opp_edge = lower_appearing_edge;
    new_node.upper_opp_edge = upper_appearing_edge;
    new_node.neighbors[0] = nullptr;
    new_node.neighbors[1] = nullptr;
    new_node.neighbors[2] = nullptr;

    // The current corner is a convex reflex corner. A new region begins between the two outgoing edges, but since this
    // is a region which starts in a convex reflex corner, it doesn't have a 'region_left_node'.
    //
    // The region above the upper outgoing edge is not part of the region we're decomposing.
    active_edges_.insert(insert_location_it, {{event.vertex_it, lower_right_vertex_it, &new_node, 0},
                                              {event.vertex_it, upper_right_vertex_it, nullptr, 0}});
  }
}

void SweepState::handle_vanish_event(const Event& event)
{
  ActiveEdgesIt removal_it = active_edges_.begin();
  for (; removal_it != active_edges_.end(); removal_it++)
  {
    if (removal_it->right_vertex_it == event.vertex_it)
    {
      break;
    }
  }

  if (event.is_concave_corner)
  {
    // The vertex is the point where two regions meet and continue as a single region, when going towards the right.

    ActiveEdge& lower_opp_edge = *(removal_it - 1);
    ActiveEdge& upper_opp_edge = *(removal_it + 2);
    ActiveEdge& upper_vanishing_edge = *(removal_it + 1);

    Node& node = *(nodes_it_++);
    node.direction = HorizontalDirection::left;
    node.type = NodeType::branch;
    node.vertex_it = event.vertex_it;
    node.lower_opp_edge = lower_opp_edge.edge();
    node.upper_opp_edge = upper_opp_edge.edge();
    node.neighbors[0] = nullptr;
    node.neighbors[1] = lower_opp_edge.region_left_node;
    node.neighbors[2] = upper_vanishing_edge.region_left_node;

    lower_opp_edge.region_left_node->neighbors[lower_opp_edge.region_left_node_branch_index] = &node;
    upper_vanishing_edge.region_left_node->neighbors[upper_vanishing_edge.region_left_node_branch_index] = &node;

    lower_opp_edge.region_left_node = &node;
    lower_opp_edge.region_left_node_branch_index = 0;
  }
  else
  {
    ActiveEdge& lower_vanishing_edge = *removal_it;
    ActiveEdge& upper_vanishing_edge = *(removal_it + 1);

    Node& node = *(nodes_it_++);
    node.direction = HorizontalDirection::right;
    node.type = NodeType::leaf;
    node.vertex_it = event.vertex_it;
    node.lower_opp_edge = lower_vanishing_edge.edge();
    node.upper_opp_edge = upper_vanishing_edge.edge();
    node.neighbors[0] = lower_vanishing_edge.region_left_node;
    node.neighbors[1] = nullptr;
    node.neighbors[2] = nullptr;

    lower_vanishing_edge.region_left_node->neighbors[lower_vanishing_edge.region_left_node_branch_index] = &node;
  }

  // If the current vertex is convex corner, then it's the end of the region between the two incoming edges. The
  // relevant pointer in the current region's 'region_left_node' neighbors array is already set to nullptr, so doesn't
  // need to be updated. The parts above and below the lower and upper incoming edge are not part of the region we're
  // decomposing, so nothing needs to be done for that either.

  active_edges_.erase(removal_it, removal_it + 2);
}

void SweepState::handle_transition_event(const Event& event)
{
  for (ActiveEdge& active_edge : active_edges_)
  {
    if (active_edge.right_vertex_it == event.vertex_it)
    {
      active_edge.left_vertex_it = active_edge.right_vertex_it;
      active_edge.right_vertex_it = event.outgoing_towards_right ? next_cyclic(vertices_, active_edge.right_vertex_it)
                                                                 : prev_cyclic(vertices_, active_edge.right_vertex_it);
      break;
    }
  }
}

SweepState::ActiveEdgesIt SweepState::insert_location(Point2 vertex)
{
  size_t range_begin, range_end;
  if (decomposition_type_ == VerticalDecompositionType::interior_decomposition)
  {
    range_begin = 0;
    range_end = active_edges_.size();
  }
  else
  {
    // If we're decomposing the exterior region, then the first and last edge in the active segment list are the edges
    // at infinity. Since we already know that 'vertex' lies between these two edges, we can exclude them from the
    // search range.
    range_begin = 1;
    range_end = active_edges_.size() - 1;
  }

  while (range_begin != range_end)
  {
    size_t range_mid = (range_begin + range_end) / 2;
    const ActiveEdge& mid_edge = active_edges_[range_mid];
    if (cross(*mid_edge.right_vertex_it - *mid_edge.left_vertex_it, vertex - *mid_edge.left_vertex_it) < 0)
    {
      range_end = range_mid;
    }
    else
    {
      range_begin = range_mid + 1;
    }
  }

  return active_edges_.begin() + range_begin;
}

Edge SweepState::ActiveEdge::edge() const
{
  // One of the following 2 holds:
  //
  //  1. next_cyclic(vertices, left_vertex_it) == right_vertex_it
  //  2. next_cyclic(vertices, right_vertex_it) == left_vertex_it
  //

  if (left_vertex_it + 1 == right_vertex_it)
  {
    // Case 1 holds, without wrapping.
    return Edge{left_vertex_it, right_vertex_it};
  }
  else if (right_vertex_it + 1 == left_vertex_it)
  {
    // Case 2 holds, without wrapping.
    return Edge{right_vertex_it, left_vertex_it};
  }
  else
  {
    // There was wrapping, so one of the two vertices must be vertices.last() while the other one must be
    // vertices.first(). vertices.last() is the first vertex of the edge, while vertices.last = next_cyclic(vertices,
    // vertices.last()) the second vertex.
    return left_vertex_it < right_vertex_it ? Edge{right_vertex_it, left_vertex_it}
                                            : Edge{left_vertex_it, right_vertex_it};
  }
}

} // namespace

VerticalDecomposition vertical_decomposition_with_sweep_line_builder(ArrayView<const Point2> vertices,
                                                                     VerticalDecompositionType decomposition_type)
{
  SweepState sweep_state(vertices, decomposition_type);
  sweep_state.init_sweep();
  sweep_state.run_sweep();

  VerticalDecomposition result;
  result.nodes = sweep_state.move_out_nodes();
  result.leftmost_node = &result.nodes.front();
  result.rightmost_node = &result.nodes.back();
  return result;
}

} // namespace dida::detail::vertical_decomposition