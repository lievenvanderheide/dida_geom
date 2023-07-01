#include "dida/detail/vertical_decomposition_operations.hpp"

#include "dida/predicates.hpp"

namespace dida::detail::vertical_decomposition
{

namespace
{

/// The following invariants hold, while assuming w.l.o.g. that <tt>sweep_direction == HorizontalDirection::right</tt>,
/// and @c < is @c lex_less_than.
///
///  - a_edge.left_vertex, b_edge.left_vertex < a_edge.right_vertex, b_edge.right_vertex
///  - a_edge.left_vertex, b_edge.left_vertex < a_next_node.vertex
///  - a_prev_node.vertex < a_edge.right_vertex, b_edge.right_vertex
///
/// The current sweep position is defined as max(a_edge.left_vertex, b_edge.left_vertex).
///
struct PolygonsIntersectState
{
  /// The first polygon. This polygon is traversed in reverse order.
  PolygonView2 a;

  /// The decomposition of the exterior of @c a.
  const VerticalDecomposition& a_exterior_decomposition;

  /// The second polygon. This polygon is traversed in forward order.
  PolygonView2 b;

  /// The current sweep direction.
  HorizontalDirection sweep_direction;

  /// The node whose vertical extension is the boundary of the current region in the direction we're moving away from
  /// (ie. if <tt>sweep_direction == HorizontalDirection::right</tt>, then this is the node on the left side of the
  /// current region).
  const Node* a_prev_node;

  /// The node whose vertical extension is the boundary of the current region in the direction we're moving towards (ie.
  /// if <tt>sweep_position == HorizontalDirection::right</tt>, then this is the node on the right side of the current
  /// region).
  const Node* a_next_node;

  /// The current edge of polygon @c a.
  ///
  /// If the sweep direction is towards the right, then this is an upward facing edge, if it's towards the left, it's a
  /// downward facing edge.
  Edge a_edge;

  /// If up to date, then this is the edge on the opposite boundary to the boundary containing @c a_edge, which
  /// corresponds to the current sweep position.
  ///
  /// This edge will often lag behind though, and is only advanced to the current sweep position right before being
  /// used.
  Edge a_opp_edge;

  /// The current edge of polygon @c b.
  ///
  /// If the sweep direction is towards the right, then this is a downward facing edge, if it's towards the left, it's
  /// an upward facing edge.
  Edge b_edge;
};

/// The return value of the @c init and @c iteration functions which either holds the result of the intersection test,
/// if it's already known, or @c continue_iterating if more calls to @c iteration are necessary.
enum class IterationsResult
{
  /// The two polygons intersect.
  intersect,

  /// The two polygons are disjoint.
  disjoint,

