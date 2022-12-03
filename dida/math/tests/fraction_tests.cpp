#include "dida/math/fraction.hpp"

#include <catch2/catch.hpp>

namespace dida::math
{

TEST_CASE("Fraction construction and access")
{
  Fraction<int, int> fraction(-792, 851);
  CHECK(fraction.numerator() == -792);
  CHECK(fraction.denominator() == 851);
}

TEST_CASE("Fraction comparison operators")
{
  SECTION("a < b")
  {
    Fraction<int, int> a(811, 964);
    Fraction<int, int> b(708, 533);
    
    CHECK_FALSE(a == b);
    CHECK(a != b);
    CHECK(a < b);
    CHECK(a <= b);
    CHECK_FALSE(a >= b);
    CHECK_FALSE(a > b);
  }

  SECTION("a == b")
  {
    Fraction<int, int> a(4480, 890);
    Fraction<int, int> b(6272, 1246);
    
    CHECK(a == b);
    CHECK_FALSE(a != b);
    CHECK_FALSE(a < b);
    CHECK(a <= b);
    CHECK(a >= b);
    CHECK_FALSE(a > b);
  }

  SECTION("a > b")
  {
    Fraction<int, int> a(657, 36);
    Fraction<int, int> b(617, 364);
    
    CHECK_FALSE(a == b);
    CHECK(a != b);
    CHECK_FALSE(a < b);
    CHECK_FALSE(a <= b);
    CHECK(a >= b);
    CHECK(a > b);
  }
}

} // namespace dida::math