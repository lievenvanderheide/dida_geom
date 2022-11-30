#include "dida/area.hpp"

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

} // namespace dida