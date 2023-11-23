namespace dida
{

template <int degree, class IntTypeT>
constexpr double Scalar<degree, IntTypeT>::calc_quantum()
{
  if constexpr (std::is_same_v<IntType, int32_t> || std::is_same_v<IntType, int64_t>)
  {
    uint64_t denominator = static_cast<uint64_t>(1) << radix;
    return 1 / static_cast<double>(denominator);
  }
  else
  {
    if (radix < 64)
    {
      uint64_t denominator = static_cast<uint64_t>(1) << radix;
      return 1 / static_cast<double>(denominator);
    }
    else
    {
      uint64_t denominator = static_cast<uint64_t>(1) << (radix - 64);
      return 0x1p-64 / static_cast<double>(denominator);
    }
    static_assert(std::is_same_v<IntType, math::Int128>);
    return 0;
  }
}

template <int degree, class IntTypeT>
constexpr Scalar<degree, IntTypeT> Scalar<degree, IntTypeT>::calc_min()
{
  static_assert(std::is_same_v<IntType, int32_t> || std::is_same_v<IntType, int64_t>,
                "Not implemented yet for higher degree scalar types");

  Scalar result{};
  result.numerator_ = std::numeric_limits<IntType>::min();
  return result;
}

template <int degree, class IntTypeT>
constexpr Scalar<degree, IntTypeT> Scalar<degree, IntTypeT>::calc_max()
{
  static_assert(std::is_same_v<IntType, int32_t> || std::is_same_v<IntType, int64_t>,
                "Not implemented yet for higher degree scalar types");

  Scalar result{};
  result.numerator_ = std::numeric_limits<IntType>::max();
  return result;
}

template <int degree, class IntTypeT>
Scalar<degree, IntTypeT>::Scalar(double value)
{
  numerator_ = static_cast<IntType>(std::nearbyint(std::ldexp(value, radix)));
}

template <int degree, class IntTypeT>
Scalar<degree, IntTypeT>::operator double() const
{
  return static_cast<double>(numerator_) * quantum;
}

template <int degree, class IntTypeT>
Scalar<degree, IntTypeT> Scalar<degree, IntTypeT>::from_numerator(const IntType& numerator)
{
  Scalar result;
  result.numerator_ = numerator;
  return result;
}

template <int degree, class IntTypeT>
const IntTypeT& Scalar<degree, IntTypeT>::numerator() const
{
  return numerator_;
}

template <int degree, class IntTypeT>
bool Scalar<degree, IntTypeT>::operator==(const Scalar& b) const
{
  return numerator_ == b.numerator_;
}

template <int degree, class IntTypeT>
bool Scalar<degree, IntTypeT>::operator!=(const Scalar& b) const
{
  return numerator_ != b.numerator_;
}

template <int degree, class IntTypeT>
bool Scalar<degree, IntTypeT>::operator<(const Scalar& b) const
{
  return numerator_ < b.numerator_;
}

template <int degree, class IntTypeT>
bool Scalar<degree, IntTypeT>::operator<=(const Scalar& b) const
{
  return numerator_ <= b.numerator_;
}

template <int degree, class IntTypeT>
bool Scalar<degree, IntTypeT>::operator>=(const Scalar& b) const
{
  return numerator_ >= b.numerator_;
}

template <int degree, class IntTypeT>
bool Scalar<degree, IntTypeT>::operator>(const Scalar& b) const
{
  return numerator_ > b.numerator_;
}

template <int degree, class IntTypeT>
bool Scalar<degree, IntTypeT>::operator==(const double b) const
{
  return *this == Scalar(b);
}

template <int degree, class IntTypeT>
bool Scalar<degree, IntTypeT>::operator!=(const double b) const
{
  return *this != Scalar(b);
}

template <int degree, class IntTypeT>
bool Scalar<degree, IntTypeT>::operator<(const double b) const
{
  return *this < Scalar(b);
}

template <int degree, class IntTypeT>
bool Scalar<degree, IntTypeT>::operator<=(const double b) const
{
  return *this <= Scalar(b);
}

template <int degree, class IntTypeT>
bool Scalar<degree, IntTypeT>::operator>=(const double b) const
{
  return *this >= Scalar(b);
}

template <int degree, class IntTypeT>
bool Scalar<degree, IntTypeT>::operator>(const double b) const
{
  return *this > Scalar(b);
}

template <int degree, class IntTypeT>
Scalar<degree, IntTypeT> Scalar<degree, IntTypeT>::operator+(const Scalar& b) const
{
  Scalar result;
  result.numerator_ = numerator_ + b.numerator_;
  return result;
}

template <int degree, class IntTypeT>
Scalar<degree, IntTypeT> Scalar<degree, IntTypeT>::operator-(const Scalar& b) const
{
  Scalar result;
  result.numerator_ = numerator_ - b.numerator_;
  return result;
}

template <int degree, class IntTypeT>
Scalar<degree, IntTypeT> Scalar<degree, IntTypeT>::operator-() const
{
  Scalar result;
  result.numerator_ = -numerator_;
  return result;
}

template <int degree, class IntTypeT>
Scalar<degree, IntTypeT>& Scalar<degree, IntTypeT>::operator+=(const Scalar& b)
{
  numerator_ += b.numerator_;
  return *this;
}

template <int degree, class IntTypeT>
Scalar<degree, IntTypeT>& Scalar<degree, IntTypeT>::operator-=(const Scalar& b)
{
  numerator_ -= b.numerator_;
  return *this;
}

inline ScalarDeg2 operator*(ScalarDeg1 a, ScalarDeg1 b)
{
  return ScalarDeg2::from_numerator(static_cast<int64_t>(a.numerator()) * static_cast<int64_t>(b.numerator()));
}

inline ScalarDeg3 operator*(ScalarDeg1 a, ScalarDeg2 b)
{
  return ScalarDeg3::from_numerator(math::Int128::multiply(a.numerator(), b.numerator()));
}

inline ScalarDeg3 operator*(ScalarDeg2 a, ScalarDeg1 b)
{
  return ScalarDeg3::from_numerator(math::Int128::multiply(a.numerator(), b.numerator()));
}

inline ScalarDeg4 operator*(ScalarDeg2 a, ScalarDeg2 b)
{
  return ScalarDeg4::from_numerator(math::Int128::multiply(a.numerator(), b.numerator()));
}

} // namespace dida