#include "dida/parser.hpp"

#include <cctype>

#include "dida/assert.hpp"

namespace dida
{

namespace
{

bool is_identifier_first_char(char c)
{
  return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '_';
}

bool is_identifier_char(char c)
{
  return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '_';
}

bool is_digit(char c)
{
  return c >= '0' && c <= '9';
}

} // namespace

std::optional<std::string_view> Parser::parse_identifier()
{
  const char* identifier_begin = head_;

  if (head_ == end_ || !is_identifier_first_char(*head_))
  {
    return std::nullopt;
  }

  head_++;
  while (head_ != end_ && is_identifier_char(*head_))
  {
    head_++;
  }

  return std::string_view(identifier_begin, static_cast<size_t>(head_ - identifier_begin));
}

namespace
{

/// Divides @c a by @c b with round to nearest. Ties are rounded down.
///
/// @param a The dividend. Must be non-negative.
/// @param b The divisor. Must be positive.
/// @return The quotient <tt>a/b</tt> rounded to the nearest integers.
int32_t div_round_nearest(int32_t a, int32_t b)
{
  DIDA_DEBUG_ASSERT(a >= 0);
  DIDA_DEBUG_ASSERT(b > 0);
  return (a + (b >> 1)) / b;
}

} // namespace

ScalarDeg1 parse_scalar_fractional_part(std::string_view digits)
{
  static_assert(ScalarDeg1::radix == 12);

  // The number of significant digits is the number of digits necessary to compute the result with an error of at most
  // ScalarDeg1::quantum.
  static constexpr size_t num_significant_digits = 4;

  if (digits.size() <= num_significant_digits)
  {
    int32_t fractional_part_num = 0;
    int32_t fractional_part_denom = 1;
    for (size_t i = 0; i < digits.size(); i++)
    {
      DIDA_DEBUG_ASSERT(is_digit(digits[i]));
      int32_t digit = static_cast<int32_t>(digits[i] - '0');
      fractional_part_num = 10 * fractional_part_num + digit;
      fractional_part_denom *= 10;
    }

    return ScalarDeg1::from_numerator(
        div_round_nearest(fractional_part_num << ScalarDeg1::radix, fractional_part_denom));
  }
  else
  {
    int32_t significant_digits = 0;
    for (size_t i = 0; i < num_significant_digits; i++)
    {
      DIDA_DEBUG_ASSERT(is_digit(digits[i]));
      int32_t digit = static_cast<int32_t>(digits[i] - '0');
      significant_digits = 10 * significant_digits + digit;
    }

    // The numerator of the correctly rounded result of this function is either 'result_num' or 'result_num + 1'.
    int32_t result_num = div_round_nearest(significant_digits << ScalarDeg1::radix, 10000);

    // 'threshold = threshold_num/threshold_denom' is the midpoint between 'result' and 'result + quantum'. If the full
    // digit sequence represents a decimal number less than or equal to this threshold, then we should round down,
    // otherwise we should round up.
    int32_t threshold_num = 2 * result_num + 1;
    int32_t threshold_denom = 1 << (ScalarDeg1::radix + 1);

    // Compare 'significant_digits' against the significant digits of 'threshold', and update 'threshold' to the
    // threshold for the remaining digits.
    int32_t threshold_significant_digits = (threshold_num * 10000) / threshold_denom;
    threshold_num = (threshold_num * 10000) % threshold_denom;
    if (threshold_significant_digits != significant_digits)
    {
      DIDA_DEBUG_ASSERT(significant_digits < threshold_significant_digits);
      return ScalarDeg1::from_numerator(result_num);
    }

    for (size_t i = num_significant_digits; i < digits.size(); i++)
    {
      DIDA_DEBUG_ASSERT(is_digit(digits[i]));
      int32_t digit = static_cast<int32_t>(digits[i] - '0');

      // Compute the most significant digit of 'threshold'.
      int32_t threshold_digit = (threshold_num * 10) / threshold_denom;
      if (threshold_digit != digit)
      {
        // If the current digit is different from 'threshold_digit', then we have enough information to know which way
        // we should round.
        if (digit > threshold_digit)
        {
          result_num++;
        }

        break;
      }

      // Update 'threshold' to the threshold for the remaining digits.
      threshold_num = (threshold_num * 10) % threshold_denom;
    }

    return ScalarDeg1::from_numerator(result_num);
  }
}

std::optional<ScalarDeg1> Parser::parse_scalar()
{
  static_assert(ScalarDeg1::radix == 12);

  static constexpr size_t max_num_int_digits = 6;
  static constexpr int32_t max_int_part = 1 << (31 - ScalarDeg1::radix);

  if (head_ == end_)
  {
    return std::nullopt;
  }

  bool negative = *head_ == '-';
  if (negative)
  {
    head_++;
    if (head_ == end_)
    {
      return std::nullopt;
    }
  }

  if (!is_digit(*head_) && *head_ != '.')
  {
    return std::nullopt;
  }

  int32_t int_part = 0;
  size_t num_digits = 0;
  while (head_ != end_ && is_digit(*head_))
  {
    if (num_digits > max_num_int_digits)
    {
      return std::nullopt;
    }

    int32_t digit = static_cast<int32_t>(*head_ - '0');
    int_part = int_part * 10 + digit;
    head_++;
    num_digits++;
  }

  ScalarDeg1 fractional_part(0);
  if (head_ != end_ && *head_ == '.')
  {
    head_++;

    const char* fractional_digits_begin = head_;
    while (head_ != end_ && is_digit(*head_))
    {
      head_++;
    }
    size_t num_fractional_digits = head_ - fractional_digits_begin;

    if (num_digits == 0 && num_fractional_digits == 0)
    {
      return std::nullopt;
    }

    fractional_part = parse_scalar_fractional_part(std::string_view(fractional_digits_begin, num_fractional_digits));
  }

  if (negative)
  {
    if (int_part > max_int_part)
    {
      return std::nullopt;
    }

    ScalarDeg1 int_part_scalar = ScalarDeg1::from_numerator((-int_part) << ScalarDeg1::radix);
    if (-fractional_part < ScalarDeg1::min - int_part_scalar)
    {
      return std::nullopt;
    }

    return int_part_scalar - fractional_part;
  }
  else
  {
    if (int_part >= max_int_part)
    {
      return std::nullopt;
    }

    ScalarDeg1 int_part_scalar = ScalarDeg1::from_numerator(int_part << ScalarDeg1::radix);
    if (fractional_part > ScalarDeg1::max - int_part_scalar)
    {
      return std::nullopt;
    }

    return int_part_scalar + fractional_part;
  }
}

std::optional<Vector2> Parser::parse_vector2()
{
  if (!match('{'))
  {
    return std::nullopt;
  }

  skip_optional_whitespace();
  std::optional<ScalarDeg1> x = parse_scalar();
  if (!x)
  {
    return std::nullopt;
  }

  skip_optional_whitespace();
  if (!match(','))
  {
    return std::nullopt;
  }

  skip_optional_whitespace();
  std::optional<ScalarDeg1> y = parse_scalar();
  if (!y)
  {
    return std::nullopt;
  }

  skip_optional_whitespace();
  if (!match('}'))
  {
    return std::nullopt;
  }

  return Vector2(*x, *y);
}

std::optional<std::vector<Point2>> Parser::parse_point2_vector()
{
  if (!match('{'))
  {
    return std::nullopt;
  }

  skip_optional_whitespace();
  if(try_match('}'))
  {
    return std::vector<Point2>();
  }

  std::vector<Point2> result;
  while (true)
  {
    std::optional<Point2> point = parse_point2();
    if (!point)
    {
      return std::nullopt;
    }

    result.push_back(*point);

    skip_optional_whitespace();
    if (!try_match(','))
    {
      break;
    }

    skip_optional_whitespace();
  }

  if (!match('}'))
  {
    return std::nullopt;
  }

  return result;
}

} // namespace dida