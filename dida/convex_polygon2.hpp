#pragma once

#include <memory>
#include <utility>
#include <vector>

#include "dida/array_view.hpp"
#include "dida/point2.hpp"

namespace dida
{

class ConvexPolygonView2;

/// A 2D convex polygon.
///
/// @tparam Storage the underlying storage. This must be a container with elements of type @c Point.
template <class Storage>
class ConvexPolygon2T
{
public:
  using const_iterator = typename Storage::const_iterator;

  /// Constructs an uninitialized @c ConvexPolygon2T.
  ConvexPolygon2T() = default;

  ///@{
  /// Constructs a @c ConvexPolygon2T with the given vertices.
  ///
  /// It's checked using @c DIDA_ASSERT that the vertices form a valid convex polygon. See @c
  /// validate_convex_polygon_vertices for the conditions for a valid convex polygon.
  ///
  /// @param vertices The vertices.
  explicit ConvexPolygon2T(Storage vertices);
  ConvexPolygon2T(std::initializer_list<Point2> vertices);
  ///@}

  /// Constructs a @c ConvexPolygon2T with the given vertices.
  ///
  /// This function is unsafe in the sense that it's not checked by default whether the vertices in @c vertices form a
  /// valid convex polygon. It's still checked if @c DIDA_DEBUG_ASSERT is enabled though. See @c
  /// validate_convex_polygon_vertices for the conditions for a valid convex polygon.
  ///
  /// @param vertices The vertices.
  /// @return The convex polygon.
  static ConvexPolygon2T unsafe_from_vertices(Storage vertices);

  /// Returns a @c ConvexPolygonView2 into this @c ConvexPolygon2T.
  ///
  /// Since the @c ConvexPolygonView2 doesn't own the data it refers to, it's the responsibility of the user to make
  /// sure that this @c ConvexPolygon2T outlives the @c ConvexPolygonView2.
  ///
  /// @return The @c ConvexPolygonView2.
  operator ConvexPolygonView2() const;

  /// Returns an @c ArrayView of the vertices of this @c ConvexPolygon2T.
  ///
  /// Since the @c ArrayView doesn't own the data it refers to, it's the responsibility of the user to make sure that
  /// this @c ConvexPolygon2T outlives the @c ArrayView.
  ///
  /// @return The view of the vertices.
  operator ArrayView<const Point2>() const;

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

  /// Gives mutable access to vertices of this @c ConvexPolygon2T.
  ///
  /// This function is unsafe because it's possible to modify the vertices in such a way that the vertices don't form a
  /// valid convex polygon anymore. It's the responsibility of the user to guarantee that this doesn't happen.
  ///
  /// @return A mutable reference to the vertices of this @c ConvexPolygon2T.
  Storage& unsafe_mutable_vertices();

private:
  Storage vertices_;
};

using Triangle2 = ConvexPolygon2T<std::array<Point2, 3U>>;
using ConvexQuad2 = ConvexPolygon2T<std::array<Point2, 4U>>;
using ConvexPolygon2 = ConvexPolygon2T<std::vector<Point2>>;

/// A view into a convex polygon.
class ConvexPolygonView2
{
public:
  using const_iterator = ArrayView<Point2>::const_iterator;

  /// Constructs a @c ConvexPolygonView2 with the given vertices.
  ///
  /// It's checked using @c DIDA_ASSERT that the vertices form a valid convex polygon. See @c
  /// validate_convex_polygon_vertices for the conditions for a valid convex polygon.
  ///
  /// Since a @c ConvexPolygonView2 doesn't own the data it refers to, it's the responsibility of the user to make sure
  /// that the vertices outlive the @c ConvexPolygonView2.
  ///
  /// @param vertices The vertices.
  inline explicit ConvexPolygonView2(ArrayView<const Point2> vertices);

  /// Constructs a @c ConvexPolygonView2 with the given vertices.
  ///
  /// This function is unsafe in the sense that it's not checked by default whether the vertices in @c vertices form a
  /// valid convex polygon. It's still checked if @c DIDA_DEBUG_ASSERT is enabled though. See @c
  /// validate_convex_polygon_vertices for the conditions for a valid convex polygon.
  ///
  /// Since a @c ConvexPolygonView2 doesn't own the data it refers to, it's the responsibility of the user to make sure
  /// that the vertices outlive the @c ConvexPolygonView2.
  ///
  /// @param vertices The vertices.
  /// @return The convex polygon.
  static inline ConvexPolygonView2 unsafe_from_vertices(ArrayView<const Point2> vertices);

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
  ConvexPolygonView2() = default;

  ArrayView<const Point2> vertices_;
};

/// Validate whether the the vertices in @c vertices form a valid convex polygon.
///
/// It's considered a valid convex polygon if it satisfies the following conditions:
///
///  1. There are at least 3 vertices.
///  2. All vertices are distinct.
///  3. Each corner is strictly convex, the interior angle at any vertex is less than 180 degrees.
///  4. The polygon winds around its interior exactly once in the counter clockwise direction.
///
/// @param vertices The vertices to validate.
/// @return True iff @c vertices for a valid convex polygon.
bool validate_convex_polygon_vertices(ArrayView<const Point2> vertices);

} // namespace dida

#include "dida/convex_polygon2.inl"