#pragma once

namespace dida::math
{

/// A fraction of two scalars.
///
/// @tparam Numerator The numerator type.
/// @tparam Denominator The denominator type.
template <class Numerator, class Denominator>
class Fraction
{
public:
  /// Constructs a fraction with the given numerator and denominator.
  ///
  /// @pre denominator must be positive.
  /// @param numerator The numerator.
  /// @param denominator The denominator.
  Fraction(Numerator numerator, Denominator denominator);

  /// Returns the numerator of this @c Fraction.
  ///
  /// @return The numerator.
  const Numerator& numerator() const;

  /// Returns the denominator of this @c Fraction.
  ///
  /// @return The denominator.
  const Denominator& denominator() const;

  /// Compares two fractions for equality.
  ///
  /// @tparam BNumerator The type of the numerator of the second fraction.
  /// @tparam BDenominator The type of the denominator of the second fraction.
  /// @param b The second operand of the comparison.
  /// @return True iff the two fractions represent the same value.
  template <class BNumerator, class BDenominator>
  bool operator==(const Fraction<BNumerator, BDenominator>& b) const;

  /// Compares two fractions for inequality.
  ///
  /// @tparam BNumerator The type of the numerator of the second fraction.
  /// @tparam BDenominator The type of the denominator of the second fraction.
  /// @param b The second operand of the comparison.
  /// @return True iff the two fractions represent different values.
  template <class BNumerator, class BDenominator>
  bool operator!=(const Fraction<BNumerator, BDenominator>& b) const;

  /// Returns whether this fraction is less than fraction @c b.
  ///
  /// @tparam BNumerator The type of the numerator of the second fraction.
  /// @tparam BDenominator The type of the denominator of the second fraction.
  /// @param b The second operand of the comparison.
  /// @return True iff this fraction is less than fraction @c b.
  template <class BNumerator, class BDenominator>
  bool operator<(const Fraction<BNumerator, BDenominator>& b) const;

  /// Returns whether this fraction is less than or equal to fraction @c b.
  ///
  /// @tparam BNumerator The type of the numerator of the second fraction.
  /// @tparam BDenominator The type of the denominator of the second fraction.
  /// @param b The second operand of the comparison.
  /// @return True iff this fraction is less than or equal to fraction @c b.
  template <class BNumerator, class BDenominator>
  bool operator<=(const Fraction<BNumerator, BDenominator>& b) const;

  /// Returns whether this fraction is less greater than or equal to fraction @c b.
  ///
  /// @tparam BNumerator The type of the numerator of the second fraction.
  /// @tparam BDenominator The type of the denominator of the second fraction.
  /// @param b The second operand of the comparison.
  /// @return True iff this fraction is greater than or equal to fraction @c b.
  template <class BNumerator, class BDenominator>
  bool operator>=(const Fraction<BNumerator, BDenominator>& b) const;

  /// Returns whether this fraction is greater than fraction @c b.
  ///
  /// @tparam BNumerator The type of the numerator of the second fraction.
  /// @tparam BDenominator The type of the denominator of the second fraction.
  /// @param b The second operand of the comparison.
  /// @return True iff this fraction is greater than fraction @c b.
  template <class BNumerator, class BDenominator>
  bool operator>(const Fraction<BNumerator, BDenominator>& b) const;

  /// Returns whether this fraction is equal to scalar @c b.
  ///
  /// @tparam B The type of the scalar.
  /// @param b The second operand of the comparison.
  /// @return True iff this fraction is equal to @c b.
  template <class B>
  bool operator==(const B& b) const;
  /// Returns whether this fraction is not equal to scalar @c b.
  ///
  /// @tparam B The type of the scalar.
  /// @param b The second operand of the comparison.
  /// @return True iff this fraction is not equal to @c b.
  template <class B>
  bool operator!=(const B& b) const;

  /// Returns whether this fraction is less than scalar @c b.
  ///
  /// @tparam B The type of the scalar.
  /// @param b The second operand of the comparison.
  /// @return True iff this fraction is less than @c b.
  template <class B>
  bool operator<(const B& b) const;

  /// Returns whether this fraction is less than or equal to scalar @c b.
  ///
  /// @tparam B The type of the scalar.
  /// @param b The second operand of the comparison.
  /// @return True iff this fraction is less than or equal to @c b.
  template <class B>
  bool operator<=(const B& b) const;

  /// Returns whether this fraction is greater than or equal to scalar @c b.
  ///
  /// @tparam B The type of the scalar.
  /// @param b The second operand of the comparison.
  /// @return True iff this fraction is greater than or equal to @c b.
  template <class B>
  bool operator>=(const B& b) const;

  /// Returns whether this fraction is greater than scalar @c b.
  ///
  /// @tparam B The type of the scalar.
  /// @param b The second operand of the comparison.
  /// @return True iff this fraction is greater than @c b.
  template <class B>
  bool operator>(const B& b) const;

private:
  Numerator numerator_;
  Denominator denominator_;
};

} // namespace dida::math

#include "dida/math/fraction.inl"