#include "dida/area.hpp"

#include "dida/detail/convex_polygons_intersection.hpp"

namespace dida
{

double area(ConvexPolygonView2 polygon)
{
  ScalarDeg2 twice_area(0);

  Point2 edge_start = polygon[polygon.size() - 1];
  for (Point2 edge_end : polygon)
  {
    twice_area += cross(static_cast<Vector2>(edge_start), static_cast<Vector2>(edge_end));
    edge_start = edge_end;
  }

  return .5 * static_cast<double>(twice_area);
}

namespace
{

/// A struct with info associated with an edge of one of the input polygons in the @c intersection_area implementation.
struct IntersectionAreaEdgeInfo
{
  /// The fraction of the edge which lies within the other polygon.
  double inner_fraction = 0;

  /// Whether the edge crosses an odd number of edges of the other polygon.
  bool odd_num_crossings = false;
};

/// An implementation of the @c find_crossing_points callbacks for @c intersection_area.
class IntersectionAreaCallbacks
{
public:
  using ForwardEdge = detail::convex_polygons_intersection::ForwardEdge;

  /// Constructs a @c IntersectionAreaCallbacks instance for the given polygons.
  ///
  /// The provided @c IntersectionAreaEdgeInfo arrays should hold an element for each edge of their respective polygon.
  ///
  /// @param a The first polygon.
  /// @param b The second polygon.
  /// @param a_edge_infos The edge infos for polygon @c a.
  /// @param b_edge_infos The edge infos for polygon @c b.
  IntersectionAreaCallbacks(ConvexPolygonView2 a, ConvexPolygonView2 b,
                            ArrayView<IntersectionAreaEdgeInfo> a_edge_infos,
                            ArrayView<IntersectionAreaEdgeInfo> b_edge_infos);

