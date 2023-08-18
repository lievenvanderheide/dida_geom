#include <algorithm>

#include "dida/utils.hpp"

namespace dida::detail::polygon_sweep
{

Events::Events(VerticesView vertices)
{
  events.resize(vertices.size());
  num_side_vertices = 0;

  VertexIt prev_vertex_it = vertices.end() - 2;
  VertexIt cur_vertex_it = vertices.end() - 1;
  bool incoming_towards_right = lex_less_than(*prev_vertex_it, *cur_vertex_it);
  for (size_t i = 0; i < vertices.size(); i++)
  {
    VertexIt next_vertex_it = vertices.begin() + i;
    bool outgoing_towards_right = lex_less_than(*cur_vertex_it, *next_vertex_it);

    events[i].vertex_it = cur_vertex_it;
    events[i].incoming_towards_right = incoming_towards_right;
    events[i].outgoing_towards_right = outgoing_towards_right;

    if (incoming_towards_right != outgoing_towards_right)
    {
      ScalarDeg2 orientation = cross(*cur_vertex_it - *prev_vertex_it, *next_vertex_it - *cur_vertex_it);
      events[i].is_concave_corner = orientation < 0;

      num_side_vertices++;
    }

    incoming_towards_right = outgoing_towards_right;
    prev_vertex_it = cur_vertex_it;
    cur_vertex_it = next_vertex_it;
  }

  std::sort(events.begin(), events.end(),
            [](const Event& a, const Event& b) { return lex_less_than(*a.vertex_it, *b.vertex_it); });
}

template <class ActiveEdgeBase>
bool ActiveEdge<ActiveEdgeBase>::point_below_edge(Point2 point) const
{
  return cross(*right_vertex_it - *left_vertex_it, point - *left_vertex_it) < 0;
}

template <class ActiveEdgeBase>
bool ActiveEdge<ActiveEdgeBase>::point_above_edge(Point2 point) const
{
  return cross(*right_vertex_it - *left_vertex_it, point - *left_vertex_it) > 0;
}

template <class ActiveEdgeBase>
InsertLocation<ActiveEdgeBase> insert_location(SweepState<ActiveEdgeBase>& sweep_state, Point2 vertex)
{
  return insert_location(sweep_state, 0, sweep_state.active_edges.size(), vertex);
}

template <class ActiveEdgeBase>
InsertLocation<ActiveEdgeBase> insert_location(SweepState<ActiveEdgeBase>& sweep_state, size_t range_begin,
                                               size_t range_end, Point2 vertex)
{
  bool on_edge = false;

  while (range_begin != range_end)
  {
    size_t range_mid = (range_begin + range_end) / 2;
    const ActiveEdge<ActiveEdgeBase>& mid_edge = sweep_state.active_edges[range_mid];

    ScalarDeg2 side = cross(*mid_edge.right_vertex_it - *mid_edge.left_vertex_it, vertex - *mid_edge.left_vertex_it);
    if (side < 0)
    {
      range_end = range_mid;
    }
    else
    {
      range_begin = range_mid + 1;
      on_edge = side == 0;
    }
  }

  return InsertLocation<ActiveEdgeBase>{sweep_state.active_edges.begin() + range_begin, on_edge};
}

template <class ActiveEdgeBase>
ActiveEdgesIt<ActiveEdgeBase> active_edge_with_right_vertex(SweepState<ActiveEdgeBase>& sweep_state,
                                                            VertexIt right_vertex_it)
{
  for (ActiveEdgesIt<ActiveEdgeBase> it = sweep_state.active_edges.begin(); it != sweep_state.active_edges.end(); ++it)
  {
    if (it->right_vertex_it == right_vertex_it)
    {
      return it;
    }
  }

  DIDA_DEBUG_ASSERT(!"Invalid right_vertex_it");

  return sweep_state.active_edges.end();
}

template <class ActiveEdgeBase>
ActiveEdgesIt<ActiveEdgeBase> handle_appear_event(SweepState<ActiveEdgeBase>& sweep_state,
                                                  InsertLocation<ActiveEdgeBase> loc, const Event& event)
{
  VertexIt prev_vertex_it = prev_cyclic(sweep_state.vertices, event.vertex_it);
  VertexIt next_vertex_it = next_cyclic(sweep_state.vertices, event.vertex_it);

  ActiveEdge<ActiveEdgeBase> lower_appearing_edge;
  lower_appearing_edge.left_vertex_it = event.vertex_it;
  lower_appearing_edge.right_vertex_it = event.is_concave_corner ? prev_vertex_it : next_vertex_it;

  ActiveEdge<ActiveEdgeBase> upper_appearing_edge;
  upper_appearing_edge.left_vertex_it = event.vertex_it;
  upper_appearing_edge.right_vertex_it = event.is_concave_corner ? next_vertex_it : prev_vertex_it;

  return sweep_state.active_edges.insert(loc.location_it, {lower_appearing_edge, upper_appearing_edge});
}

template <class ActiveEdgeBase>
void handle_transition_event(SweepState<ActiveEdgeBase>& sweep_state, ActiveEdgesIt<ActiveEdgeBase> active_edge_it,
                             const Event& event)
{
  active_edge_it->left_vertex_it = active_edge_it->right_vertex_it;
  active_edge_it->right_vertex_it = event.outgoing_towards_right
                                        ? next_cyclic(sweep_state.vertices, active_edge_it->right_vertex_it)
                                        : prev_cyclic(sweep_state.vertices, active_edge_it->right_vertex_it);
}

template <class ActiveEdgeBase>
void handle_vanish_event(SweepState<ActiveEdgeBase>& sweep_state, ActiveEdgesIt<ActiveEdgeBase> active_edge_it)
{
  sweep_state.active_edges.erase(active_edge_it, active_edge_it + 2);
}

} // namespace dida::detail::polygon_sweep