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

template <Arc arc, class Callbacks>
void find_on_arc_crossing_points(const PolygonInfo& a_info, ForwardEdge& a_edge, const PolygonInfo& b_info,
                                 ForwardEdge& b_edge, bool a_is_inner, Callbacks& callbacks)
{
  while (true)
  {
    if (sweep_position_less_than<arc, PerturbationVector2::right_up>(a_edge.end_it->x(), b_edge.end_it->x()))
    {
      // Advance 'b'.

      // cross(b_dir, a_end - s * a_dir - b_end) = 0
      // s = cross(b_dir, a_end - b_end) / cross(b_dir, a_dir)
      Vector2 ends_diff = *a_edge.end_it - *b_edge.end_it;
      ScalarDeg2 s_num = cross(b_edge.dir, ends_diff);
      bool a_end_is_inner = s_num > 0 || (s_num == 0 && cross_is_positive(b_edge.dir, PerturbationVector2::left_down));
      if (a_is_inner != a_end_is_inner)
      {
        // cross(a_dir, b_end - t * b_dir - a_end) = 0
        // t = cross(a_dir, b_end - a_end) / cross(a_dir, b_dir)
        ScalarDeg2 t_num = cross(a_edge.dir, ends_diff);
        ScalarDeg2 denom = cross(b_edge.dir, a_edge.dir);

        callbacks.crossing_point(a_edge, b_edge, s_num, t_num, denom);

        a_is_inner = a_end_is_inner;
      }

      if (!advance_forward_edge<arc>(a_info, a_edge))
      {
        return;
      }
    }
    else
    {
      // Advance 'a'.

      // cross(a_dir, b_end - t * b_dir - a_end) = 0
      // t = cross(a_dir, b_end - a_end) / cross(a_dir, b_dir)
      Vector2 ends_diff = *b_edge.end_it - *a_edge.end_it;
      ScalarDeg2 t_num = cross(a_edge.dir, ends_diff);
      bool b_end_is_inner = t_num > 0 || (t_num == 0 && cross_is_positive(a_edge.dir, PerturbationVector2::right_up));
      if (a_is_inner == b_end_is_inner)
      {
        // cross(b_dir, a_end - s * a_dir - b_end) = 0
        // s = cross(b_dir, b_end - a_end) / cross(a_dir, b_dir)
        ScalarDeg2 s_num = cross(b_edge.dir, ends_diff);
        ScalarDeg2 denom = cross(a_edge.dir, b_edge.dir);

        callbacks.crossing_point(a_edge, b_edge, s_num, t_num, denom);

        a_is_inner = !a_is_inner;
      }

      if (!advance_forward_edge<arc>(b_info, b_edge))
      {
        return;
      }
    }
  }
}

} // namespace convex_polygons_intersection

} // namespace dida::detail