  /// The algorithm wasn't able to determine whether the polygons intersect or not, so more calls to @c iteration are
  /// necessary.
  continue_iterating,
};

/// Initializes a @c PolygonsIntersectState. The @c a, @c a_vertical_decompositon and @c b fields must already be set,
/// all other members are initialized by this function.
IterationsResult init(PolygonsIntersectState& state);

/// Performs an iteration of the algorithm.
///
/// This function takes a @c sweep_direction parameter which must be equal to state.sweep_direction.
template <HorizontalDirection sweep_direction>
IterationsResult iteration(PolygonsIntersectState& state);

/// Implements the case of @c iteration where @c a_edge is advanced to the next edge.
template <HorizontalDirection sweep_direction>
IterationsResult iteration_advance_a_edge(PolygonsIntersectState& state);

/// Implements the case of @c iteration where @c b_edge is advanced to the next edge.
template <HorizontalDirection sweep_direction>
IterationsResult iteration_advance_b_edge(PolygonsIntersectState& state);

/// Implements the case of @c iteration where we're transitioning to a new region.
template <HorizontalDirection sweep_direction>
IterationsResult iteration_advance_region(PolygonsIntersectState& state);

/// Called by @c init or @c iteration when @c b_edge.start_vertex is outside the vertical decomposition. It will keep
/// advancing @c b_edge until it has entered a region of the vertical decomposition again, or until it can determine
/// that the polygons are intersecting/disjoint.
///
/// @c sweep_direction must be the direction into which we'd be entering the vertical decomposition, so if we're to the
/// left of it p
template <HorizontalDirection sweep_direction>
IterationsResult iterations_outside_decomposition(PolygonsIntersectState& state);

/// Advances @c a_opp_edge to the edge of the current region's opposite boundary which intersects the vertical line with
/// the given x-coordinate.
///
/// Assume w.l.o.g. that sweep_direction == HorizontalDirection::right: If the vertical line intersects one or more
/// vertices of the boundary, then the resulting edge is the edge whose end vertex is on the vertical line, and start
/// vertex is strictly to the left of it.
template <HorizontalDirection sweep_direction>
void advance_a_opp_edge(PolygonsIntersectState& state, ScalarDeg1 x);

/// Returns whether @c point is strictly on the exterior side of @c edge.
bool on_exterior_side(Edge edge, Point2 point);

/// Returns whether @c point is strictly on the interior side of @c edge.
bool on_interior_side(Edge edge, Point2 point);

/// An edge pair returned by @c is_point_in_region.
struct EdgePair
{
  /// The edge of the lower boundary of the region directly below the query point.
  Edge lower_edge;

