#pragma once

#include <optional>
#include <string_view>

#include "dida/scalar.hpp"

namespace dida::detail
{

/// A parser to parse c-style markup.
class Parser
{
public:
  /// Constructs a @c Parser for the given @c string.
  inline Parser(std::string_view string);

  /// Constructs a @c Parser for the string starting at @c begin and ending in @c end.
  inline Parser(const char* begin, const char* end);

  /// Returns a pointer to the current head of the parser.
  ///
  /// @return The current head.
  inline const char* head() const;

  /// Tries parsing the characters at the head of the parser as a scalar value, rounds it to the  nearest multiple of @c
  /// ScalarDeg1::quantum and returns the result. If the characters at the head of the parser don't represent a scalar,
  /// or if the resulting scalar is out of range, then @c std::nullopt is returned.
  ///
  /// On success, the parser's head is advanced to the first character after the scalar, on failure, the parser remains
  /// unchanged.
  ///
  /// @return The scalar, or @c std::nullopt on failure.
  std::optional<ScalarDeg1> try_parse_scalar();

private:
  const char* head_;
  const char* end_;
};

/// Parses a sequence of decimal digits as the fractional part after a decimal '.', and rounds it to the nearest
/// @c ScalarDeg1 (so the result is in the in the range '[0, 1)').
///
/// @param digits The digits. Should consists of 0 or more decimal digits.
/// @return The nearest @c ScalarDeg1 to a fractional part with the digits in @c digits.
ScalarDeg1 parse_scalar_fractional_part(std::string_view digits);

} // namespace dida::detail

#include "dida/detail/parser.inl"