#pragma once

#include <optional>
#include <string_view>
#include <cstdint>

namespace dida::math
{

/// A 128 bit signed integer.
class Int128
{
public:
  /// Constructs an uninitialized @c Int128.
  Int128() = default;

  /// Constructs an @c Int128 using the given low and high 64 bit words. The value of the resulting integer is
  /// <tt>low_word * 2^64 + high_word</tt>.
  ///
  /// @param low_word The low word.
  /// @param high_word The high word.
  constexpr Int128(uint64_t low_word, uint64_t high_word);

  /// Constructs an @c Int128 with the given value.
  ///
  /// @param value The value.
  constexpr explicit Int128(int64_t value);

  /// Returns a pointer the 2 64 bit words of this @c Int128.
  ///
  /// @return A pointer to the words.
  inline const uint64_t* words() const;

  /// Compares this @c Int128 and @c b for equality.
  ///
  /// @param b The second operand.
  /// @return True iff this @c Int128 and @p b are equal.
  inline bool operator==(const Int128& b) const;

  /// Compares this @c Int128 and @c b for inequality.
  ///
  /// @param b The second operand.
  /// @return True iff this @c Int128 and @p b are not equal.
  inline bool operator!=(const Int128& b) const;

  /// Returns whether this @c Int128 is less than @c b.
  ///
  /// @param b The second operand.
  /// @return True iff this @c Int128 is less than @c b.
  inline bool operator<(const Int128& b) const;

  /// Returns whether this @c Int128 is less than or equal to @c b.
  ///
  /// @param b The second operand.
  /// @return True iff this @c Int128 is less than or equal to @c b.
  inline bool operator<=(const Int128& b) const;

  /// Returns whether this @c Int128 is greater than or equal to @c b.
  ///
  /// @param b The second operand.
  /// @return True iff this @c Int128 is greater than or equal to @c b.
  inline bool operator>=(const Int128& b) const;

  /// Returns whether this @c Int128 is greater than @c b.
  ///
  /// @param b The second operand.
  /// @return True iff this @c Int128 is greater than @c b.
  inline bool operator>(const Int128& b) const;

  /// Adds @c b to this @c Int128.
  ///
  /// @param b The second operand.
  /// @return A reference to this @c Int128.
  inline Int128& operator+=(Int128 b);

  /// Subtracts @c b from this @c Int128.
  ///
  /// @param b The second operand.
  /// @return A reference to this @c Int128.
  inline Int128& operator-=(Int128 b);

  /// Adds this @c Int128 and @c b, and returns the result.
  ///
  /// The result wraps around in the standard 2's complement way in case of overflow.
  ///
  /// @param b The second operand.
  /// @return The sum of this @c Int128 and @c b.
  inline Int128 operator+(Int128 b) const;

  /// Subtracts @c b from this @c Int128 and returns the result.
  ///
  /// The result wraps around in the standard 2's complement way in case of overflow.
  ///
  /// @param b The second operand.
  /// @return The difference of this @c Int128 and @c b.
  inline Int128 operator-(Int128 b) const;

  /// Negates this @c Int128 and returns the result.
  ///
  /// The result wraps around in the standard 2's complement way in case of overflow (which only happens when its value
  /// is -2^127).
  inline Int128 operator-() const;

  /// Multiplies two @c int64_t values and returns the result as a @c Int128.
  ///
  /// @param a The second operand.
  /// @param b The first operand.
  /// @return The produt.
  static inline Int128 multiply(int64_t a, int64_t b);

  /// Computes the sum of this @c Int128 and @c b, checks whether the result fits the @c Int128 range, and returns the
  /// result if it fits. If it doesn't fit, @c std::nullopt is returned.
  ///
  /// @param b The second operand.
  /// @return The sum of this @c Int128 and @c b, or @c std::nullopt if the result doesn't fit the @c Int128 range.
  inline std::optional<Int128> add_checked(const Int128& b) const;

  /// Computes the difference of this @c Int128 and @c b, checks whether the result fits the @c Int128 range, and
  /// returns the result if it fits. If it doesn't fit, @c std::nullopt is returned.
  ///
  /// @param b The second operand.
  /// @return The difference of this @c Int128 and @c b, or @c std::nullopt if the result doesn't fit the @c Int128
  /// range.
  inline std::optional<Int128> sub_checked(const Int128& b) const;

  /// Parses the given string representation of an @c Int128 and returns the result. If the string doesn't hold a valid
  /// @c Int128, then std::nullopt is returned.
  ///
  /// @param string The string to parse.
  /// @return The parsed value, or @c std::nullopt if @c string doesn't hold a valid @c Int128.
  static std::optional<Int128> from_string(std::string_view string);

private:
  /// The words which makes up this @c Int128, where @c words_[0] contains bits 0:63, and @c words_[1] bits 64:127.
  uint64_t words_[2];
};

} // namespace dida::math

#include "dida/math/int128.inl"