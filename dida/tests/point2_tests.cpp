#include "dida/point2.hpp"

#include <catch2/catch.hpp>
#include <sstream>
#include <unordered_set>

namespace dida
{

TEST_CASE("Point2(ScalarDeg1, ScalarDeg1) and access")
{
  Point2 a(ScalarDeg1(2849), ScalarDeg1(7045));
  CHECK(a.x() == 2849);
  CHECK(a.y() == 7045);
}

TEST_CASE("Point2(double, double)")
{
  Point2 a(-18.86, 2.88);
  CHECK(a.x() == -18.86);
  CHECK(a.y() == 2.88);
}

TEST_CASE("Point2 from/to Vector2")
{
  Point2 a(Vector2(9298, -6690));
  CHECK(a.x() == 9298);
  CHECK(a.y() == -6690);

  Vector2 as_vector = static_cast<Vector2>(a);
  CHECK(as_vector.x() == 9298);
  CHECK(as_vector.y() == -6690);
}

TEST_CASE("Point2::operator==/!=")
{
  SECTION("Equal")
  {
    Vector2 a(9785, 3903);
    Vector2 b(9785, 3903);
    CHECK(a == b);
    CHECK_FALSE(a != b);
  }

  SECTION("X different")
  {
    Vector2 a(9785, 3903);
    Vector2 b(2349, 3903);
    CHECK_FALSE(a == b);
    CHECK(a != b);
  }

  SECTION("Y different")
  {
    Vector2 a(9785, 3903);
    Vector2 b(9785, -3578);
    CHECK_FALSE(a == b);
    CHECK(a != b);
  }
}

TEST_CASE("Point2::operator-(Point2)")
{
  Point2 a(5320, 7885);
  Point2 b(4662, -6416);
  CHECK(a - b == Vector2(5320 - 4662, 7885 - -6416));
}

TEST_CASE("Point2::operator+(Vector2)")
{
  Point2 a(-4031, -8208);
  Vector2 b(-2121, -5369);
  CHECK(a + b == Point2(-4031 + -2121, -8208 + -5369));
}

TEST_CASE("Point2::operator-(Vector2)")
{
  Point2 a(747, 6810);
  Vector2 b(3052, 7932);
  CHECK(a + b == Point2(747 + 3052, 6810 + 7932));
}

TEST_CASE("lex_less_than/lex_greater_than")
{
  SECTION("X less than")
  {
    Point2 a(-8872, 3321);
    Point2 b(914, -7840);
    CHECK(lex_less_than(a, b));
    CHECK_FALSE(lex_greater_than(a, b));
  }

  SECTION("X greater than")
  {
    Point2 a(-5813, -9424);
    Point2 b(-6797, -1844);
    CHECK_FALSE(lex_less_than(a, b));
    CHECK(lex_greater_than(a, b));
  }

  SECTION("X equal, y less than")
  {
    Point2 a(-6478, 1557);
    Point2 b(-6478, 7267);
    CHECK(lex_less_than(a, b));
    CHECK_FALSE(lex_greater_than(a, b));
  }

  SECTION("X equal, y greater than")
  {
    Point2 a(6251, 8457);
    Point2 b(6251, -9366);
    CHECK_FALSE(lex_less_than(a, b));
    CHECK(lex_greater_than(a, b));
  }

  SECTION("Equal")
  {
    Point2 a(8568, -6933);
    Point2 b(8568, -6933);
    CHECK_FALSE(lex_less_than(a, b));
    CHECK_FALSE(lex_greater_than(a, b));
  }
}

TEST_CASE("Point2 printing")
{
  std::stringstream s;
  s << Point2(4021, -9580);
  CHECK(s.str() == "{4021, -9580}");
}

TEST_CASE("Point2 hashing")
{
  std::unordered_set<Point2> set;

  Point2 a{-1.06, 3.04};
  Point2 b{4.40, 5.52};
  Point2 c{4.64, 0.78};

  set.insert(a);
  set.insert(b);

  CHECK(set.find(a) != set.end());
  CHECK(set.find(b) != set.end());
  CHECK(set.find(c) == set.end());
}

} // namespace dida