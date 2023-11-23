#pragma once

#include "dida/vector2.hpp"

namespace dida
{

/// A 2D point using @c ScalarDeg1 coordinates.
class Point2
{
public:
  /// Constructs an uninitialized @c Point2.
  Point2() = default;

  /// Constructs a @c Point2 with the given coordinates.
  ///
  /// @param x The x-coordinate.
  /// @param y The y-coordinate.
  inline Point2(ScalarDeg1 x, ScalarDeg1 y);

  /// Constructs a @c Point2 with the given coordinates.
  ///
  /// @param x The x-coordinate.
  /// @param y The y-coordinate.
  inline Point2(double x, double y);

  /// Constructs a @c Point2 from a @c Vector2.
  ///
  /// @param position The position vector of the point.
  inline explicit Point2(Vector2 position);

  /// Converts this @c Point2 to a @c Vector2.
  //
  /// @return The position vector of this point.
  inline explicit operator Vector2() const;

  /// Returns the x-coordinate of this point.
  ///
  /// @return The x-coordinate.
  inline ScalarDeg1 x() const;

  /// Returns the y-coordinate of this point.
  ///
  /// @return The y-coordinate.
  inline ScalarDeg1 y() const;

  /// Compares two points for equality.
  ///
  /// @param b The second operand.
  /// @return True iff the two points are equal.
  inline bool operator==(Point2 b) const;

  /// Compares two points for inequality.
  ///
  /// @param b The second operand.
  /// @return True iff the two points are not equal.
  inline bool operator!=(Point2 b) const;

  /// Returns the difference of two points as a @c Vector2.
  ///
  /// @param b The second operand.
  /// @return The vector from this point to point @c b.
  inline Vector2 operator-(const Point2 b) const;

  /// Adds vector @c b to this point and returns the result.
  ///
  /// @param b The vector to add to this point.
  /// @return The result of the addition.
  inline Point2 operator+(Vector2 b) const;

  /// Subtracts vector @c b from this point and returns the result.
  ///
  /// @param b The vector to subtract from this point.
  /// @return The result of the subtraction.
  inline Point2 operator-(Vector2 b) const;

private:
  Vector2 position_;
};

/// Returns whether point @c a is lexicographically less than point @c b.
///
/// @param a The first operand.
/// @param b The second operand.
/// @return True iff @c a is lexicographically less than @c b.
inline bool lex_less_than(Point2 a, Point2 b);

/// Returns whether point @c a is lexicographically greater than point @c b.
///
/// @param a The first operand.
/// @param b The second operand.
/// @return True iff @c a is lexicographically greater than @c b.
inline bool lex_greater_than(Point2 a, Point2 b);

/// Writes @c v to the output stream @c s.
///
/// @param s The output stream.
/// @param v The @c Point2 to write to the @c s.
/// @return A reference to the output stream.
inline std::ostream& operator<<(std::ostream& s, Point2 v);

} // namespace dida

/// Adds @c std::hash support to @c dida::Point2.
template <>
struct std::hash<dida::Point2>
{
  /// Hashes @c point.
  inline size_t operator()(dida::Point2 point) const noexcept;
};

#include "dida/point2.inl"