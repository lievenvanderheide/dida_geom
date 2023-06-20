#include "dida/predicates.hpp"

#include "dida/utils.hpp"

namespace dida
{

namespace
{

template <PerturbationVector2 b_perturbation_vector>
bool y_less_than_with_perturbation(ScalarDeg1 a_y, ScalarDeg1 b_y)
{
  if constexpr (b_perturbation_vector == PerturbationVector2::left_down)
  {
    return a_y < b_y;
  }
  else
  {
    return a_y <= b_y;
  }
}

template <PerturbationVector2 b_perturbation_vector>
bool on_right_side_of_edge(Point2 edge_lower_vertex, Point2 edge_upper_vertex, Point2 b)
{
  ScalarDeg2 side = cross(edge_upper_vertex - edge_lower_vertex, b - edge_lower_vertex);
  if constexpr (b_perturbation_vector == PerturbationVector2::left_down)
  {
    return side < 0;
  }
  else
  {
    return side <= 0;
  }
}

} // namespace

bool is_within(PolygonView2 polygon, Point2 point)
{
  bool result = false;

  Point2 v0 = polygon[polygon.size() - 1];
  for (Point2 v1 : polygon)
  {
    bool v0_below_ray = y_less_than_with_perturbation<PerturbationVector2::right_up>(v0.y(), point.y());
    bool v1_below_ray = y_less_than_with_perturbation<PerturbationVector2::right_up>(v1.y(), point.y());

    if (v0_below_ray && !v1_below_ray)
    {
      if (on_right_side_of_edge<PerturbationVector2::left_down>(v0, v1, point))
      {
        result = !result;
      }
    }
    else if (v1_below_ray && !v0_below_ray)
    {
      if (on_right_side_of_edge<PerturbationVector2::right_up>(v1, v0, point))
      {
        result = !result;
      }
    }

    if(v0 == point)
    {
      return true;
    }

    if(v0.y() == point.y() && v1.y() == point.y())
    {
      if(point.x() <= v0.x() && point.x() >= v1.x())
      {
        return true;
      }
    }

    v0 = v1;
  }

  return result;
}

template <PerturbationVector2 point_perturbation_vector>
bool is_within_with_perturbation(PolygonView2 polygon, Point2 point)
{
  bool result = false;

  Point2 v0 = polygon[polygon.size() - 1];
  for (Point2 v1 : polygon)
  {
    bool v0_below_ray = y_less_than_with_perturbation<point_perturbation_vector>(v0.y(), point.y());
    bool v1_below_ray = y_less_than_with_perturbation<point_perturbation_vector>(v1.y(), point.y());

    if (v0_below_ray && !v1_below_ray)
    {
      if (on_right_side_of_edge<point_perturbation_vector>(v0, v1, point))
      {
        result = !result;
      }
    }
    else if (v1_below_ray && !v0_below_ray)
    {
      if (on_right_side_of_edge<point_perturbation_vector>(v1, v0, point))
      {
        result = !result;
      }
    }

    v0 = v1;
  }

  return result;
}

template bool is_within_with_perturbation<PerturbationVector2::left_down>(PolygonView2, Point2);
template bool is_within_with_perturbation<PerturbationVector2::right_up>(PolygonView2, Point2);

} // namespace dida