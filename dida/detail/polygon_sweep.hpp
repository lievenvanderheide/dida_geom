#pragma once

#include "dida/array_view.hpp"
#include "dida/point2.hpp"

namespace dida::detail::polygon_sweep
{

using VerticesView = ArrayView<const Point2>;
using VertexIt = VerticesView::const_iterator;

/// A sweep line event.
struct Event
{
  /// The vertex corresponding to the event.
  VertexIt vertex_it;

  /// Whether the incoming edge at this event's vertex is a rightward edge, that is, whether
  /// <tt>lex_less_than(prev_vertex, vertex)</tt>.
  bool incoming_towards_right;

  /// Whether the outgoing edge at this event's vertex is a rightward edge, that is, whether
  /// <tt>lex_less_than(vertex, next_vertex)</tt>.
  bool outgoing_towards_right;

  /// If this event's vertex is a reflex corner (which is the case if <tt>incoming_towards_right !=
  /// outgoing_towards_right</tt>), then this indicates if it's a concave corner, that is, if the interior angle is
  /// greater than 180 degrees.
  ///
  /// This value is not set if the event's vertex isn't a reflex corner.
  bool is_concave_corner;
};

/// The sweep line events.
struct Events
{
  /// The sweep line events, sorted by to the order they occur during the sweep.
  std::vector<Event> events;

  /// The number of reflex vertices in the input polygon.
  size_t num_reflex_vertices;

  /// Constructs an @c Events object for a polygon with the given vertices.
  ///
  /// It's assumed that there are at least 3 vertices, and that all consecutive vertices are distinct. It's not a
  /// violation if the polygon has self-intersections (though in that case, care should be taken during the sweep,
  /// because certain assumptions the algorithm might make may not hold).
  inline Events(VerticesView vertices);
};

/// An edge which currently intersects the sweep line.
///
/// @tparam ActiveEdgeBase The base struct this @c ActiveEdge should derive from. This base struct can be used to
/// associated user data with each active edge.
template <class ActiveEdgeBase>
struct ActiveEdge : ActiveEdgeBase
{
  /// The left vertex of this edge.
  VertexIt left_vertex_it;

  /// The right vertex of this edge.
  VertexIt right_vertex_it;

  /// Returns whether @c point lies strictly below this edge.
  ///
  /// @param point The point.
  /// @return True iff @c point lies strictly below this edge.
  bool point_below_edge(Point2 point) const;

  /// Returns whether @c point lies strictly above this edge.
  ///
  /// @param point The point.
  /// @return True iff @c point lies strictly above this edge.
  bool point_above_edge(Point2 point) const;
};

/// An list of active edges.
template <class ActiveEdgeBase>
using ActiveEdges = std::vector<ActiveEdge<ActiveEdgeBase>>;

/// An iterator into the list of active edges.
template <class ActiveEdgeBase>
using ActiveEdgesIt = typename ActiveEdges<ActiveEdgeBase>::iterator;

/// A struct with state of a polygon sweep.
template <class ActiveEdgeBase>
struct SweepState
{
  /// The vertices of the polygon we're sweeping over.
  VerticesView vertices;

  /// The current list of active edges.
  ActiveEdges<ActiveEdgeBase> active_edges;
};

/// The result of the @c insert_location function.
///
/// @tparam ActiveEdgeBase The @c ActiveEdge base struct.
template <class ActiveEdgeBase>
struct InsertLocation
{
  /// The location in the active edges list where the new edges should be inserted.
  ActiveEdgesIt<ActiveEdgeBase> location_it;

  /// Whether @c vertex lies on one or more active edges. If true, then @c location_it points to the location
  /// immediately after the edge(s) which @c vertex is on.
  bool on_edge;
};

/// Returns the location in the sweep state's active edges list where new edges eminating from @c vertex should be
/// inserted.
///
/// @tparam ActiveEdgeBase The @c ActiveEdge base struct.
/// @param sweep_state The sweep state.
/// @param vertex The left vertex of the edges to be inserted.
/// @return The @c InsertLocation.
template <class ActiveEdgeBase>
InsertLocation<ActiveEdgeBase> insert_location(SweepState<ActiveEdgeBase>& sweep_state, Point2 vertex);

/// An overload of @c insert_location with a limited search range.
///
/// @tparam ActiveEdgeBase The @c ActiveEdge base struct.
/// @param sweep_state The sweep state.
/// @param range_begin The index of the beginning of the range.
/// @param range_end The index of the (exclusive) end of the range.
/// @param vertex The left vertex of the edges to be inserted.
/// @return The @c InsertLocation.
template <class ActiveEdgeBase>
InsertLocation<ActiveEdgeBase> insert_location(SweepState<ActiveEdgeBase>& sweep_state, size_t range_begin,
                                               size_t range_end, Point2 vertex);

/// Returns an iterator pointing to the first active edge whose right vertex is the vertex pointed to by @c
/// right_vertex_it. It's an error if active edge with the given right vertex exists in the active edges list.
///
/// @tparam ActiveEdgeBase The @c ActiveEdge base struct.
/// @param sweep_state The sweep state.
/// @param right_vertex_it The iterator pointing to the right vertex to search for.
/// @return An iterator pointing to the first active edge with the given right vertex.
template <class ActiveEdgeBase>
ActiveEdgesIt<ActiveEdgeBase> active_edge_with_right_vertex(SweepState<ActiveEdgeBase>& sweep_state,
                                                            VertexIt right_vertex_it);

/// Handles an appear event. An appear event is an event for which @c incoming_towards_right is @c false, and @c
/// outgoing_towards_right is @c true.
///
/// @tparam ActiveEdgeBase The @c ActiveEdge base struct.
/// @param sweep_state The sweep state.
/// @param loc The location in the active edges list where the new edges should appear. This should be the
/// result of @c insert_location with the vertex of @c event.
/// @param event The event.
/// @return An iterator to the lower one of the two new edges.
template <class ActiveEdgeBase>
ActiveEdgesIt<ActiveEdgeBase> handle_appear_event(SweepState<ActiveEdgeBase>& sweep_state,
                                                  InsertLocation<ActiveEdgeBase> loc, const Event& event);

/// Handles a transition event. A transition event is an event for which <tt>incoming_towards_right == outgoing_towards
/// right</tt>.
///
/// @tparam ActiveEdgeBase The @c ActiveEdge base struct.
/// @param sweep_state The sweep state.
/// @param active_edge_it An iterator to the active edge whose right vertex is the vertex where the event occurs. This
/// should be the result of @c active_edge_with_right_vertex with the vertex of @c event.
/// @param event The event.
template <class ActiveEdgeBase>
void handle_transition_event(SweepState<ActiveEdgeBase>& sweep_state, ActiveEdgesIt<ActiveEdgeBase> active_edge_it,
                             const Event& event);

/// Handles a vanish event. A vanish event is an event for which @c incoming_towards_right is @c true and @c
/// outgoing_towards_right is @c false.
///
///
/// @tparam ActiveEdgeBase The @c ActiveEdge base struct.
/// @param sweep_state The sweep state.
/// @param active_edge_it An iterator to the lower one of the two active edges whose right vertex is the vertex where
/// the event occurs. This should be the result of @c active_edge_with_right_vertex with the vertex of @c event.
template <class ActiveEdgeBase>
void handle_vanish_event(SweepState<ActiveEdgeBase>& sweep_state, ActiveEdgesIt<ActiveEdgeBase> active_edge_it);

} // namespace dida::detail::polygon_sweep

#include "dida/detail/polygon_sweep.inl"