  /// The edge of the upper boundary of the region directly above the query point.
  Edge upper_edge;
};

/// Determines whether @c point is inside @c region, and if so, returns the @c EdgePair containing the two edges below
/// and above @c point.
std::optional<EdgePair> is_point_in_region(VerticesView vertices, const Region& region, Point2 point);

IterationsResult init(PolygonsIntersectState& state)
{
  if (lex_less_than(state.b[0], *state.a_exterior_decomposition.leftmost_node->vertex_it))
  {
    state.sweep_direction = HorizontalDirection::right;
    state.a_prev_node = nullptr;
    state.a_next_node = state.a_exterior_decomposition.leftmost_node;
    state.a_edge = Edge::invalid();
    state.a_opp_edge = Edge::invalid();
    state.b_edge = Edge{state.b.begin(), state.b.begin() + 1};
    return iterations_outside_decomposition<HorizontalDirection::right>(state);
  }

  if (lex_less_than(*state.a_exterior_decomposition.rightmost_node->vertex_it, state.b[0]))
  {
    state.sweep_direction = HorizontalDirection::left;
    state.a_prev_node = nullptr;
    state.a_next_node = state.a_exterior_decomposition.rightmost_node;
    state.a_edge = Edge::invalid();
    state.a_opp_edge = Edge::invalid();
    state.b_edge = Edge{state.b.begin(), state.b.begin() + 1};
    return iterations_outside_decomposition<HorizontalDirection::left>(state);
  }

  RegionIterator region_iterator(&state.a_exterior_decomposition.nodes[0]);
  do
  {
    Region region = region_iterator.region();

    if (std::optional<EdgePair> edge_pair = is_point_in_region(ArrayView<const Point2>(state.a.begin(), state.a.size()),
                                                               region_iterator.region(), state.b[0]))
    {
      if (lex_less_than(state.b[0], state.b[1]))
      {
        state.sweep_direction = HorizontalDirection::right;
        state.a_prev_node = region.left_node;
        state.a_next_node = region.right_node;
        state.a_edge = edge_pair->lower_edge;
        state.a_opp_edge = edge_pair->upper_edge;
        state.b_edge = Edge{state.b.begin(), state.b.begin() + 1};
      }
      else
      {
        state.sweep_direction = HorizontalDirection::left;
        state.a_prev_node = region.right_node;
        state.a_next_node = region.left_node;
        state.a_edge = edge_pair->upper_edge;
        state.a_opp_edge = edge_pair->lower_edge;
        state.b_edge = Edge{state.b.begin(), state.b.begin() + 1};
      }

      return IterationsResult::continue_iterating;
    }

  } while (region_iterator.move_next());

  return IterationsResult::intersect;
}

template <HorizontalDirection sweep_direction>
IterationsResult iteration(PolygonsIntersectState& state)
{
  // Assuming w.l.o.g. that direction == HorizontalDirection::right.
  //
  // If both right vertices have the same x-coordinate, then the invariant for the next iteration requires that we
  // advance the edge with the lower y-coordinate. We know that either 'a_edge' is the edge with the lower right vertex,
  // or otherwise the two edges must cross in which case the algorithm terminates, and since a cross is detected
  // regardless of the edge we advance, it's safe to always advance 'b_edge'.

  if (state.a_edge.is_valid() &&
      !x_less_than_with_direction<sweep_direction>(state.b_edge.end_vertex_it->x(), state.a_edge.start_vertex_it->x()))
  {
    // If there's equality, then 'a_next_node.vertex' is either the right vertex of 'a_edge', or it has a lower
    // y-coordinate. Proof: Assume that it has a higher y-coordinate. Since a_edge is the upper edge, this is only
    // possible if 'a' is a reflex vertex which turns left, for otherwise the continuation of 'a' would block the
    // vertical extension coming from above. But if 'a' is a relfex vertex, then 'a' must have a node as well, which
    // comes before 'a_next_node', so this is a contradiction.

    if (x_less_than_with_direction<sweep_direction>(state.a_edge.start_vertex_it->x(),
                                                    state.a_next_node->vertex_it->x()))
    {
      return iteration_advance_a_edge<sweep_direction>(state);
    }
  }
  else
  {
    // Here the lexicographical compare is necessary, just comparing the x-coordinate is not enough.
    if (lex_less_than_with_direction<sweep_direction>(*state.b_edge.end_vertex_it, *state.a_next_node->vertex_it))
    {
      return iteration_advance_b_edge<sweep_direction>(state);
    }
  }

  return iteration_advance_region<sweep_direction>(state);
}

template <HorizontalDirection sweep_direction>
IterationsResult iteration_advance_a_edge(PolygonsIntersectState& state)
{
  if (!on_exterior_side(state.b_edge, *state.a_edge.start_vertex_it))
  {
    return IterationsResult::intersect;
  }

  state.a_edge.end_vertex_it = state.a_edge.start_vertex_it;
  state.a_edge.start_vertex_it = prev_cyclic(state.a, state.a_edge.start_vertex_it);
  return IterationsResult::continue_iterating;
}

template <HorizontalDirection sweep_direction>
IterationsResult iteration_advance_b_edge(PolygonsIntersectState& state)
{
  if (state.a_edge.is_valid() && !on_exterior_side(state.a_edge, *state.b_edge.end_vertex_it))
  {
    return IterationsResult::intersect;
  }

  state.b_edge.start_vertex_it = state.b_edge.end_vertex_it;
  state.b_edge.end_vertex_it = next_cyclic(state.b, state.b_edge.start_vertex_it);

  if (state.b_edge.start_vertex_it == state.b.begin())
  {
    advance_a_opp_edge<sweep_direction>(state, state.b_edge.start_vertex_it->x());
    return on_exterior_side(state.a_opp_edge, *state.b_edge.start_vertex_it) ? IterationsResult::disjoint
                                                                             : IterationsResult::intersect;
  }

  if (!lex_less_than_with_direction<sweep_direction>(*state.b_edge.start_vertex_it, *state.b_edge.end_vertex_it))
  {
    if (state.a_opp_edge.is_valid())
    {
      advance_a_opp_edge<sweep_direction>(state, state.b_edge.start_vertex_it->x());
      if (!on_exterior_side(state.a_opp_edge, *state.b_edge.start_vertex_it))
      {
        return IterationsResult::intersect;
      }
    }

    state.sweep_direction = other_direction(sweep_direction);
    std::swap(state.a_prev_node, state.a_next_node);
    std::swap(state.a_edge, state.a_opp_edge);
  }

  return IterationsResult::continue_iterating;
}

template <HorizontalDirection sweep_direction>
IterationsResult iteration_advance_region(PolygonsIntersectState& state)
{
  if (state.a_next_node->is_leaf)
  {
    return IterationsResult::intersect;
  }

  if (state.a_next_node->direction == sweep_direction)
  {
    if (!on_exterior_side(state.b_edge, *state.a_next_node->vertex_it))
    {
      // If we're going to the left, then 'b_edge_' is the lower edge, and the reflex vertex of 'a_next_node_' is
      // below 'b_edge_', so we should take the upper branch. If we're going to the right, then 'b_edge_' is the
      // upper edge, and the reflex vertex of 'a_next_node_' is above 'b_edge_', so we should take the lower branch.

      state.a_opp_edge.start_vertex_it = state.a_next_node->vertex_it;
      state.a_opp_edge.end_vertex_it = next_cyclic(state.a, state.a_opp_edge.start_vertex_it);

      state.a_prev_node = state.a_next_node;
      state.a_next_node = state.a_next_node->neighbors[sweep_direction == HorizontalDirection::left ? 2 : 1];
    }
    else
    {
      state.a_edge.end_vertex_it = state.a_next_node->vertex_it;
      state.a_edge.start_vertex_it = prev_cyclic(state.a, state.a_edge.end_vertex_it);

      state.a_opp_edge = sweep_direction == HorizontalDirection::left ? state.a_next_node->lower_opp_edge
                                                                      : state.a_next_node->upper_opp_edge;

      state.a_prev_node = state.a_next_node;
      state.a_next_node = state.a_next_node->neighbors[sweep_direction == HorizontalDirection::left ? 1 : 2];
    }
  }
  else
  {
    if (state.a_next_node->vertex_it == state.a_edge.start_vertex_it)
    {
      if (!on_exterior_side(state.b_edge, *state.a_next_node->vertex_it))
      {
        return IterationsResult::intersect;
      }
    }
    else
    {
      if (!on_interior_side(state.b_edge, *state.a_next_node->vertex_it))
      {
        return IterationsResult::intersect;
      }
    }

    state.a_prev_node = state.a_next_node;
    state.a_next_node = state.a_next_node->neighbors[0];

    if (state.sweep_direction == HorizontalDirection::left)
    {
      state.a_edge = state.a_prev_node->upper_opp_edge;
      state.a_opp_edge = state.a_prev_node->lower_opp_edge;
    }
    else
    {
      state.a_edge = state.a_prev_node->lower_opp_edge;
      state.a_opp_edge = state.a_prev_node->upper_opp_edge;
    }
  }

  if (!state.a_next_node)
  {
    state.a_next_node = state.a_prev_node;
    state.sweep_direction = other_direction(sweep_direction);
    return iterations_outside_decomposition<other_direction(sweep_direction)>(state);
  }

  return IterationsResult::continue_iterating;
}

template <HorizontalDirection sweep_direction>
IterationsResult iterations_outside_decomposition(PolygonsIntersectState& state)
{
  // The condition in the outer loop implies the condition in the outer loop, so this nested loop construct isn't
  // necessary for correctness. It does however lead to a substantial performance gain in cases when a big part of B is
  // outside A's decomposition.
  while (lex_less_than_with_direction<sweep_direction>(*state.b_edge.end_vertex_it, *state.a_next_node->vertex_it))
  {
    do
    {
      state.b_edge.start_vertex_it = state.b_edge.end_vertex_it;
      state.b_edge.end_vertex_it = next_cyclic(state.b, state.b_edge.start_vertex_it);

      if (state.b_edge.start_vertex_it == state.b.begin())
      {
        return IterationsResult::disjoint;
      }
    } while (x_less_than_with_direction<sweep_direction>(state.b_edge.end_vertex_it->x(),
                                                         state.a_next_node->vertex_it->x()));
  }

  if (!on_exterior_side(state.b_edge, *state.a_next_node->vertex_it))
  {
    state.a_edge = Edge::invalid();

    state.a_opp_edge.start_vertex_it = state.a_next_node->vertex_it;
    state.a_opp_edge.end_vertex_it = next_cyclic(state.a, state.a_next_node->vertex_it);

    state.a_prev_node = state.a_next_node;
    state.a_next_node = state.a_next_node->neighbors[sweep_direction == HorizontalDirection::left ? 2 : 1];
  }
  else
  {
    state.a_edge.start_vertex_it = prev_cyclic(state.a, state.a_next_node->vertex_it);
    state.a_edge.end_vertex_it = state.a_next_node->vertex_it;

    state.a_opp_edge = Edge::invalid();

    state.a_prev_node = state.a_next_node;
    state.a_next_node = state.a_next_node->neighbors[sweep_direction == HorizontalDirection::left ? 1 : 2];
  }

  return IterationsResult::continue_iterating;
}

template <HorizontalDirection sweep_direction>
void advance_a_opp_edge(PolygonsIntersectState& state, ScalarDeg1 x)
{
  DIDA_DEBUG_ASSERT(state.a_opp_edge.is_valid());

  while (x_less_than_with_direction<sweep_direction>(state.a_opp_edge.end_vertex_it->x(), x))
  {
    state.a_opp_edge.start_vertex_it = state.a_opp_edge.end_vertex_it;
    state.a_opp_edge.end_vertex_it = next_cyclic(state.a, state.a_opp_edge.end_vertex_it);
  }
}

bool on_exterior_side(Edge edge, Point2 point)
{
  Segment2 segment = edge.segment();
  ScalarDeg2 side = cross(segment.direction(), point - segment.start());
  return side < 0;
}

bool on_interior_side(Edge edge, Point2 point)
{
  Segment2 segment = edge.segment();
  ScalarDeg2 side = cross(segment.direction(), point - segment.start());
  return side > 0;
}

std::optional<EdgePair> is_point_in_region(VerticesView vertices, const Region& region, Point2 point)
{
  if (lex_less_than(point, *region.left_node->vertex_it))
  {
    return std::nullopt;
  }

  if (lex_less_than(*region.right_node->vertex_it, point))
  {
    return std::nullopt;
  }

  EdgePair result;

  EdgeRange lower_boundary = region.lower_boundary(VerticalDecompositionType::exterior_decomposition);
  if (lower_boundary.is_valid())
  {
    result.lower_edge =
        edge_for_point_with_monotone_edge_range<HorizontalDirection::left>(vertices, lower_boundary, point);
    if (!on_exterior_side(result.lower_edge, point))
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
    if (!on_exterior_side(result.upper_edge, point))
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

} // namespace

bool polygons_intersect(PolygonView2 a, const VerticalDecomposition& a_exterior_decomposition, PolygonView2 b)
{
  // Test if a[0] is within b, to catch the case where a is fully contained within b (in which case there are no
  // boundary intersections).
  if (is_within(b, a[0]))
  {
    return true;
  }

  PolygonsIntersectState state{a, a_exterior_decomposition, b};

  {
    IterationsResult result = init(state);
    if (result != IterationsResult::continue_iterating)
    {
      return result == IterationsResult::intersect;
    }
  }

  while (true)
  {
    IterationsResult result = state.sweep_direction == HorizontalDirection::left
                                  ? iteration<HorizontalDirection::left>(state)
                                  : iteration<HorizontalDirection::right>(state);
    if (result != IterationsResult::continue_iterating)
    {
      return result == IterationsResult::intersect;
    }
  }
}

} // namespace dida::detail::vertical_decomposition