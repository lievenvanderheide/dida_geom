#include "dida/detail/parser.hpp"

#include <catch2/catch.hpp>

#include <iomanip>
#include <iostream>

namespace dida::detail
{

TEST_CASE("Construction and access")
{
  SECTION("With string view")
  {
    std::string string("The string to parse");
    Parser parser(string);
    CHECK(parser.head() == string.data());
    CHECK(parser.end() == string.data() + string.size());
  }

  SECTION("With pointers")
  {
    std::string string("The string to parse");
    Parser parser(string.data(), string.data() + string.size());
    CHECK(parser.head() == string.data());
    CHECK(parser.end() == string.data() + string.size());
  }
}

TEST_CASE("Parser::finished")
{
  Parser parser("l");
  CHECK(!parser.finished());

  parser.match('l');
  CHECK(parser.finished());
}

TEST_CASE("match")
{
  SECTION("Success")
  {
    std::string string("Q");
    Parser parser(string);
    CHECK(parser.match('Q'));
    CHECK(parser.head() == string.data() + 1);
  }

  SECTION("Failure, different character")
  {
    std::string string("Q");
    Parser parser(string);
    CHECK(!parser.match('R'));
  }

  SECTION("Failure, end of string")
  {
    std::string string("Q");
    Parser parser(string.data(), string.data());
    CHECK(!parser.match('Q'));
  }
}

TEST_CASE("Parser::skip_optional_whitespace")
{
  SECTION("Skips to end")
  {
    Parser parser("  \t  \n  ");
    parser.skip_optional_whitespace();
    CHECK(parser.finished());
  }

  SECTION("Skipts to non-whitespace")
  {
    Parser parser("\t\t  \tNotWhiteSpace   ");
    parser.skip_optional_whitespace();
    CHECK(*parser.head() == 'N');
  }

  SECTION("Nothing to skip")
  {
    Parser parser("NotWhiteSpace");
    parser.skip_optional_whitespace();
    CHECK(*parser.head() == 'N');
  }

  SECTION("Empty string")
  {
    Parser parser("");
    parser.skip_optional_whitespace();
    CHECK(parser.finished());
  }
}

TEST_CASE("Parser::try_parse_scalar")
{
  SECTION("Positive integer")
  {
    SECTION("Ends at end of string")
    {
      Parser parser("3495");
      std::optional<ScalarDeg1> result = parser.parse_scalar();
      CHECK(result == ScalarDeg1(3495));
      CHECK(*parser.head() == '\0');
    }

    SECTION("Ends at other character")
    {
      Parser parser("3495, ");
      std::optional<ScalarDeg1> result = parser.parse_scalar();
      CHECK(result == ScalarDeg1(3495));
      CHECK(*parser.head() == ',');
    }

    SECTION("Largest positive integer")
    {
      Parser parser("524287");
      std::optional<ScalarDeg1> result = parser.parse_scalar();
      CHECK(result == ScalarDeg1(524287));
      CHECK(*parser.head() == '\0');
    }

    SECTION("Too large")
    {
      Parser parser("524288");
      std::optional<ScalarDeg1> result = parser.parse_scalar();
      CHECK(result == std::nullopt);
    }

    SECTION("Way too large")
    {
      Parser parser("1000524287");
      std::optional<ScalarDeg1> result = parser.parse_scalar();
      CHECK(result == std::nullopt);
    }
  }

  SECTION("Negative integer")
  {
    SECTION("Ends at end of string")
    {
      Parser parser("-2421");
      std::optional<ScalarDeg1> result = parser.parse_scalar();
      CHECK(result == ScalarDeg1(-2421));
      CHECK(*parser.head() == '\0');
    }

    SECTION("Ends at other character")
    {
      Parser parser("-2421, ");
      std::optional<ScalarDeg1> result = parser.parse_scalar();
      CHECK(result == ScalarDeg1(-2421));
      CHECK(*parser.head() == ',');
    }

    SECTION("Largest negative integer")
    {
      Parser parser("-524288");
      std::optional<ScalarDeg1> result = parser.parse_scalar();
      CHECK(result == ScalarDeg1(-524288));
      CHECK(*parser.head() == '\0');
    }

    SECTION("Too large")
    {
      Parser parser("-524289");
      std::optional<ScalarDeg1> result = parser.parse_scalar();
      CHECK(result == std::nullopt);
    }

    SECTION("Way too large")
    {
      Parser parser("-1000524288");
      std::optional<ScalarDeg1> result = parser.parse_scalar();
      CHECK(result == std::nullopt);
    }
  }

  SECTION("Positive with fractional part")
  {
    SECTION("Ends at end of string")
    {
      Parser parser("7604.375");
      std::optional<ScalarDeg1> result = parser.parse_scalar();
      CHECK(result == ScalarDeg1(7604.375));
      CHECK(*parser.head() == '\0');
    }

    SECTION("Ends at other character")
    {
      Parser parser("7604.375, ");
      std::optional<ScalarDeg1> result = parser.parse_scalar();
      CHECK(result == ScalarDeg1(7604.375));
      CHECK(*parser.head() == ',');
    }

    SECTION("Greatest value")
    {
      Parser parser("524287.999755859375");
      std::optional<ScalarDeg1> result = parser.parse_scalar();
      CHECK(result == ScalarDeg1::max);
      CHECK(*parser.head() == '\0');
    }

    SECTION("Overflow in fractional part")
    {
      Parser parser("524287.99990234374");
      std::optional<ScalarDeg1> result = parser.parse_scalar();
      CHECK(result == std::nullopt);
    }
  }

  SECTION("Negative with fractional part")
  {
    SECTION("Ends at end of string")
    {
      Parser parser("-7412.114");
      std::optional<ScalarDeg1> result = parser.parse_scalar();
      CHECK(result == ScalarDeg1(-7412.114));
      CHECK(*parser.head() == '\0');
    }

    SECTION("Ends at other character")
    {
      Parser parser("-7412.114, ");
      std::optional<ScalarDeg1> result = parser.parse_scalar();
      CHECK(result == ScalarDeg1(-7412.114));
      CHECK(*parser.head() == ',');
    }

    SECTION("Rounds to lowest value")
    {
      Parser parser("-524288.000001");
      std::optional<ScalarDeg1> result = parser.parse_scalar();
      CHECK(result == ScalarDeg1::min);
      CHECK(*parser.head() == '\0');
    }

    SECTION("Underflow in fractional part")
    {
      Parser parser("-524288.01");
      std::optional<ScalarDeg1> result = parser.parse_scalar();
      CHECK(result == std::nullopt);
    }
  }

  SECTION("Fractional part only, no leading zero")
  {
    SECTION("Positive")
    {
      Parser parser(".123");
      std::optional<ScalarDeg1> result = parser.parse_scalar();
      CHECK(result == ScalarDeg1(.123));
      CHECK(*parser.head() == '\0');
    }

    SECTION("Negative")
    {
      Parser parser("-.123");
      std::optional<ScalarDeg1> result = parser.parse_scalar();
      CHECK(result == ScalarDeg1(-.123));
      CHECK(*parser.head() == '\0');
    }
  }

  SECTION("Ends in dot, but no fractional digits")
  {
    SECTION("Positive")
    {
      Parser parser("6330.");
      std::optional<ScalarDeg1> result = parser.parse_scalar();
      CHECK(result == ScalarDeg1(6330));
      CHECK(*parser.head() == '\0');
    }

    SECTION("Negative")
    {
      Parser parser("-4998.");
      std::optional<ScalarDeg1> result = parser.parse_scalar();
      CHECK(result == ScalarDeg1(-4998));
      CHECK(*parser.head() == '\0');
    }
  }

  SECTION(". is invalid")
  {
    Parser parser(".");
    std::optional<ScalarDeg1> result = parser.parse_scalar();
    CHECK(result == std::nullopt);
  }

  SECTION("- is invalid")
  {
    Parser parser("-");
    std::optional<ScalarDeg1> result = parser.parse_scalar();
    CHECK(result == std::nullopt);
  }

  SECTION("-. is invalid")
  {
    Parser parser("-.");
    std::optional<ScalarDeg1> result = parser.parse_scalar();
    CHECK(result == std::nullopt);
  }

  SECTION("Empty string is invalid")
  {
    SECTION("Empty string")
    {
      Parser parser("");
      std::optional<ScalarDeg1> result = parser.parse_scalar();
      CHECK(result == std::nullopt);
    }

    SECTION("Unparsable characters")
    {
      Parser parser("not a number");
      std::optional<ScalarDeg1> result = parser.parse_scalar();
      CHECK(result == std::nullopt);
    }
  }
}

TEST_CASE("parse_scalar_fractional_part")
{
  SECTION("Short")
  {
    SECTION("Exact")
    {
      // 512 * ScalarDeg1::quantum = 0.125
      ScalarDeg1 result = parse_scalar_fractional_part("125");
      CHECK(result == ScalarDeg1::from_numerator(512));
    }

    SECTION("Round up")
    {
      // Slightly lower than 512 * ScalarDeg1::quantum, but should still round up to it.
      ScalarDeg1 result = parse_scalar_fractional_part("1249");
      CHECK(result == ScalarDeg1::from_numerator(512));
    }

    SECTION("Round down")
    {
      // Slightly greater than 512 * ScalarDeg1::quantum, but should still round down to it.
      ScalarDeg1 result = parse_scalar_fractional_part("1251");
      CHECK(result == ScalarDeg1::from_numerator(512));
    }
  }

  SECTION("Long")
  {
    // Parse numbers around the mid point between 1951 * ScalarDeg1::quantum and 1952 * ScalarDeg::quantum.

    SECTION("Exactly at mid point")
    {
      ScalarDeg1 result = parse_scalar_fractional_part("4764404296875");
      CHECK(result == ScalarDeg1::from_numerator(1951));
    }

    SECTION("Slightly below mid point")
    {
      ScalarDeg1 result = parse_scalar_fractional_part("4764404296865");
      CHECK(result == ScalarDeg1::from_numerator(1951));
    }

    SECTION("Slightly above mid point")
    {
      ScalarDeg1 result = parse_scalar_fractional_part("4764404296975");
      CHECK(result == ScalarDeg1::from_numerator(1952));
    }

    SECTION("Below midpoint in significant part")
    {
      // 1951 * ScalarDeg1::quantum = 0.476318359375
      ScalarDeg1 result = parse_scalar_fractional_part("4763");
      CHECK(result == ScalarDeg1::from_numerator(1951));
    }
  }

  SECTION("Empty string")
  {
    CHECK(parse_scalar_fractional_part("") == ScalarDeg1(0));
  }
}

TEST_CASE("Parser::parse_vector2")
{
  SECTION("Valid, not whitespace")
  {
    Parser parser("{-92.89,40.34}");
    CHECK(parser.parse_vector2() == Vector2(-92.89, 40.34));
  }

  SECTION("Valid, with whitespace")
  {
    Parser parser("{  84.65  ,  51.45     }");
    CHECK(parser.parse_vector2() == Vector2(84.65, 51.45));
  }

  SECTION("Too many coordinates")
  {
    Parser parser("{-33.67, 85.26, -34.22}");
    CHECK(parser.parse_vector2() == std::nullopt);
  }

  SECTION("No {")
  {
    Parser parser("-92.89, 40.34}");
    CHECK(parser.parse_vector2() == std::nullopt);
  }

  SECTION("First coordinate invalid")
  {
    Parser parser("{ NaN, 40.34}");
    CHECK(parser.parse_vector2() == std::nullopt);
  }

  SECTION("No ,")
  {
    Parser parser("{-92.89 40.34}");
    CHECK(parser.parse_vector2() == std::nullopt);
  }

  SECTION("Second coordinate invalid")
  {
    Parser parser("{-92.89, NaN}");
    CHECK(parser.parse_vector2() == std::nullopt);
  }

  SECTION("No }")
  {
    Parser parser("{-92.89, 40.34");
    CHECK(parser.parse_vector2() == std::nullopt);
  }
}

TEST_CASE("Parser::parse_point2")
{
  SECTION("Success")
  {
    Parser parser("{96.40, -80.67}");
    CHECK(parser.parse_point2() == Point2(96.40, -80.67));
  }

  SECTION("Failure")
  {
    Parser parser("NotAPoint");
    CHECK(parser.parse_point2() == std::nullopt);
  }
}

} // namespace dida::detail