#include "dida/polygon2.hpp"

#include <ostream>

#include "dida/detail/polygon_sweep.hpp"

namespace dida
{

bool validate_polygon_vertices(ArrayView<const Point2> vertices)
{
  using namespace detail::polygon_sweep;

  if (vertices.size() < 3)
  {
    return false;
  }

  // Make sure there are no duplicated vertices.
  Point2 prev_vertex = vertices[vertices.size() - 1];
  for (Point2 vertex : vertices)
  {
    if (prev_vertex == vertex)
    {
      return false;
    }

    prev_vertex = vertex;
  }

  // Use a sweep line based algorithm to determine that there are no self intersections.

  Events events(vertices);

  if (events.events.front().is_concave_corner)
  {
    // Incorrect winding.
    return false;
  }

  struct ActiveEdgeBase
  {
  };

  SweepState<ActiveEdgeBase> sweep_state{vertices, {}};

  for (const Event& event : events.events)
  {
    if (event.incoming_towards_right == event.outgoing_towards_right)
    {
      ActiveEdgesIt<ActiveEdgeBase> it = active_edge_with_right_vertex(sweep_state, event.vertex_it);

      if (it != sweep_state.active_edges.begin() && !(it - 1)->point_above_edge(*event.vertex_it))
      {
        return false;
      }

      if (it + 1 != sweep_state.active_edges.end() && !(it + 1)->point_below_edge(*event.vertex_it))
      {
        return false;
      }

      handle_transition_event(sweep_state, it, event);
    }
    else if (event.outgoing_towards_right)
    {
      InsertLocation<ActiveEdgeBase> appear_location = insert_location(sweep_state, *event.vertex_it);
      if (appear_location.on_edge)
      {
        return false;
      }

      handle_appear_event(sweep_state, appear_location, event);
    }
    else
    {
      ActiveEdgesIt<ActiveEdgeBase> it = active_edge_with_right_vertex(sweep_state, event.vertex_it);

      DIDA_DEBUG_ASSERT(it != sweep_state.active_edges.end());

      if (it->right_vertex_it != (it + 1)->right_vertex_it)
      {
        return false;
      }

      if (it != sweep_state.active_edges.begin() && !(it - 1)->point_above_edge(*event.vertex_it))
      {
        return false;
      }

      if (it + 2 != sweep_state.active_edges.end() && !(it + 2)->point_below_edge(*event.vertex_it))
      {
        return false;
      }

      handle_vanish_event(sweep_state, it);
    }
  }

  return true;
}

std::ostream& operator<<(std::ostream& s, PolygonView2 polygon)
{
  s << "{";
  for (size_t i = 0; i < polygon.size() - 1; i++)
  {
    s << polygon[i] << ", ";
  }
  s << polygon[polygon.size() - 1] << "}";
  return s;
}

} // namespace dida