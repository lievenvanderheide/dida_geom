#pragma once

#include <memory>
#include <utility>
#include <vector>

#include "dida/array_view.hpp"
#include "dida/point2.hpp"

namespace dida
{

class PolygonView2;

/// A 2D cpolygon.
///
/// @tparam Storage the underlying storage. This must be a container with elements of type @c Point.
template <class Storage>
class Polygon2T
{
public:
  using const_iterator = typename Storage::const_iterator;

  /// Constructs an uninitialized @c Polygon2T.
  Polygon2T() = default;

  ///@{
  /// Constructs a @c Polygon2T with the given vertices.
  ///
  /// It's checked using @c DIDA_ASSERT that the vertices form a valid polygon. See @c validate_polygon_vertices for the
  /// conditions for a valid polygon.
  ///
  /// @param vertices The vertices.
  explicit Polygon2T(Storage vertices);
  Polygon2T(std::initializer_list<Point2> vertices);
  ///@}

  /// Constructs a @c Polygon2T with the given vertices.
  ///
  /// This function is unsafe in the sense that it's not checked by default whether the vertices in @c vertices form a
  /// valid polygon. It's still checked if @c DIDA_DEBUG_ASSERT is enabled though. See @c
  /// validate_polygon_vertices for the conditions for a valid polygon.
  ///
  /// @param vertices The vertices.
  /// @return The convex polygon.
  static Polygon2T unsafe_from_vertices(Storage vertices);

  /// Returns a @c PolygonView2 into this @c Polygon2T.
  ///
  /// Since the @c PolygonView2 doesn't own the data it refers to, it's the responsibility of the user to make sure that
  /// this @c Polygon2T outlives the @c PolygonView2.
  ///
  /// @return The @c PolygonView2.
  operator PolygonView2() const;

  /// Returns the number of vertices of this polygon.
  ///
  /// @return The number of vertices.
  size_t size() const;

  /// Returns the vertex at @c index.
  ///
  /// @returns The vertex.
  Point2 operator[](size_t index) const;

  /// Returns an iterator to the beginning of the vertex list.
  ///
  /// @return The iterator.
  const_iterator begin() const;

  /// Returns an iterator to the end of the vertex list.
  ///
  /// @return The iterator.
  const_iterator end() const;

  /// Gives mutable access to vertices of this @c Polygon2T.
  ///
  /// This function is unsafe because it's possible to modify the vertices in such a way that the vertices don't form a
  /// valid polygon anymore. It's the responsibility of the user to guarantee that this doesn't happen.
  ///
  /// @return A mutable reference to the vertices of this @c Polygon2T.
  Storage& unsafe_mutable_vertices();

private:
  Storage vertices_;
};

using Polygon2 = Polygon2T<std::vector<Point2>>;

/// A view into a polygon.
class PolygonView2
{
public:
  using const_iterator = ArrayView<Point2>::const_iterator;

  /// Constructs a @c PolygonView2 with the given vertices.
  ///
  /// It's checked using @c DIDA_ASSERT that the vertices form a valid polygon. See @c validate_polygon_vertices for the
  /// conditions for a valid polygon.
  ///
  /// Since a @c PolygonView2 doesn't own the data it refers to, it's the responsibility of the user to make sure that
  /// the vertices outlive the @c PolygonView2.
  ///
  /// @param vertices The vertices.
  inline explicit PolygonView2(ArrayView<const Point2> vertices);

  /// Constructs a @c PolygonView2 with the given vertices.
  ///
  /// This function is unsafe in the sense that it's not checked by default whether the vertices in @c vertices form a
  /// valid polygon. It's still checked if @c DIDA_DEBUG_ASSERT is enabled though. See @c validate_polygon_vertices for
  /// the conditions for a valid polygon.
  ///
  /// Since a @c PolygonView2 doesn't own the data it refers to, it's the responsibility of the user to make sure that
  /// the vertices outlive the @c PolygonView2.
  ///
  /// @param vertices The vertices.
  /// @return The polygon.
  static inline PolygonView2 unsafe_from_vertices(ArrayView<const Point2> vertices);

  /// Returns the number of vertices of this polygon.
  ///
  /// @return The number of vertices.
  inline size_t size() const;

  /// Returns the vertex at @c index.
  ///
  /// @returns The vertex.
  inline Point2 operator[](size_t index) const;

  /// Returns an iterator to the beginning of the vertex list.
  ///
  /// @return The iterator.
  inline const_iterator begin() const;

  /// Returns an iterator to the end of the vertex list.
  ///
  /// @return The iterator.
  inline const_iterator end() const;

private:
  PolygonView2() = default;

  ArrayView<const Point2> vertices_;
};

/// Validate whether the the vertices in @c vertices form a valid polygon.
///
/// It's considered a valid polygon if it satisfies the following conditions:
///
///  1. There are at least 3 vertices.
///  2. All vertices are distinct.
///  3. The polygon has counter-clockwise winding.
///  4. There are no self-intersections.
///
/// @param vertices The vertices to validate.
/// @return True iff @c vertices for a valid polygon.
bool validate_polygon_vertices(ArrayView<const Point2> vertices);

} // namespace dida

#include "dida/polygon2.inl"