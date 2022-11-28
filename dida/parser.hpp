#pragma once

#include <optional>
#include <string_view>
#include <vector>

#include "dida/point2.hpp"
#include "dida/scalar.hpp"
#include "dida/vector2.hpp"

namespace dida
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

  /// Returns a pointer to the end of the string we're parsing.
  ///
  /// @return A pointer to the end of the string to parse.
  inline const char* end() const;

  /// Returns whether the parser has reached the end of the string to parse.
  ///
  /// @return True iff the end was reached.
  inline bool finished() const;

  /// Checks whether the character at the head of the parser matches @c c, and returns true iff this is the case.
  ///
  /// On success, the parser's head is advanced to the next character, on failure, the parser is left in an undefined
  /// state.
  ///
  /// @param c The expected character.
  /// @return True iff the character at the head of the parser matched @c c.
  inline bool match(char c);

  /// Checks whether the characters at the head of the parser match the given string, and returns true iff this is the
  /// case.
  ///
  /// On success, the parser's head is advanced to the first character after the matched characters, on failure, the
  /// parses is left in an undefined state.
  ///
  /// @param string The expected string.
  /// @return True iff the characters at the head of the parser match @c string.
  inline bool match(std::string_view string);

  /// Similar to @c match(char), with the added guarantee that the parser's head remains in its original position if
  /// there was no match.
  ///
  /// @param c The expected characgter.
  /// @return True iff the character at the head of the parser matched @c c.
  inline bool try_match(char c);

  /// Similar to @c match(std::string_view), with the added guarantee that the parser's head remains in its original
  /// position if there was no match.
  ///
  /// @param string The expected characters.
  /// @return True iff the characters at the ehad of the parser match @c string.
  inline bool try_match(std::string_view string);

  /// Skips required whitespace at the parser's head, until either the first non-whitespace or the end of the string to
  /// parse is reached.
  ///
  /// If no whitespace was found, @c false is returned, and the parser is left in an undefined state.
  ///
  /// @return True iff whitespace was found.
  inline bool skip_required_whitespace();

  /// Skips whitespace at the head of the parser, until either the first non-whitespace character or the end of the
  /// string to parse is reached.
  inline void skip_optional_whitespace();

  /// Parses a C-style identifier and returns the result. If the characters at the head of the parser don't form a valid
  /// identifier, then @c std::nullopt is returned.
  ///
  /// On success, the parser's head is advanced to the first character after the vector, on failure the parser is left
  /// in an undefined state.
  ///
  /// @return The identifier, or @c std::nullopt on failure.
  std::optional<std::string_view> parse_identifier();

  /// Parses a scalar value, rounds it to the nearest multiple of @c ScalarDeg1::quantum and returns the result. If the
  /// characters at the head of the parser don't represent a scalar, or if the resulting scalar is out of range, then @c
  /// std::nullopt is returned.
  ///
  /// On success, the parser's head is advanced to the first character after the scalar, on failure, the parser is left
  /// in an undefined state.
  ///
  /// @return The scalar, or @c std::nullopt on failure.
  std::optional<ScalarDeg1> parse_scalar();

  /// Parses a @c Vector2 and returns the result. If the characters at the head of the parser don't represent a @c
  /// Vector2, then @c std::nullopt is returned.
  ///
  /// On success, the parser's head is advanced to the first character after the vector, on failure the parser is left
  /// in an undefined state.
  ///
  /// The parses uses c-style formatting, so an example of a valid @c Vector2 string is <tt>{59.21, -40.57}</tt>,
  ///
  /// @return The @c Vector2, or @c std::nullopt on failure.
  std::optional<Vector2> parse_vector2();

  /// Parses a @c Point2 and returns the result. If the characters at the head of the parser don't represent a @c
  /// Point2, then @c std::nullopt is returned.
  ///
  /// On success, the parser's head is advanced to the first character after the point, on failure the parser is left
  /// in an undefined state.
  ///
  /// The parses uses c-style formatting, so an example of a valid @c Point2 string is <tt>{59.21, -40.57}</tt>,
  ///
  /// @return The @c Vector2, or @c std::nullopt on failure.
  inline std::optional<Point2> parse_point2();

  std::optional<std::vector<Point2>> parse_point2_vector();

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

} // namespace dida

#include "dida/parser.inl"