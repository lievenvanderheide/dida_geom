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

/// Returns the number of significant fractional digits in the base 10 representation of a scalar of the given type.
///
/// This is the number of digits such that truncating the decimal representation to this many digits results in a value
/// which is less than half a quantum lower than the untruncated value:
///
///   v - truncated(v) < .5 * ScalarType::quantum
///
template <class ScalarType>
constexpr size_t base_10_num_significant_fractional_digits()
{
  using IntType = typename ScalarType::IntType;

  // The result is the lowest 'n' for which
  //
  //   1/10^n <= .5 * 1/2^radix
  //
  // Taking the reciprocal gives
  //
  //   10^n >= 2^(radix + 1)

  IntType lhs = 1;
  IntType rhs = static_cast<IntType>(2) << ScalarType::radix;
  size_t n = 0;
  while (lhs < rhs)
  {
    lhs *= 10;
    n++;
  }

  return n;
}

}

template <class ScalarType>
ScalarType Parser::parse_scalar_fractional_part()
{
  using IntType = typename ScalarType::IntType;

  constexpr size_t num_significant_digits = base_10_num_significant_fractional_digits<ScalarType>();

  IntType base_10_num = 0;
  IntType base_10_denom = 1;
  for (size_t i = 0; i < num_significant_digits; i++)
  {
    if (head_ == end_ || !is_digit(*head_))
    {
      break;
    }

    base_10_num = 10 * base_10_num + static_cast<IntType>(*head_ - '0');
    base_10_denom *= 10;

    head_++;
  }

  // The significant digits have been parsed. The final value will be either base_2_num / base_2_denom or base_2_num /
  // base_2_denom + quantum.

  IntType base_2_denom = static_cast<IntType>(1) << ScalarType::radix;
  IntType base_2_num = base_10_num * base_2_denom / base_10_denom;
  IntType remainder = base_10_num * base_2_denom % base_10_denom;

  if (remainder > (base_10_denom / 2) || (remainder == (base_10_denom / 2) && (base_2_num & 1) == 1))
  {
    // We're already rounding up, so even if there are digits remaining, these can't be enough to bump the result up by
    // another ScalarDeg1::quantum.

    while (head_ != end_ && is_digit(*head_))
    {
      head_++;
    }

    return ScalarType::from_numerator(base_2_num + 1);
  }

  if (head_ == end_ || !is_digit(*head_))
  {
    // There are no digits remaining.
    return ScalarType::from_numerator(base_2_num);
  }

  // The truncated value resulted in downwards rounding, and there are digits remaining, so it may be possible that
  // these remaining digits push 'v' over the threshold for upwards rounding.
  //
  // We should round up if
  //
  //                     truncated(v) + tail(v) > base_2_num / base_2_denom + .5 / base_2_denom
  //                                    tail(v) > .5 / base_2_denom - remainder / (base_10_denom * base_2_denom)
  //   (tail(v) * base_10_denom) * base_2_denom > base_10_denom / 2 - remainder
  //

  IntType threshold = base_10_denom / 2 - remainder;

  for (; head_ != end_ && is_digit(*head_); head_++)
  {
    IntType digit_base_2_num = static_cast<IntType>(*head_ - '0') * base_2_denom;
    threshold *= 10;
    if (digit_base_2_num + base_2_denom <= threshold)
    {
      while (head_ != end_ && is_digit(*head_))
      {
        head_++;
      }

      return ScalarType::from_numerator(base_2_num);
    }
    else if (digit_base_2_num > threshold)
    {
      while (head_ != end_ && is_digit(*head_))
      {
        head_++;
      }

      return ScalarType::from_numerator(base_2_num + 1);
    }
    else
    {
      threshold -= digit_base_2_num;
    }
  }

  if (threshold == 0)
  {
    // We're exactly on the threshold, so we should round to even.
    if ((base_2_num & 1) == 1)
    {
      base_2_num++;
    }
  }

  return ScalarDeg1::from_numerator(base_2_num);
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

    if (head_ != end_ && is_digit(*head_))
    {
      fractional_part = parse_scalar_fractional_part<ScalarDeg1>();
    }
    else if (num_digits == 0)
    {
      return std::nullopt;
    }
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
  if (try_match('}'))
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
      // If there's no comma, then we must have reached the end of the vector.
      if (!match('}'))
      {
        return std::nullopt;
      }

      return result;
    }

    skip_optional_whitespace();

    if (match('}'))
    {
      // There was a comma, but the comma was immediately followed by closing brace, so we've reached the end of the
      // vector.
      return result;
    }
  }
}

} // namespace dida