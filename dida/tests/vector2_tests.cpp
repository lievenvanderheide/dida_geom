#include "dida/vector2.hpp"

#include <catch2/catch.hpp>

namespace dida
{

TEST_CASE("Vector2(ScalarDeg1, ScalarDeg1) and access")
{
  Vector2 a(ScalarDeg1(14.39), ScalarDeg1(-92.59));
  CHECK(a.x() == ScalarDeg1(14.39));
  CHECK(a.y() == ScalarDeg1(-92.59));
}

TEST_CASE("Vector2(double, double)")
{
  Vector2 a(14.39, -92.59);
  CHECK(a.x() == ScalarDeg1(14.39));
  CHECK(a.y() == ScalarDeg1(-92.59));
}

TEST_CASE("Vector2::from_grid_coordinates")
{
  Vector2 a = Vector2::from_grid_coordinates(-4679, -868);
  CHECK(a.x() == ScalarDeg1(-4679 * ScalarDeg1::quantum));
  CHECK(a.y() == ScalarDeg1(-868 * ScalarDeg1::quantum));
}

TEST_CASE("Vector2::operator==/!=")
{
  SECTION("Equal")
  {
    Vector2 a(-4792, -5488);
    Vector2 b(-4792, -5488);
    CHECK(a == b);
    CHECK_FALSE(a != b);
  }

  SECTION("X different")
  {
    Vector2 a(5401, 1272);
    Vector2 b(9522, 1272);
    CHECK_FALSE(a == b);
    CHECK(a != b);
  }

  SECTION("Y different")
  {
    Vector2 a(-2983, -4196);
    Vector2 b(-2983, -6163);
    CHECK_FALSE(a == b);
    CHECK(a != b);
  }
}

TEST_CASE("Vector2::operator+")
{
  Vector2 a(-8432, -9563);
  Vector2 b(-4608, 3820);
  Vector2 result = a + b;
  CHECK(result == Vector2(-8432 + -4608, -9563 + 3820));
}

TEST_CASE("Vector2::operator-")
{
  Vector2 a(-7674, 6543);
  Vector2 b(3937, 4133);
  Vector2 result = a - b;
  CHECK(result == Vector2(-7674 - 3937, 6543 - 4133));
}

TEST_CASE("Vector2::operator- unary")
{
  Vector2 a(1218, -774);
  Vector2 result = -a;
  CHECK(result == Vector2(-1218, 774));
}

TEST_CASE("dot")
{
  Vector2 a(-2455, 1736);
  Vector2 b(5419, -8972);
  CHECK(dot(a, b) == ScalarDeg2(-2455 * 5419 + 1736 * -8972));
}

TEST_CASE("cross")
{
  Vector2 a(-8884, 4515);
  Vector2 b(2452, 3189);
  CHECK(cross(a, b) == ScalarDeg2(-8884 * 3189 - 4515 * 2452));
}

TEST_CASE("left_perpendicular")
{
  Vector2 a(9444, 2730);
  CHECK(left_perpendicular(a) == Vector2(-2730, 9444));
}

TEST_CASE("right_perpendicular")
{
  Vector2 a(6740, 4550);
  CHECK(right_perpendicular(a) == Vector2(4550, -6740));
}

} // namespace dida