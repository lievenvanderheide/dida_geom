#include "dida/assert.hpp"

namespace dida::math
{

template <class Numerator, class Denominator>
Fraction<Numerator, Denominator>::Fraction(Numerator numerator, Denominator denominator)
    : numerator_(numerator), denominator_(denominator)
{
  DIDA_DEBUG_ASSERT(denominator_ > 0);
}

template <class Numerator, class Denominator>
const Numerator& Fraction<Numerator, Denominator>::numerator() const
{
  return numerator_;
}

template <class Numerator, class Denominator>
const Denominator& Fraction<Numerator, Denominator>::denominator() const
{
  return denominator_;
}

template <class Numerator, class Denominator>
template <class BNumerator, class BDenominator>
bool Fraction<Numerator, Denominator>::operator==(const Fraction<BNumerator, BDenominator>& b) const
{
  return numerator_ * b.denominator_ == b.numerator_ * denominator_;
}

template <class Numerator, class Denominator>
template <class BNumerator, class BDenominator>
bool Fraction<Numerator, Denominator>::operator!=(const Fraction<BNumerator, BDenominator>& b) const
{
  return numerator_ * b.denominator_ != b.numerator_ * denominator_;
}

template <class Numerator, class Denominator>
template <class BNumerator, class BDenominator>
bool Fraction<Numerator, Denominator>::operator<(const Fraction<BNumerator, BDenominator>& b) const
{
  return numerator_ * b.denominator_ < b.numerator_ * denominator_;
}

template <class Numerator, class Denominator>
template <class BNumerator, class BDenominator>
bool Fraction<Numerator, Denominator>::operator<=(const Fraction<BNumerator, BDenominator>& b) const
{
  return numerator_ * b.denominator_ <= b.numerator_ * denominator_;
}

template <class Numerator, class Denominator>
template <class BNumerator, class BDenominator>
bool Fraction<Numerator, Denominator>::operator>=(const Fraction<BNumerator, BDenominator>& b) const
{
  return numerator_ * b.denominator_ >= b.numerator_ * denominator_;
}

template <class Numerator, class Denominator>
template <class BNumerator, class BDenominator>
bool Fraction<Numerator, Denominator>::operator>(const Fraction<BNumerator, BDenominator>& b) const
{
  return numerator_ * b.denominator_ > b.numerator_ * denominator_;
}

} // namespace dida::math