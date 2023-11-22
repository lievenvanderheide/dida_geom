#pragma once

#include <cmath>
#include <cstdint>
#include <iosfwd>
#include <type_traits>

#include "dida/math/int128.hpp"

namespace dida
{

/// A fixed point scalar type.
///
/// Each scalar has a degree associated with it. Scalars of the same degree can
/// be added and subtracted, resulting in a scalar of the same degree, while
/// scalars of any degree can be multiplied, resulting in a scalar whose degree
/// is the sum of the degree of the two operands. Since the number of bits of a
/// scalar, as well its radix position grow with the degree of the scalar, the
/// result of a multiplication can always be stored exactly. There's no need to
/// shift the result to the correct radix position, and no bits need to be
/// discarded.
///
/// Scalars of this type lie at the heart of DidaGeom's exact evaluation
/// paradigm. Coordinates of points and vectors are generally 1st degree scalars
/// which means points and vectors are aligned to a fixed resolution grid. Most
/// expressions in terms of coordinates, such as the dot product and (2D) cross
/// product then result in a scalar of higher degree which contains the exact
/// result.
///
/// @tparam degree The degree of the scalar.
/// @tparam IntType The underlying int type. This type must be large enough to
/// store integers of @c degree times the number of bits as used by scalars of
/// degree 1.
template <int degree, class IntType>
class Scalar
{
private:
  static constexpr double calc_quantum();
  static constexpr Scalar calc_min();
  static constexpr Scalar calc_max();

public:
  /// The radix position of a first degree scalar.
  static constexpr int deg_1_radix = 12;

  /// The radix position of scalars of the current degree.
  static constexpr int radix = degree * deg_1_radix;

  /// The difference between adjacent scalars of the current degree, represented
  /// as a @c double value.
  static constexpr double quantum = calc_quantum();

  /// The lowest value for this scalar type.
  static constexpr Scalar min = calc_min();

  /// The greatest value for this scalar type.
  static constexpr Scalar max = calc_max();

  /// Constructs an unitialized Scalar.
  Scalar() = default;

  /// Constructs a scalar with the given double value. If @p value is not
  /// representable as a scalar of the current degree, then it's rounded to the
  /// closest scalar.
  ///
  /// @param value The value.
  explicit Scalar(double value);

  /// Converts this scalar to a @c double.
  ///
  /// @return The result of the conversion.
  explicit operator double() const;

  /// Returns the scalar with the given numerator.
  ///
  /// @return The scalar.
  static Scalar from_numerator(const IntType& numerator);

  /// Returns the numerator of the fraction 'numerator() / 2^radix' whose value
  /// is equal to the value of this scalar.
  ///
  /// @return The numerator.
  const IntType& numerator() const;

  /// Compares two scalars for equality.
  ///
  /// @param b The second operand of the comparison.
  /// @return True iff the two scalars are equal.
  bool operator==(const Scalar& b) const;

  /// Compares two scalars for inequality.
  ///
  /// @param b The second operand of the comparison.
  /// @return True iff the two scalars are not equal.
  bool operator!=(const Scalar& b) const;

  /// Returns whether this scalar is less than @c b.
  ///
  /// @param b The second operand of the comparison.
  /// @return True iff this scalar is less than @c b.
  bool operator<(const Scalar& b) const;

  /// Returns whether this scalar is less than or equal to @c b.
  ///
  /// @param b The second operand of the comparison.
  /// @return True iff this scalar is less than or equal to @c b.
  bool operator<=(const Scalar& b) const;

  /// Returns whether this scalar is greater than or equal to @c b.
  ///
  /// @param b The second operand of the comparison.
  /// @return True iff this scalar is greater than or equal to @c b.
  bool operator>=(const Scalar& b) const;

  /// Returns whether this scalar is greater than @c b.
  ///
  /// @param b The second operand of the comparison.
  /// @return True iff this scalar is greater than @c b.
  bool operator>(const Scalar& b) const;

