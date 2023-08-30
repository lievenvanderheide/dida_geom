#include <algorithm>

namespace dida
{

template <class Storage>
void flip_horizontally(Polygon2T<Storage>& polygon)
{
  Storage& vertices = polygon.unsafe_mutable_vertices();
  for (Point2& vertex : vertices)
  {
    vertex = Point2(-vertex.x(), vertex.y());
  }
  std::reverse(vertices.begin(), vertices.end());
}

template <class Storage>
void flip_vertically(Polygon2T<Storage>& polygon)
{
  Storage& vertices = polygon.unsafe_mutable_vertices();
  for (Point2& vertex : vertices)
  {
    vertex = Point2(vertex.x(), -vertex.y());
  }
  std::reverse(vertices.begin(), vertices.end());
}

} // namespace dida