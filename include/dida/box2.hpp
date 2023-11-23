#pragma once

#include "dida/point2.hpp"

namespace dida
{

/// A 2D point using @c ScalarDeg1 coordinates.
class Box2
{
public:
  /// Constructs an uninitialized @c Box2.
  Box2() = default;

  /// Constructs a @c Box2 with the given @c min and @c max corners.
  ///
  /// @param min The min corner.
  /// @param max The max corner.
  inline Box2(Point2 min, Point2 max);

  /// Returns the min corner of this @c Box2.
  ///
  /// @return The min corner.
  inline Point2 min() const;

  /// Returns the max corner of this @c Box2.
  ///
  /// @return The max corner.
  inline Point2 max() const;

  /// Returns the diagonal (the vector from the min corner to the max corner) of this @c Box2.
  ///
  /// @return The diagonal.
  inline Vector2 diag() const;

private:
  Point2 min_;
  Point2 max_;
};

/// Writes @c v to the output stream @c s.
///
/// @param s The output stream.
/// @param v The @c Box2 to write to the @c s.
/// @return A reference to the output stream.
std::ostream& operator<<(std::ostream& s, Box2 v);

} // namespace dida

#include "dida/box2.inl"