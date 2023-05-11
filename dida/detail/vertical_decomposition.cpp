#include "dida/detail/vertical_decomposition.hpp"

namespace dida::detail::vertical_decomposition
{

YOnEdge y_on_edge_for_x(Segment2 edge, ScalarDeg1 x)
{
  DIDA_DEBUG_ASSERT(edge.start().x() != edge.end().x());
  
  // start_x + t * dir_x = x
  // t = (x - start_x) / dir_x
  // y = start_y + t * dir_y
  // y = start_y + (x - start_x) / dir_x * dir_y
  // y = (start_y * dir_x + (x - start_x) * dir_y) / dir_x

  Point2 start = edge.start();
  Vector2 dir = edge.end() - edge.start();

  ScalarDeg2 num = start.y() * dir.x() + (x - start.x()) * dir.y();
  ScalarDeg1 denom = dir.x();
  if (denom < 0)
  {
    num = -num;
    denom = -denom;
  }

  return YOnEdge(num, denom);
}

}