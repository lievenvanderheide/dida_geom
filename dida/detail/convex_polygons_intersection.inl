#include "dida/convex_polygon2_utils.hpp"
#include "dida/utils.hpp"

namespace dida::detail
{

namespace convex_polygons_intersection
{

constexpr Arc other_arc(Arc arc)
{
  return arc == Arc::lower ? Arc::upper : Arc::lower;
}

PolygonInfo::PolygonInfo(ConvexPolygonView2 _polygon) : polygon(_polygon)
{
  leftmost_it = leftmost_vertex(polygon);
  rightmost_it = rightmost_vertex(polygon);
}

template <Arc arc, PerturbationVector2 b_perturbation_vector>
bool sweep_position_less_than(ScalarDeg1 a, ScalarDeg1 b)
{
  if constexpr (arc == Arc::lower)
  {
    if constexpr (b_perturbation_vector == PerturbationVector2::left_down)
    {
      return a < b;
    }
    else
    {
      return a <= b;
    }
  }
  else
  {
    if constexpr (b_perturbation_vector == PerturbationVector2::left_down)
    {
      return a >= b;
    }
    else
    {
      return a > b;
    }
  }
}

template <Arc arc>
ConvexPolygonView2::const_iterator arc_first_vertex_it(const PolygonInfo& polygon_info)
{
  if constexpr (arc == Arc::lower)
  {
    return polygon_info.leftmost_it;
  }
  else
  {
    return polygon_info.rightmost_it;
  }
}

template <Arc arc>
ConvexPolygonView2::const_iterator arc_last_vertex_it(const PolygonInfo& polygon_info)
{
  if constexpr (arc == Arc::lower)
  {
    return polygon_info.rightmost_it;
  }
  else
  {
    return polygon_info.leftmost_it;
  }
}

template <Arc arc>
ForwardEdge arc_first_forward_edge(const PolygonInfo& polygon_info)
{
  ConvexPolygonView2::const_iterator start_it = arc_first_vertex_it<arc>(polygon_info);
  ConvexPolygonView2::const_iterator end_it = next_cyclic(polygon_info.polygon, start_it);
  return ForwardEdge{end_it, *end_it - *start_it};
}

template <Arc arc>
ReverseEdge arc_first_reverse_edge(const PolygonInfo& polygon_info)
{
  ConvexPolygonView2::const_iterator end_it = arc_last_vertex_it<arc>(polygon_info);
  ConvexPolygonView2::const_iterator start_it = prev_cyclic(polygon_info.polygon, end_it);
  return ReverseEdge{start_it, *end_it - *start_it};
}

template <Arc arc, PerturbationVector2 point_perturbation_vector>
ForwardEdge forward_edge_for_sweep_position(const PolygonInfo& polygon_info, ScalarDeg1 sweep_position)
{
  ConvexPolygonView2::const_iterator start_it = arc_first_vertex_it<arc>(polygon_info);
  while (true)
  {
    ConvexPolygonView2::const_iterator end_it = next_cyclic(polygon_info.polygon, start_it);
    if (!sweep_position_less_than<arc, point_perturbation_vector>(end_it->x(), sweep_position))
    {
      return ForwardEdge{end_it, *end_it - *start_it};
    }

    start_it = end_it;

    DIDA_DEBUG_ASSERT(start_it != arc_last_vertex_it<arc>(polygon_info));
  }
}

template <Arc arc, PerturbationVector2 point_perturbation_vector>
ReverseEdge reverse_edge_for_sweep_position(const PolygonInfo& polygon_info, ScalarDeg1 sweep_position)
{
  ConvexPolygonView2::const_iterator end_it = arc_last_vertex_it<arc>(polygon_info);
  while (true)
  {
    ConvexPolygonView2::const_iterator start_it = prev_cyclic(polygon_info.polygon, end_it);
    if (sweep_position_less_than<arc, point_perturbation_vector>(start_it->x(), sweep_position))
    {
      return ReverseEdge{start_it, *end_it - *start_it};
    }

    end_it = start_it;

    DIDA_DEBUG_ASSERT(start_it != arc_first_vertex_it<arc>(polygon_info));
  }
}

template <Arc arc>
bool advance_forward_edge(const PolygonInfo& polygon_info, ForwardEdge& edge)
{
  if (edge.end_it == arc_last_vertex_it<arc>(polygon_info))
  {
    return false;
  }

  ConvexPolygonView2::const_iterator start_it = edge.end_it;
  edge.end_it = next_cyclic(polygon_info.polygon, start_it);
  edge.dir = *edge.end_it - *start_it;
  return true;
}

template <Arc arc>
bool advance_reverse_edge(const PolygonInfo& polygon_info, ReverseEdge& edge)
{
  if (edge.start_it == arc_first_vertex_it<arc>(polygon_info))
  {
    return false;
  }

  ConvexPolygonView2::const_iterator end_it = edge.start_it;
  edge.start_it = prev_cyclic(polygon_info.polygon, end_it);
  edge.dir = *end_it - *edge.start_it;
  return true;
}

} // namespace convex_polygons_intersection

} // namespace dida::detail