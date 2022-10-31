#pragma once

#include <iosfwd>

#include "dida/scalar.hpp"

namespace dida
{

/// A 2D vector using @c ScalarDeg1 coordinates.
class Vector2
{
public:
  /// Constructs an uninitialized @c Vector2.
  Vector2() = default;

  /// Constructs a @c Vector2 with the given coordinates.
  ///
  /// @param x The x-coordinate.
  /// @param y The y-coordinate.
  inline Vector2(ScalarDeg1 x, ScalarDeg1 y);

  /// Constructs a @c Vector2 with the given coordinates. The coordinates will be rounded to the DidaGeom grid.
  ///
  /// @param x The x-coordinate.
  /// @param y The y-coordinate
  inline Vector2(double x, double y);

  /// Constructs a @c Vector2 with the given grid coordinates. Grid coordinates are coordinates where a distance of 1
  /// corresponds to a real distance of @c ScalarDeg1::quantum.
  ///
  /// @param x The x grid coordinate.
  /// @param y The y grid coordinate.
  /// @return The @c Vector2 with the given grid coordinates.
  static inline Vector2 from_grid_coordinates(int32_t x, int32_t y);

  /// Returns the x-coordinate of this @c Vector2.
  ///
  /// @return The x-coordinate.
  inline ScalarDeg1 x() const;

  /// Returns th y-coordinate of this @c Vector2.
  ///
  /// @return The y-coordinate.
  inline ScalarDeg1 y() const;

  /// Compares two vectors for equality.
  ///
  /// @param b The second operand.
  /// @return True iff the two vectors are equal.
  inline bool operator==(Vector2 b) const;

  /// Compares two vectors for inequality.
  ///
  /// @param b The second operand.
  /// @return True iff the two vectors are not qual.
  inline bool operator!=(Vector2 b) const;

  /// Adds this vector and vector @c b and returns the result.
  ///
  /// @param b The second operand.
  /// @return The sum of this vector and @c b.
  inline Vector2 operator+(Vector2 b) const;

  /// Subtracts @c b from this vector and returns the result.
  ///
  /// @param b The second operand.
  /// @return The difference of this vector and @c b.
  inline Vector2 operator-(Vector2 b) const;

  /// Negates this vector and returns the result.
  ///
  /// @return The negated vector.
  inline Vector2 operator-() const;

private:
  ScalarDeg1 coordinates_[2];
};

/// Returns the dot product of @c a and @c b.
///
/// @param a The first operand.
/// @param b The second operand.
/// @return The dot product.
inline ScalarDeg2 dot(Vector2 a, Vector2 b);

/// Returns the 2D cross product of @c a and @c b.
///
/// The 2D cross product is also known as the perp dot product, and is defined as
///
///  cross(a, b) = dot(left_perpendicular(a), b)
///
/// @param a The first operand.
/// @param b The second operand.
/// @return The cross product.
inline ScalarDeg2 cross(Vector2 a, Vector2 b);

/// Returns the left perpendicular of @c a. The left perpendicular is the vector obtained by rotating @c a by 90 degrees
/// in the counter clockwise direction.
///
/// @param a The vector whose left perpendicular to compute.
/// @return The left perpendicular of @p a.
inline Vector2 left_perpendicular(Vector2 a);

/// Returns the right perpendicular of @c a. The right perpendicular is the vector obtained by rotating @c a by 90
/// degrees in the clockwise direction.
///
/// @param a The vector whose right perpendicular to compute.
/// @return The right perpendicular of @p a.
inline Vector2 right_perpendicular(Vector2 a);

/// Writes @c v to the output stream @c s.
///
/// @param s The output stream.
/// @param v The @c Vector2 to write to the @c s.
/// @return A reference to the output stream.
std::ostream& operator<<(std::ostream& s, Vector2 v);

} // namespace dida

#include "dida/vector2.inl"