  /// The function which is called for each crossing point.
  ///
  /// The parameter of the crossing point on each edge is <tt>s_num / denom</tt> and <tt>t_num / denom</tt>
  /// respectively, where an parameter of 0 corresponds to the _end_ vertex of the edge, and a parameter of 0 to the
  /// _start_ vertex of the edge.
  ///
  /// @param a_edge The first edge.
  /// @param b_edge The second edge.
  /// @param s_num The numerator of the parameter of the crossing point on edge @c a_edge.
  /// @param t_num The numerator of the parameter of the crossing point on edge @c b_edge.
  /// @param denom The denominator of both parameters.
  /// @param a_inner_to_outer If true, then the part of @c a_edge before the crossing point is inside polygon @c
  /// while the part after the crossing point is outside polygon @c b. If false, then it's the other way around.
  void crossing_point(const ForwardEdge& a_edge, const ForwardEdge& b_edge, ScalarDeg2 s_num, ScalarDeg2 t_num,
                      ScalarDeg2 denom, bool a_inner_to_outer);

private:
  ConvexPolygonView2 a_;
  ConvexPolygonView2 b_;
  ArrayView<IntersectionAreaEdgeInfo> a_edge_infos_;
  ArrayView<IntersectionAreaEdgeInfo> b_edge_infos_;
};

IntersectionAreaCallbacks::IntersectionAreaCallbacks(ConvexPolygonView2 a, ConvexPolygonView2 b,
                                                     ArrayView<IntersectionAreaEdgeInfo> a_edge_infos,
                                                     ArrayView<IntersectionAreaEdgeInfo> b_edge_infos)
    : a_(a), b_(b), a_edge_infos_(a_edge_infos), b_edge_infos_(b_edge_infos)
{
}

void IntersectionAreaCallbacks::crossing_point(const ForwardEdge& a_edge, const ForwardEdge& b_edge, ScalarDeg2 s_num,
                                               ScalarDeg2 t_num, ScalarDeg2 denom, bool a_inner_to_outer)
{
  double s = 1.0 - static_cast<double>(s_num) / static_cast<double>(denom);
  double t = 1.0 - static_cast<double>(t_num) / static_cast<double>(denom);

  IntersectionAreaEdgeInfo& a_edge_info = a_edge_infos_[a_edge.end_it - a_.begin()];
  IntersectionAreaEdgeInfo& b_edge_info = b_edge_infos_[b_edge.end_it - b_.begin()];

  // The inner_fracfion of an edge is equal to a sum of the form
  //
  //  (interval_0_end - interval_0_start) + (interval_1_end - interval_1_start) + ...
  //
  // Each crossing point either corresponds to the beginning or the end of the interval. If it's a crossing point at
  // the beginning of an interval, then its position should be added with negative sign, if it's at the end of an
  // interval its position should be added with positive sin.

  if (a_inner_to_outer)
  {
    // The crossing point ends an inner interval of a, and starts an inner interval of b.
    a_edge_info.inner_fraction += s;
    b_edge_info.inner_fraction -= t;
  }
  else
  {
    // The crossing point starts an inner interval of a, and ends an inner interval of b.
    a_edge_info.inner_fraction -= s;
    b_edge_info.inner_fraction += t;
  }

  a_edge_info.odd_num_crossings = !a_edge_info.odd_num_crossings;
  b_edge_info.odd_num_crossings = !b_edge_info.odd_num_crossings;
}

/// Returns twice the "area contribution" to the intersection area coming from edges of @c polygon. This "area
/// contribution" is the sum if the terms in the shoelace formula of the intersection area, which correspond to edges
/// of @c polygon.
///
/// @param polygon The polygon.
/// @param leftmost_index The index of the leftmost vertex of @c polygon.
/// @param edge_infos The edge infos corresponding to the edges of @c polygon.
/// @param leftmost_inside_other True iff the leftmost vertex of this polygon lies inside the other polygon.
/// @return Twice the "area contribution"
double twice_area_contribution(ConvexPolygonView2 polygon, size_t leftmost_index,
                               ArrayView<IntersectionAreaEdgeInfo> edge_infos, bool leftmost_inside_other)
{
  double result = 0;

  size_t edge_start_index = leftmost_index;
  bool edge_start_inside_other = leftmost_inside_other;
  for (size_t i = 0; i < polygon.size(); i++)
  {
    size_t edge_end_index = succ_modulo(edge_start_index, polygon.size());
    bool edge_end_inside_other = edge_start_inside_other != edge_infos[edge_end_index].odd_num_crossings;

    double inner_fraction = edge_infos[edge_end_index].inner_fraction;

    // If the end vertex of this edge is inside the other polygon, then there's an interval which ends at position 1, so
    // we need to add the position for this position too.
    //
    // Note that this is only necessary for the end vertex. If the start vertex of the edge is inside the other polygon
    // then it also corresponds to the beginning of an interval, but since the position of this end point is 0, adding
    // it has no effect.
    if (edge_end_inside_other)
    {
      inner_fraction += 1;
    }

    Vector2 edge_start = static_cast<Vector2>(polygon[edge_start_index]);
    Vector2 edge_end = static_cast<Vector2>(polygon[edge_end_index]);
    result += inner_fraction * static_cast<double>(cross(edge_start, edge_end));

    edge_start_index = edge_end_index;
    edge_start_inside_other = edge_end_inside_other;
  }

  return result;
}

double intersection_area(ConvexPolygonView2 a, ConvexPolygonView2 b, ArrayView<IntersectionAreaEdgeInfo> a_edge_infos,
                         ArrayView<IntersectionAreaEdgeInfo> b_edge_infos)
{
  using namespace detail::convex_polygons_intersection;

  IntersectionAreaCallbacks callbacks(a, b, a_edge_infos, b_edge_infos);

  PolygonInfo a_polygon_info(a);
  PolygonInfo b_polygon_info(b);
  FindCrossingPointsResult find_crossing_point_result = find_crossing_points(a_polygon_info, b_polygon_info, callbacks);
  if (find_crossing_point_result == FindCrossingPointsResult::disjoint)
  {
    return 0;
  }

  double a_twice_area_contribution =
      twice_area_contribution(a, static_cast<size_t>(a_polygon_info.leftmost_it - a.begin()), a_edge_infos,
                              find_crossing_point_result == FindCrossingPointsResult::a_leftmost_inside_b);
  double b_twice_area_contribution =
      twice_area_contribution(b, static_cast<size_t>(b_polygon_info.leftmost_it - b.begin()), b_edge_infos,
                              find_crossing_point_result == FindCrossingPointsResult::b_leftmost_inside_a);
  return .5 * (a_twice_area_contribution + b_twice_area_contribution);
}

} // namespace

double intersection_area(ConvexPolygonView2 a, ConvexPolygonView2 b)
{
  std::vector<IntersectionAreaEdgeInfo> a_edge_infos(a.size());
  std::vector<IntersectionAreaEdgeInfo> b_edge_infos(b.size());
  return intersection_area(a, b, a_edge_infos, b_edge_infos);
}

double intersection_over_union(ConvexPolygonView2 a, ConvexPolygonView2 b)
{
  double a_area = area(a);
  double b_area = area(b);
  double inters_area = intersection_area(a, b);
  double union_area = a_area + b_area - inters_area;
  return inters_area / union_area;
}

} // namespace dida