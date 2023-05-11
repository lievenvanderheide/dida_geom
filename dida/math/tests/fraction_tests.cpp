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

TEST_CASE("Fraction - scalar comparison operators")
{
  SECTION("a < b")
  {
    Fraction<int, int> a(24, 3);
    int b = 9;

    CHECK_FALSE(a == b);
    CHECK(a != b);
    CHECK(a < b);
    CHECK(a <= b);
    CHECK_FALSE(a >= b);
    CHECK_FALSE(a > b);
  }

  SECTION("a == b")
  {
    Fraction<int, int> a(24, 3);
    int b = 8;
    CHECK(a == b);
    CHECK_FALSE(a != b);
    CHECK_FALSE(a < b);
    CHECK(a <= b);
    CHECK(a >= b);
    CHECK_FALSE(a > b);
  }

  SECTION("a > b")
  {
    Fraction<int, int> a(24, 3);
    int b = 7;
    CHECK_FALSE(a == b);
    CHECK(a != b);
    CHECK_FALSE(a < b);
    CHECK_FALSE(a < b);
    CHECK(a >= b);
    CHECK(a > b);
  }
}

TEST_CASE("Fraction::infinity")
{
  Fraction<int, int> inf = Fraction<int, int>::infinity();

  SECTION("Compare against finite fraction")
  {
    Fraction<int, int> a(56, 15);

    CHECK_FALSE(a == inf);
    CHECK(a != inf);
    CHECK(a < inf);
    CHECK(a <= inf);
    CHECK_FALSE(a >= inf);
    CHECK_FALSE(a > inf);
  }

  SECTION("Compare against itself")
  {
    CHECK(inf == inf);
    CHECK_FALSE(inf != inf);
    CHECK_FALSE(inf < inf);
    CHECK(inf <= inf);
    CHECK(inf >= inf);
    CHECK_FALSE(inf > inf);
  }
}

TEST_CASE("Fraction::negative_infinity")
{
  Fraction<int, int> neg_inf = Fraction<int, int>::negative_infinity();

  SECTION("Compare against finite fraction")
  {
    Fraction<int, int> a(56, 15);

    CHECK_FALSE(a == neg_inf);
    CHECK(a != neg_inf);
    CHECK_FALSE(a < neg_inf);
    CHECK_FALSE(a <= neg_inf);
    CHECK(a >= neg_inf);
    CHECK(a > neg_inf);
  }

  SECTION("Compare against itself")
  {
    CHECK(neg_inf == neg_inf);
    CHECK_FALSE(neg_inf != neg_inf);
    CHECK_FALSE(neg_inf < neg_inf);
    CHECK(neg_inf <= neg_inf);
    CHECK(neg_inf >= neg_inf);
    CHECK_FALSE(neg_inf > neg_inf);
  }
}

} // namespace dida::math