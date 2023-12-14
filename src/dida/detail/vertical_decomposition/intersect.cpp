#include "dida/detail/vertical_decomposition/vertical_decomposition.hpp"
#include "dida/polygon2.hpp"

namespace dida::detail::vertical_decomposition
{

namespace
{

/// The part of @c IntersectState associated with one of the input polygons.
///
/// Conceptually, each polygon has a traversal point (which is never actually computed), which gradually moves over the
/// boundary of its polygon. The current state of the polygon relates to this traversal point.
struct PolygonIntersectState
{
  /// The vertices of this input polygon.
  VerticesView vertices;

  /// The current edge of this input polygon. This is the edge which contains this polygon's current traversal point.
  Edge edge;

  /// The previous node which was encountered by this polygon's traversal point.
  const Node* prev;

  /// The next node which will be encountered by this polygon's traversal point.
  const Node* next;
};

struct IntersectState
{
  /// The state of the input polygon whose boundary we're traversing in the default winding direction (so counter
  /// clockwise around its interior).
  PolygonIntersectState a;

  /// The state of the input polygon whose boundary we're traversing in the opposite winding direction (so clockwise
  /// around its interior).
  PolygonIntersectState b;

  /// The first node of the polygon of state @c b. This is used to detect when the traversal went all the way around
  /// polygon @c b, without finding an intersection.
  ///
  /// This can be @c nullptr if it's known up-front that the traversal can't go all the way around.
  Node* b_first_node;

  /// The current traversal direction.
  HorizontalDirection direction;
};

/// The intermediate result returned by the various iterate function.
enum class IntermediateResult
{
  /// Indicates that an intersection was found.
  intersect,

  /// Indicates that the traversal point of polygon @c b went all the way around its polygon, without finding an
  /// intersection.
  b_first_node_reached,

  /// Indicates that the traversal points of the two polygons reached a point where it wasn't possible to continue while
  /// still facing each other.
  ///
  /// When this is returned, then it may still be necessary to start a new traversal from same startpoint, but in the
  /// opposite direction.
  separated,

  /// Indicates that we should continue iteration.
  continue_iterating,
};

/// A pair of an edge on the lower boundary and an edge on the upper boundary of a region.
struct EdgePair
{
  /// The lower edge.
  Edge lower_edge;

  /// The upper edge.
  Edge upper_edge;
};

/// The return type of the @c find_initial_region function.
struct InitialRegion
{
  /// The region which contains @c b_leftmost.
  Region region;

