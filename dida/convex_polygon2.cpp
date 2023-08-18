#include "dida/convex_polygon2.hpp"

namespace dida
{

bool validate_convex_polygon_vertices(ArrayView<const Point2> vertices)
{
  if (vertices.size() < 3)
  {
    return false;
  }

  bool left_side_vertex_found = false;
  bool right_side_vertex_found = false;

  Point2 cur_vertex = vertices[vertices.size() - 1];
  Vector2 incoming = cur_vertex - vertices[vertices.size() - 2];
  for (Point2 next_vertex : vertices)
  {
    Vector2 outgoing = next_vertex - cur_vertex;

    if (cross(incoming, outgoing) <= 0)
    {
      return false;
    }

    if (incoming.x() <= 0 && outgoing.x() > 0)
    {
      if (left_side_vertex_found)
      {
        return false;
      }

      left_side_vertex_found = true;
    }

    if (incoming.x() >= 0 && outgoing.x() < 0)
    {
      if (right_side_vertex_found)
      {
        return false;
      }

      right_side_vertex_found = true;
    }

    cur_vertex = next_vertex;
    incoming = outgoing;
  }

  return true;
}

} // namespace dida