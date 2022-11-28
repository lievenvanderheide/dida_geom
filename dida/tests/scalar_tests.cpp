#include "dida/scalar.hpp"

#include <catch2/catch.hpp>
#include <sstream>

namespace dida
{

TEST_CASE("Scalar constants")
{
  SECTION("ScalarDeg1")
  {
    CHECK(ScalarDeg1::deg_1_radix == 12);
    CHECK(ScalarDeg1::radix == 12);
    CHECK(ScalarDeg1::quantum == std::ldexp(1, -12));
    CHECK(ScalarDeg1::min == ScalarDeg1::from_numerator(std::numeric_limits<int32_t>::min()));
    CHECK(ScalarDeg1::max == ScalarDeg1::from_numerator(std::numeric_limits<int32_t>::max()));
  }

  SECTION("ScalarDeg2")
  {
    CHECK(ScalarDeg2::deg_1_radix == 12);
    CHECK(ScalarDeg2::radix == 24);
    CHECK(ScalarDeg2::quantum == std::ldexp(1, -24));
    CHECK(ScalarDeg2::min == ScalarDeg2::from_numerator(std::numeric_limits<int64_t>::min()));
    CHECK(ScalarDeg2::max == ScalarDeg2::from_numerator(std::numeric_limits<int64_t>::max()));
  }

  SECTION("ScalarDeg3")
  {
    CHECK(ScalarDeg3::deg_1_radix == 12);
    CHECK(ScalarDeg3::radix == 36);
    CHECK(ScalarDeg3::quantum == std::ldexp(1, -36));
  }

  SECTION("ScalarDeg4")
  {
    CHECK(ScalarDeg4::deg_1_radix == 12);
    CHECK(ScalarDeg4::radix == 48);
    CHECK(ScalarDeg4::quantum == std::ldexp(1, -48));
  }
}

TEST_CASE("Scalar from/to double")
{
  SECTION("ScalarDeg1")
  {
    CHECK(static_cast<double>(ScalarDeg1(1)) == 1);
    CHECK(static_cast<double>(ScalarDeg1(1234.5)) == 1234.5);
    CHECK(static_cast<double>(ScalarDeg1(-551.75)) == -551.75);
  }

  SECTION("ScalarDeg2")
  {
    CHECK(static_cast<double>(ScalarDeg2(1)) == 1);
    CHECK(static_cast<double>(ScalarDeg2(1234.5)) == 1234.5);
    CHECK(static_cast<double>(ScalarDeg2(-551.75)) == -551.75);
  }
}

TEST_CASE("Scalar from/to numerator")
{
  SECTION("ScalarDeg1")
  {
    ScalarDeg1 scalar = ScalarDeg1::from_numerator(0x7f9a4111);
    CHECK(scalar.numerator() == 0x7f9a4111);
    CHECK(static_cast<double>(scalar) == 0x7f9a4111 * ScalarDeg1::quantum);
  }

  SECTION("ScalarDeg2")
  {
    ScalarDeg2 scalar = ScalarDeg2::from_numerator(0x15a2e8b30f8aa5c7);
    CHECK(scalar.numerator() == 0x15a2e8b30f8aa5c7);
    CHECK(static_cast<double>(scalar) == 0x15a2e8b30f8aa5c7 * ScalarDeg2::quantum);
  }
}

TEST_CASE("Scalar comparison")
{
  SECTION("Less than")
  {
    ScalarDeg1 a(-63.617);
    ScalarDeg1 b(474.11);
    CHECK_FALSE(a == b);
    CHECK(a != b);
    CHECK(a < b);
    CHECK(a <= b);
    CHECK_FALSE(a >= b);
    CHECK_FALSE(a > b);
  }

  SECTION("Equal")
  {
    ScalarDeg1 a(11.64);
    ScalarDeg1 b(11.64);
    CHECK(a == b);
    CHECK_FALSE(a != b);
    CHECK_FALSE(a < b);
    CHECK(a <= b);
    CHECK(a >= b);
    CHECK_FALSE(a > b);
  }

  SECTION("Greater than")
  {
    ScalarDeg1 a(1616.337);
    ScalarDeg1 b(493.619);
    CHECK_FALSE(a == b);
    CHECK(a != b);
    CHECK_FALSE(a < b);
    CHECK_FALSE(a <= b);
    CHECK(a >= b);
    CHECK(a > b);
  }
}

TEST_CASE("Scalar agaist double comparison")
{
  SECTION("Less than")
  {
    ScalarDeg1 a(-63.617);
    double b = 474.11;
    CHECK_FALSE(a == b);
    CHECK(a != b);
    CHECK(a < b);
    CHECK(a <= b);
    CHECK_FALSE(a >= b);
    CHECK_FALSE(a > b);
  }

  SECTION("Equal")
  {
    ScalarDeg1 a(11.64);
    double b = 11.64;
    CHECK(a == b);
    CHECK_FALSE(a != b);
    CHECK_FALSE(a < b);
    CHECK(a <= b);
    CHECK(a >= b);
    CHECK_FALSE(a > b);
  }

  SECTION("Greater than")
  {
    ScalarDeg1 a(1616.337);
    double b = 493.619;
    CHECK_FALSE(a == b);
    CHECK(a != b);
    CHECK_FALSE(a < b);
    CHECK_FALSE(a <= b);
    CHECK(a >= b);
    CHECK(a > b);
  }

  SECTION("Compares against rounded rhs")
  {
    SECTION("Nearest is up")
    {
      ScalarDeg1 a(236236 * ScalarDeg1::quantum);
      double b = 236235.6 * ScalarDeg1::quantum;

      CHECK(a == b);
      CHECK_FALSE(a != b);
      CHECK_FALSE(a < b);
      CHECK(a <= b);
      CHECK(a >= b);
      CHECK_FALSE(a > b);
    }

    SECTION("Nearest is down")
    {
      ScalarDeg1 a(236236 * ScalarDeg1::quantum);
      double b = 236236.4 * ScalarDeg1::quantum;

      CHECK(a == b);
      CHECK_FALSE(a != b);
      CHECK_FALSE(a < b);
      CHECK(a <= b);
      CHECK(a >= b);
      CHECK_FALSE(a > b);
    }
  }
}

TEST_CASE("Scalar::operator+")
{
  ScalarDeg1 a(-4323);
  ScalarDeg1 b(2675);
  CHECK(a + b == -4323 + 2675);
}

TEST_CASE("Scalar::operator-")
{
  ScalarDeg1 a(3294);
  ScalarDeg1 b(8384);
  CHECK(a - b == 3294 - 8384);
}

TEST_CASE("Unary Scalar::operator-")
{
  SECTION("Positive to negative")
  {
    ScalarDeg1 a(8143);
    CHECK(-a == -8143);
  }

  SECTION("Negative to positive")
  {
    ScalarDeg1 a(-8143);
    CHECK(-a == 8143);
  }
}

TEST_CASE("Scalar multiplication")
{
  SECTION("ScalarDeg1 * ScalarDeg1")
  {
    ScalarDeg1 a = ScalarDeg1::from_numerator(932655148);
    ScalarDeg1 b = ScalarDeg1::from_numerator(2075214922);
    CHECK(a * b == ScalarDeg2::from_numerator(1935459880209718456));
  }
}

TEST_CASE("Scalar printing")
{
  SECTION("ScalarDeg1")
  {
    std::stringstream s;
    s << ScalarDeg1(1623.36);
    CHECK(s.str() == "1623.36");
  }

  SECTION("ScalarDeg2")
  {
    std::stringstream s;
    s << ScalarDeg2(1623.36);
    CHECK(s.str() == "1623.36");
  }
}

} // namespace dida