#include "dida/assert.hpp"

namespace dida
{

template <class Storage>
Polygon2T<Storage>::Polygon2T(Storage vertices) : vertices_(std::move(vertices))
{
  DIDA_ASSERT(validate_polygon_vertices(vertices_));
}

template <class Storage>
Polygon2T<Storage>::Polygon2T(std::initializer_list<Point2> vertices) : vertices_(vertices)
{
  DIDA_ASSERT(validate_polygon_vertices(vertices_));
}

template <class Storage>
Polygon2T<Storage> Polygon2T<Storage>::unsafe_from_vertices(Storage vertices)
{
  DIDA_DEBUG_ASSERT(validate_polygon_vertices(vertices));

  Polygon2T result;
  result.vertices_ = std::move(vertices);
  return result;
}

template <class Storage>
Polygon2T<Storage>::operator PolygonView2() const
{
  return PolygonView2::unsafe_from_vertices(vertices_);
}

template <class Storage>
Polygon2T<Storage>::operator ArrayView<const Point2>() const
{
  return ArrayView<const Point2>(vertices_);
}

template <class Storage>
size_t Polygon2T<Storage>::size() const
{
  return vertices_.size();
}

template <class Storage>
Point2 Polygon2T<Storage>::operator[](size_t index) const
{
  return vertices_[index];
}

template <class Storage>
typename Polygon2T<Storage>::const_iterator Polygon2T<Storage>::begin() const
{
  return vertices_.begin();
}

template <class Storage>
typename Polygon2T<Storage>::const_iterator Polygon2T<Storage>::end() const
{
  return vertices_.end();
}

template <class Storage>
Storage& Polygon2T<Storage>::unsafe_mutable_vertices()
{
  return vertices_;
}

PolygonView2::PolygonView2(ArrayView<const Point2> vertices) : vertices_(vertices)
{
  DIDA_ASSERT(validate_polygon_vertices(vertices));
}

PolygonView2 PolygonView2::unsafe_from_vertices(ArrayView<const Point2> vertices)
{
  DIDA_DEBUG_ASSERT(validate_polygon_vertices(vertices));

  PolygonView2 result;
  result.vertices_ = vertices;
  return result;
}

PolygonView2::operator ArrayView<const Point2>() const
{
  return vertices_;
}

size_t PolygonView2::size() const
{
  return vertices_.size();
}

Point2 PolygonView2::operator[](size_t index) const
{
  return vertices_[index];
}

PolygonView2::const_iterator PolygonView2::begin() const
{
  return vertices_.begin();
}

PolygonView2::const_iterator PolygonView2::end() const
{
  return vertices_.end();
}

} // namespace dida