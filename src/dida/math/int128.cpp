#include "dida/math/int128.hpp"

namespace dida::math
{

namespace
{

/// The number of decimal digits which go into a single chunk. This is the largest number of digits for which it's
/// guaranteed that the value of a chunk fits in a @c uint64_t.
constexpr int num_digits_per_chunk = 19;
constexpr uint64_t chunk_base = 10000000000000000000U;
constexpr Int128 chunk_base_squared(0x098a224000000000, 0x4b3b4ca85a86c47a);

std::optional<uint64_t> parse_digit_chunk(std::string_view::const_reverse_iterator& it,
                                          std::string_view::const_reverse_iterator end)
{
  uint64_t result = 0;
  uint64_t pow_of_10 = 1;
  for (int i = 0; i < num_digits_per_chunk; i++)
  {
    if (it == end)
    {
      return result;
    }

    if (*it < '0' || *it > '9')
    {
      return std::nullopt;
    }

    result += static_cast<uint64_t>(*it - '0') * pow_of_10;
    pow_of_10 *= 10;

    it++;
  }

  return result;
}

} // namespace

std::optional<Int128> Int128::from_string(std::string_view string)
{
  std::string_view::const_reverse_iterator it = string.rbegin();
  std::string_view::const_reverse_iterator end_it = string.rend();

  if (it == end_it)
  {
    return std::nullopt;
  }

  bool negative = false;
  if (*(end_it - 1) == '-')
  {
    end_it--;
    if (it == end_it)
    {
      return std::nullopt;
    }

    negative = true;
  }

  // We'll now parse the digits of the string. Digits are parsed in chunks of at must 'num_digits_per_chunk', and then
  // combined into the resulting Int128.

  // Handle the first chunk.
  std::optional<uint64_t> chunk_1 = parse_digit_chunk(it, end_it);
  if (!chunk_1)
  {
    return std::nullopt;
  }

  Int128 result(*chunk_1, 0);
  if (it == end_it)
  {
    return negative ? -result : result;
  }

  // Handle the second chunk.
  std::optional<uint64_t> chunk_2 = parse_digit_chunk(it, end_it);
  if (!chunk_2)
  {
    return std::nullopt;
  }

  // Note: We can't use Int128::multply, because chunk_base doesn't fit in a int64_t, only uint64_t. The product of
  // chunk_base * *chink_2 is guaranteed to fit though, so the following code is safe.
  UnsignedMul128Result chunk_shifted = mul128(chunk_base, *chunk_2);
  result += Int128(chunk_shifted.low_word, chunk_shifted.high_word);
  if (it == end_it)
  {
    return negative ? -result : result;
  }

  // The only allowed value for the last chunk is '1', for any other value it's an overflow.
  if (*it != '1' || it + 1 != end_it)
  {
    return std::nullopt;
  }

  if (negative)
  {
    result = -result;
    return result.sub_checked(chunk_base_squared);
  }
  else
  {
    return result.add_checked(chunk_base_squared);
  }
}

} // namespace dida::math