  /// An edge pair with the edges of @c region wich are immediately below and above @c b_leftmost.
  EdgePair edge_pair;
};

/// Implements the @c intersect function, while expecting the leftmost vertex of polygon @c a to be to the left of the
/// leftmost vertex of polygon @c b.
bool intersect_ordered(VerticesView a_vertices, const VerticalDecomposition& a_exterior_decomposition,
                       VerticesView b_vertices, const VerticalDecomposition& b_exterior_decomposition);

/// Finds the initial region of polygon @c a, given the leftmost vertex of polygon @c b. If @c b_leftmost lies inside @c
/// a, then @c std::nullopt is returned.
///
/// It is assumed that <tt>a_leftmost <= b_leftmost <= a_rightmost</tt>.
std::optional<InitialRegion>
find_initial_region(VerticesView a_vertices, const VerticalDecomposition& a_exterior_decomposition, Point2 b_leftmost);

/// Checks whether @c point is inside @c region, and if so, returns the @c EdgePair with the two boundary edges
/// immediately above and immediately below @c point (both can be @c Edge::invalid(), if the region extends to infinity
/// in the corresponding direction). If @c point doesn't lie inside region, @c std::nullopt is returned.
std::optional<EdgePair> is_point_in_region(VerticesView vertices, Region region, Point2 point);

/// Implements the mainloop of the intersection test. This functions expects a fully initialized @c state, and keeps
/// iterating until the intermediate result is something other than @c IntermediateResult::continue_iterating.
IntermediateResult intersect_main_loop(IntersectState& state);

/// Advances the traversal to either @c state.a.next or @c state.b.next, depending on which one is reached first.
template <HorizontalDirection direction>
IntermediateResult intersect_iteration(IntersectState& state);

/// Advances past @c p.next, when the direction of @c p.next is equal to @c direction.
template <HorizontalDirection direction, bool p_is_lower>
IntermediateResult intersect_iteration_advance_forward_node(IntersectState& state, PolygonIntersectState& p,
                                                            PolygonIntersectState& q);

/// Advances past @c p.next, when the direction of @c p.next is opposite to @c direction.
template <HorizontalDirection direction, bool p_is_lower>
IntermediateResult intersect_iteration_advance_reverse_node(IntersectState& state, PolygonIntersectState& p,
                                                            PolygonIntersectState& q);

/// Advances the current edge of @c state to the next edge. @c advance_forward indicates whether to advance in the CCW
/// or CW direction.
template <HorizontalDirection direction, bool is_lower>
void advance_edge(PolygonIntersectState& state);

/// Returns the edge on the opposite boundary of the current region of @c p, such that the vertical line through @c
/// point intersects this edge.
template <HorizontalDirection direction, bool p_is_lower>
Edge opposite_edge_for_point(const PolygonIntersectState& p, Point2 point);

bool intersect_ordered(VerticesView a_vertices, const VerticalDecomposition& a_exterior_decomposition,
                       VerticesView b_vertices, const VerticalDecomposition& b_exterior_decomposition)
{
  if (lex_less_than(*a_exterior_decomposition.rightmost_node->vertex_it,
                    *b_exterior_decomposition.leftmost_node->vertex_it))
  {
    // 'a' is fully to the left of 'b', so there's no intersection.
    return false;
  }

  std::optional<InitialRegion> initial_region =
      find_initial_region(a_vertices, a_exterior_decomposition, *b_exterior_decomposition.leftmost_node->vertex_it);
  if (!initial_region)
  {
    // If the leftmost point of 'b' does not lie inside a region of the external decomposition of 'a', then
    // it must lie in the interior of 'a', so there's an intersection.
    return true;
  }

  if (initial_region->edge_pair.upper_edge.is_valid())
  {
    // Start a traversal which starts with b's leftmost vertex, while traversing a's boundary in the counter clockwise
    // direction and b's boundary in the clockwise direction.

    IntersectState state;
    state.a.vertices = a_vertices;
    state.a.edge = initial_region->edge_pair.upper_edge;
    state.a.prev = initial_region->region.left_node;
    state.a.next = initial_region->region.right_node;
    state.b.vertices = b_vertices;
    state.b.edge = Edge::incoming_edge(b_vertices, b_exterior_decomposition.leftmost_node->vertex_it);
    state.b.prev = b_exterior_decomposition.leftmost_node;
    state.b.next = b_exterior_decomposition.leftmost_node->neighbors[2];
    state.b_first_node = b_exterior_decomposition.leftmost_node;
    state.direction = HorizontalDirection::right;

    IntermediateResult result = intersect_main_loop(state);
    if (result != IntermediateResult::separated)
    {
      return result == IntermediateResult::intersect;
    }
  }

  // If the previous traversal ended with IntermediateResult::separated, or if it couldn't start because there was no
  // upper_edge in A's region (which can be considered an immediate separation), then we have to start another
  // traversal, which also starts with b's leftmost vertex, but this time traverses the boundaries in the opposite
  // direction.
  //
  // If there's no lower_edge, then there's nothing left to traverse, so there's no intersection.
  if (initial_region->edge_pair.lower_edge.is_valid())
  {
    IntersectState state;
    state.a.vertices = b_vertices;
    state.a.edge = Edge::outgoing_edge(b_vertices, b_exterior_decomposition.leftmost_node->vertex_it);
    state.a.prev = b_exterior_decomposition.leftmost_node;
    state.a.next = b_exterior_decomposition.leftmost_node->neighbors[1];
    state.b.vertices = a_vertices;
    state.b.edge = initial_region->edge_pair.lower_edge;
    state.b.prev = initial_region->region.left_node;
    state.b.next = initial_region->region.right_node;
    state.b_first_node = nullptr;
    state.direction = HorizontalDirection::right;

    IntermediateResult result = intersect_main_loop(state);
    return result == IntermediateResult::intersect;
  }
  else
  {
    return false;
  }
}

IntermediateResult intersect_main_loop(IntersectState& state)
{
  while (true)
  {
    IntermediateResult result = state.direction == HorizontalDirection::right
                                    ? intersect_iteration<HorizontalDirection::right>(state)
                                    : intersect_iteration<HorizontalDirection::left>(state);

    if (result != IntermediateResult::continue_iterating)
    {
      return result;
    }
  }
}

std::optional<InitialRegion>
find_initial_region(VerticesView a_vertices, const VerticalDecomposition& a_exterior_decomposition, Point2 b_leftmost)
{
  if (a_exterior_decomposition.leftmost_node->neighbors[1] == a_exterior_decomposition.leftmost_node->neighbors[2])
  {
    VertexIt a_leftmost_it = a_exterior_decomposition.leftmost_node->vertex_it;
    VertexIt a_rightmost_it = a_exterior_decomposition.rightmost_node->vertex_it;

    // Check if b_leftmost is in the region below 'a', and find the edge on the region's upper boundary above
    // b_leftmost.
    Edge upper_edge = edge_for_point_with_monotone_edge_range<HorizontalDirection::right>(
        a_vertices, EdgeRange{a_leftmost_it, a_rightmost_it}, b_leftmost);
    if (upper_edge.on_exterior_side<Winding::ccw>(b_leftmost))
    {
      return InitialRegion{
          Region{a_exterior_decomposition.leftmost_node, a_exterior_decomposition.rightmost_node},
          EdgePair{
              Edge::invalid(),
              upper_edge,
          },
      };
    }

    // Check if b_leftmost is in the region above 'a', and find the edge on the region's lower boundary below
    // b_leftmost.
    Edge lower_edge = edge_for_point_with_monotone_edge_range<HorizontalDirection::left>(
        a_vertices, EdgeRange{a_rightmost_it, a_leftmost_it}, b_leftmost);
    if (lower_edge.on_exterior_side<Winding::ccw>(b_leftmost))
    {
      return InitialRegion{
          Region{a_exterior_decomposition.leftmost_node, a_exterior_decomposition.rightmost_node},
          EdgePair{
              lower_edge,
              Edge::invalid(),
          },
      };
    }

    return std::nullopt;
  }

  RegionIterator it(a_exterior_decomposition.leftmost_node);
  do
  {
    Region a_region = it.region();
    std::optional<EdgePair> a_initial_edges = is_point_in_region(a_vertices, a_region, b_leftmost);
    if (a_initial_edges)
    {
      return InitialRegion{a_region, *a_initial_edges};
    }

  } while (it.move_next());

  return std::nullopt;
}

std::optional<EdgePair> is_point_in_region(VerticesView vertices, Region region, Point2 point)
{
  if (!lex_greater_than(point, *region.left_node->vertex_it) || !lex_less_than(point, *region.right_node->vertex_it))
  {
    return std::nullopt;
  }

  EdgePair result;

  EdgeRange lower_boundary = region.lower_boundary(VerticalDecompositionType::exterior_decomposition);
  if (lower_boundary.is_valid())
  {
    result.lower_edge =
        edge_for_point_with_monotone_edge_range<HorizontalDirection::left>(vertices, lower_boundary, point);
    if (!result.lower_edge.on_exterior_side<Winding::ccw>(point))
    {
      return std::nullopt;
    }
  }
  else
  {
    result.lower_edge = Edge::invalid();
  }

  EdgeRange upper_boundary = region.upper_boundary(VerticalDecompositionType::exterior_decomposition);
  if (upper_boundary.is_valid())
  {
    result.upper_edge =
        edge_for_point_with_monotone_edge_range<HorizontalDirection::right>(vertices, upper_boundary, point);
    if (!result.upper_edge.on_exterior_side<Winding::ccw>(point))
    {
      return std::nullopt;
    }
  }
  else
  {
    result.upper_edge = Edge::invalid();
  }

  return result;
}

template <HorizontalDirection direction>
IntermediateResult intersect_iteration(IntersectState& state)
{
  const Node* next_node = lex_less_than_with_direction<direction>(*state.a.next->vertex_it, *state.b.next->vertex_it)
                              ? state.a.next
                              : state.b.next;

  while (true)
  {
    if (lex_less_than_with_direction<direction>(*state.a.edge.end_vertex_it, *state.b.edge.start_vertex_it))
    {
      if (!lex_less_than_with_direction<direction>(*state.a.edge.end_vertex_it, *next_node->vertex_it))
      {
        break;
      }

      if (!state.b.edge.on_exterior_side<Winding::ccw>(*state.a.edge.end_vertex_it))
      {
        return IntermediateResult::intersect;
      }

      state.a.edge.start_vertex_it = state.a.edge.end_vertex_it;
      state.a.edge.end_vertex_it = next_cyclic(state.a.vertices, state.a.edge.end_vertex_it);
    }
    else
    {
      if (!lex_less_than_with_direction<direction>(*state.b.edge.start_vertex_it, *next_node->vertex_it))
      {
        break;
      }

      if (!state.a.edge.on_exterior_side<Winding::ccw>(*state.b.edge.start_vertex_it))
      {
        return IntermediateResult::intersect;
      }

      state.b.edge.end_vertex_it = state.b.edge.start_vertex_it;
      state.b.edge.start_vertex_it = prev_cyclic(state.b.vertices, state.b.edge.start_vertex_it);
    }
  }

  if (next_node == state.a.next)
  {
    constexpr bool a_is_lower = direction == HorizontalDirection::left;
    return next_node->direction == direction
               ? intersect_iteration_advance_forward_node<direction, a_is_lower>(state, state.a, state.b)
               : intersect_iteration_advance_reverse_node<direction, a_is_lower>(state, state.a, state.b);
  }
  else
  {
    constexpr bool b_is_lower = direction == HorizontalDirection::right;
    return next_node->direction == direction
               ? intersect_iteration_advance_forward_node<direction, b_is_lower>(state, state.b, state.a)
               : intersect_iteration_advance_reverse_node<direction, b_is_lower>(state, state.b, state.a);
  }
}

template <HorizontalDirection direction, bool p_is_lower>
IntermediateResult intersect_iteration_advance_forward_node(IntersectState& state, PolygonIntersectState& p,
                                                            PolygonIntersectState& q)
{
  constexpr bool p_is_ccw = p_is_lower == (direction == HorizontalDirection::left);

  Point2 p_vertex = *p.next->vertex_it;

  if (p.next->type == NodeType::branch)
  {
    if (q.edge.on_exterior_side<Winding::ccw>(p_vertex))
    {
      p.edge = p_is_ccw ? Edge::outgoing_edge(p.vertices, p.next->vertex_it)
                        : Edge::incoming_edge(p.vertices, p.next->vertex_it);
      p.prev = p.next;
      p.next = p.next->neighbors[p_is_lower ? 2 : 1];
      return IntermediateResult::continue_iterating;
    }
    else
    {
      p.prev = p.next;
      p.next = p.next->neighbors[p_is_lower ? 1 : 2];
      return IntermediateResult::continue_iterating;
    }
  }
  else
  {
    DIDA_DEBUG_ASSERT(p.next->type == NodeType::leaf);

    // If p.next is a leaf node, and q continues past it, then there must necessarily be an intersection.
    return IntermediateResult::intersect;
  }
}

template <HorizontalDirection direction, bool p_is_lower>
IntermediateResult intersect_iteration_advance_reverse_node(IntersectState& state, PolygonIntersectState& p,
                                                            PolygonIntersectState& q)
{
  constexpr bool p_is_ccw = p_is_lower == (direction == HorizontalDirection::left);

  if (p.edge.start_vertex_it != p.next->vertex_it && p.edge.end_vertex_it != p.next->vertex_it)
  {
    // If p.next->vertex_it is not a vertex of p.edge, then this node corresponds to a vertex on the opposite boundary,
    // so we can just move past it.

    p.prev = p.next;
    p.next = p.next->neighbors[0];
    return IntermediateResult::continue_iterating;
  }

  Point2 p_vertex = *p.next->vertex_it;

  if (!q.edge.on_exterior_side<Winding::ccw>(p_vertex))
  {
    return IntermediateResult::intersect;
  }

  // Determine whether we should turn around and continue in the branch above/below the current one.
  Edge q_opp_edge = opposite_edge_for_point<direction, !p_is_lower>(q, p_vertex);
  Edge p_opp_edge = p_is_lower ? p.next->lower_opp_edge : p.next->upper_opp_edge;
  bool should_turn_around;
  if (p_opp_edge.is_valid() && q_opp_edge.is_valid())
  {
    YOnEdge y_on_p_opp_edge = y_on_edge_for_x(p_opp_edge.segment(), p_vertex.x());
    YOnEdge y_on_q_opp_edge = y_on_edge_for_x(q_opp_edge.segment(), p_vertex.x());
    should_turn_around = p_is_lower ? y_on_q_opp_edge > y_on_p_opp_edge : y_on_q_opp_edge < y_on_p_opp_edge;
  }
  else if (p_opp_edge.is_valid())
  {
    should_turn_around = false;
  }
  else if (q_opp_edge.is_valid())
  {
    should_turn_around = true;
  }
  else
  {
    return IntermediateResult::separated;
  }

  if (should_turn_around)
  {
    if (q_opp_edge.is_valid() && !q_opp_edge.on_exterior_side<Winding::ccw>(p_vertex))
    {
      return IntermediateResult::intersect;
    }

    if constexpr (!p_is_ccw)
    {
      if (p.next == state.b_first_node)
      {
        return IntermediateResult::b_first_node_reached;
      }
    }

    advance_edge<direction, p_is_lower>(p);

    p.prev = p.next;
    p.next = p.next->neighbors[p_is_lower ? 1 : 2];

    q.edge = q_opp_edge;
    std::swap(q.next, q.prev);

    state.direction = other_direction(direction);

    return IntermediateResult::continue_iterating;
  }
  else
  {
    p.edge = p_opp_edge;
    p.prev = p.next;
    p.next = p.next->neighbors[0];
    return IntermediateResult::continue_iterating;
  }
}

template <HorizontalDirection direction, bool is_lower>
void advance_edge(PolygonIntersectState& state)
{
  constexpr bool is_ccw = is_lower == (direction == HorizontalDirection::left);
  if constexpr (is_ccw)
  {
    state.edge.start_vertex_it = state.edge.end_vertex_it;
    state.edge.end_vertex_it = next_cyclic(state.vertices, state.edge.end_vertex_it);
  }
  else
  {
    state.edge.end_vertex_it = state.edge.start_vertex_it;
    state.edge.start_vertex_it = prev_cyclic(state.vertices, state.edge.start_vertex_it);
  }
}

template <HorizontalDirection direction, bool p_is_lower>
Edge opposite_edge_for_point(const PolygonIntersectState& p, Point2 point)
{
  const Node *left_node, *right_node;
  if constexpr (direction == HorizontalDirection::right)
  {
    left_node = p.prev;
    right_node = p.next;
  }
  else
  {
    left_node = p.next;
    right_node = p.prev;
  }

  if (left_node->type == NodeType::branch && left_node->neighbors[1] == left_node->neighbors[2])
  {
    // If the above condition holds (which happens when dealing with a monotone polygon), then we can't use tests like
    // left_node->neighbors[2] == right_node to determine whether we're in the lower or upper branch, so the code below
    // won't work. At the same time, we also have the guarantee that there's no opposite boundary, so we can just early
    // out, returning Edge::invalid().
    return Edge::invalid();
  }

  EdgeRange opp_edge_range;
  if constexpr (p_is_lower)
  {
    opp_edge_range.start_vertex_it =
        left_node->neighbors[1] == right_node ? left_node->vertex_it : left_node->upper_opp_edge.start_vertex_it;
    opp_edge_range.end_vertex_it =
        right_node->neighbors[1] == left_node ? right_node->vertex_it : right_node->upper_opp_edge.end_vertex_it;
  }
  else
  {
    opp_edge_range.start_vertex_it =
        right_node->neighbors[2] == left_node ? right_node->vertex_it : right_node->lower_opp_edge.start_vertex_it;
    opp_edge_range.end_vertex_it =
        left_node->neighbors[2] == right_node ? left_node->vertex_it : left_node->lower_opp_edge.end_vertex_it;
  }

  if (!opp_edge_range.is_valid())
  {
    return Edge::invalid();
  }

  constexpr HorizontalDirection opp_edge_range_direction =
      p_is_lower ? HorizontalDirection::right : HorizontalDirection::left;
  return edge_for_point_with_monotone_edge_range<opp_edge_range_direction>(p.vertices, opp_edge_range, point);
}

} // namespace

bool intersect(PolygonView2 a, const VerticalDecomposition& a_exterior_decomposition, PolygonView2 b,
               const VerticalDecomposition& b_exterior_decomposition)
{
  // The vertex ranges should be non-overlapping.
  DIDA_DEBUG_ASSERT(a.end() < b.begin() || b.end() < a.begin());

  if (lex_less_than(*a_exterior_decomposition.leftmost_node->vertex_it,
                    *b_exterior_decomposition.leftmost_node->vertex_it))
  {
    return intersect_ordered(a, a_exterior_decomposition, b, b_exterior_decomposition);
  }
  else
  {
    return intersect_ordered(b, b_exterior_decomposition, a, a_exterior_decomposition);
  }
}

} // namespace dida::detail::vertical_decomposition