#include "dida/math/big_int_utils.hpp"

namespace dida::math
{

constexpr Int128::Int128(uint64_t low_word, uint64_t high_word) : words_{low_word, high_word}
{
}

inline bool Int128::operator==(const Int128& b) const
{
  return words_[0] == b.words_[0] && words_[1] == b.words_[1];
}

inline bool Int128::operator!=(const Int128& b) const
{
  return words_[0] != b.words_[0] || words_[1] != b.words_[1];
}

inline Int128& Int128::operator+=(Int128 b)
{
  *this = *this + b;
  return *this;
}

inline Int128& Int128::operator-=(Int128 b)
{
  *this = *this - b;
  return *this;
}

inline Int128 Int128::operator+(Int128 b) const
{
  Int128 result;
  char carry = add_with_carry(0, words_[0], b.words_[0], result.words_[0]);
  add_with_carry(carry, words_[1], b.words_[1], result.words_[1]);
  return result;
}

inline Int128 Int128::operator-(Int128 b) const
{
  Int128 result;
  char borrow = sub_with_borrow(0, words_[0], b.words_[0], result.words_[0]);
  sub_with_borrow(borrow, words_[1], b.words_[1], result.words_[1]);
  return result;
}

inline Int128 Int128::operator-() const
{
  Int128 result;
  char borrow = sub_with_borrow(0, 0, words_[0], result.words_[0]);
  sub_with_borrow(borrow, 0, words_[1], result.words_[1]);
  return result;
}

inline Int128 Int128::multiply(int64_t a, int64_t b)
{
  SignedMul128Result mul_result = mul128(a, b);
  return Int128(mul_result.low_word, static_cast<uint64_t>(mul_result.high_word));
}

std::optional<Int128> Int128::add_checked(const Int128& b) const
{
  Int128 result;
  char carry = add_with_carry(0, words_[0], b.words_[0], result.words_[0]);
  add_with_carry(carry, words_[1], b.words_[1], result.words_[1]);

  // We use the following rules to determine whether there was an overflow:
  //
  //  - If both 'a' and 'b' are positive, then the result should be positive. If not, then there was an overflow.
  //  - If both 'a' and 'b' are negative, then the result should be negative. If not, then there was an overflow.
  //  - If 'a' and 'b' have opposite signs, then the result always fits.
  //
  // Zero is considered positive in this context.

  bool a_negative = static_cast<int64_t>(words_[1]) < 0;
  bool b_negative = static_cast<int64_t>(b.words_[1]) < 0;
  bool result_negative = static_cast<int64_t>(result.words_[1]) < 0;
  if (!a_negative && !b_negative && result_negative)
  {
    return std::nullopt;
  }

  if (a_negative && b_negative && !result_negative)
  {
    return std::nullopt;
  }

  return result;
}

std::optional<Int128> Int128::sub_checked(const Int128& b) const
{
  Int128 result;
  char borrow = sub_with_borrow(0, words_[0], b.words_[0], result.words_[0]);
  sub_with_borrow(borrow, words_[1], b.words_[1], result.words_[1]);

  // We use the following rules to determine whether there was an overflow:
  //
  //  - If 'a' is positive and 'b' negative, then the result should be positive. If not, then there was an overflow.
  //  - If 'a' is negative and 'b' positive, then the result should be negative. If not, then there was an overflow.
  //  - If 'a' and 'b' have the same sign, then the result always fits.
  //
  // Zero is considered positive in this context.

  bool a_negative = static_cast<int64_t>(words_[1]) < 0;
  bool b_negative = static_cast<int64_t>(b.words_[1]) < 0;
  bool result_negative = static_cast<int64_t>(result.words_[1]) < 0;
  if (!a_negative && b_negative && result_negative)
  {
    return std::nullopt;
  }

  if (a_negative && !b_negative && !result_negative)
  {
    return std::nullopt;
  }

  return result;
}

} // namespace dida::math