  /// Returns whether this scalar is equal to @c b.
  ///
  /// @note @c b is rounded to the resultion of scalars of the current degree
  /// before the comparison.
  /// @param b The second operand of the comparison.
  /// @return True iff the scalar is equal to @c b.
  bool operator==(const double b) const;

  /// Returns whether this scalar is not equal to @c b.
  ///
  /// @note @c b is rounded to the resultion of scalars of the current degree
  /// before the comparison.
  /// @param b The second operand of the comparison.
  /// @return True iff the scalar is not equal to @c b.
  bool operator!=(const double b) const;

  /// Returns whether this scalar is less than @c b.
  ///
  /// @note @c b is rounded to the resultion of scalars of the current degree
  /// before the comparison.
  /// @param b The second operand of the comparison.
  /// @return True iff the scalar is less than @c b.
  bool operator<(const double b) const;

  /// Returns whether this scalar is less than or equal to @c b.
  ///
  /// @note @c b is rounded to the resultion of scalars of the current degree
  /// before the comparison.
  /// @param b The second operand of the comparison.
  /// @return True iff the scalar is less than or equal to @c b.
  bool operator<=(const double b) const;

  /// Returns whether this scalar is greater than or equal to @c b.
  ///
  /// @note @c b is rounded to the resultion of scalars of the current degree
  /// before the comparison.
  /// @param b The second operand of the comparison.
  /// @return True iff the scalar is greater than or equal to @c b.
  bool operator>=(const double b) const;

  /// Returns whether this scalar is greater than @c b.
  ///
  /// @note @c b is rounded to the resultion of scalars of the current degree
  /// before the comparison.
  /// @param b The second operand of the comparison.
  /// @return True iff the scalar is greater than @c b.
  bool operator>(const double b) const;

  /// Adds this scalar and @c b and returns the result.
  ///
  /// @param b The second operand.
  /// @return The result of the addition.
  Scalar operator+(const Scalar& b) const;

  /// Subtracts this scalar and @c b and returns the result.
  ///
  /// @param b The second operand.
  /// @return The result of the subtraction.
  Scalar operator-(const Scalar& b) const;

  /// Negates this scalar and returns the result.
  ///
  /// @return The negated value of this scalar.
  Scalar operator-() const;

  /// Adds scalar @c b to this scalar.
  ///
  /// @param b The scalar to add to this scalar.
  /// @return A reference to this scalar.
  Scalar& operator+=(const Scalar& b);

  /// Subtracts scalar @c b from this scalar.
  ///
  /// @param b The scalar to subtracts from this scalar.
  /// @return A reference to this scalar.
  Scalar& operator-=(const Scalar& b);

private:
  IntType numerator_;
};

using ScalarDeg1 = Scalar<1, int32_t>;
using ScalarDeg2 = Scalar<2, int64_t>;
using ScalarDeg3 = Scalar<3, math::Int128>;
using ScalarDeg4 = Scalar<4, math::Int128>;

///@{
/// Multiplies two scalars.
///
/// @param a The first operand.
/// @param b The second operand.
/// @return The result of the multiplication.
inline ScalarDeg2 operator*(ScalarDeg1 a, ScalarDeg1 b);
inline ScalarDeg3 operator*(ScalarDeg1 a, ScalarDeg2 b);
inline ScalarDeg3 operator*(ScalarDeg2 a, ScalarDeg1 b);
inline ScalarDeg4 operator*(ScalarDeg2 a, ScalarDeg2 b);
///@}

///@{
/// Writes @c scalar to an output stream.
///
/// @param s The output stream.
/// @param scalar The scalar.
/// @return A reference to the output stream.
std::ostream& operator<<(std::ostream& s, ScalarDeg1 scalar);
std::ostream& operator<<(std::ostream& s, ScalarDeg2 scalar);
///@}

} // namespace dida

#include "dida/scalar.inl"