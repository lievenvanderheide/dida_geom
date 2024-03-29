#include "dida/assert.hpp"

namespace dida
{

template <class Storage>
ConvexPolygon2T<Storage>::ConvexPolygon2T(Storage vertices) : vertices_(std::move(vertices))
{
  DIDA_ASSERT(validate_convex_polygon_vertices(vertices_));
}

template <class Storage>
ConvexPolygon2T<Storage>::ConvexPolygon2T(std::initializer_list<Point2> vertices) : vertices_(vertices)
{
  DIDA_ASSERT(validate_convex_polygon_vertices(vertices_));
}

template <class Storage>
ConvexPolygon2T<Storage> ConvexPolygon2T<Storage>::unsafe_from_vertices(Storage vertices)
{
  DIDA_DEBUG_ASSERT(validate_convex_polygon_vertices(vertices));

  ConvexPolygon2T result;
  result.vertices_ = std::move(vertices);
  return result;
}

template <class Storage>
std::optional<ConvexPolygon2T<Storage>> ConvexPolygon2T<Storage>::try_construct_from_vertices(Storage vertices)
{
  if (!validate_convex_polygon_vertices(vertices))
  {
    return std::nullopt;
  }

  ConvexPolygon2T result;
  result.vertices_ = std::move(vertices);
  return result;
}

template <class Storage>
ConvexPolygon2T<Storage>::operator ConvexPolygonView2() const
{
  return ConvexPolygonView2::unsafe_from_vertices(vertices_);
}

template <class Storage>
ConvexPolygon2T<Storage>::operator ArrayView<const Point2>() const
{
  return ArrayView<const Point2>(vertices_);
}

template <class Storage>
size_t ConvexPolygon2T<Storage>::size() const
{
  return vertices_.size();
}

template <class Storage>
Point2 ConvexPolygon2T<Storage>::operator[](size_t index) const
{
  return vertices_[index];
}

template <class Storage>
typename ConvexPolygon2T<Storage>::const_iterator ConvexPolygon2T<Storage>::begin() const
{
  return vertices_.begin();
}

template <class Storage>
typename ConvexPolygon2T<Storage>::const_iterator ConvexPolygon2T<Storage>::end() const
{
  return vertices_.end();
}

template <class Storage>
Storage& ConvexPolygon2T<Storage>::unsafe_mutable_vertices()
{
  return vertices_;
}

ConvexPolygonView2::ConvexPolygonView2(ArrayView<const Point2> vertices)
{
  DIDA_ASSERT(validate_convex_polygon_vertices(vertices));

  vertices_ = vertices;
}

ConvexPolygonView2 ConvexPolygonView2::unsafe_from_vertices(ArrayView<const Point2> vertices)
{
  DIDA_DEBUG_ASSERT(validate_convex_polygon_vertices(vertices));

  ConvexPolygonView2 result;
  result.vertices_ = vertices;
  return result;
}

} // namespace dida