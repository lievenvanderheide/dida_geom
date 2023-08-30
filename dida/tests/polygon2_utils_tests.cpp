#include "dida/polygon2_utils.hpp"

#include <catch2/catch.hpp>

namespace dida
{

TEST_CASE("flip_horizontally")
{
  Polygon2 polygon{{-2.22, 2.68}, {0.82, 1.28}, {4.42, 4.64}, {1.98, 7.80}, {0.96, 4.06}};
  flip_horizontally(polygon);

  REQUIRE(polygon.size() == 5);
  CHECK(polygon[0] == Point2(-0.96, 4.06));
  CHECK(polygon[1] == Point2(-1.98, 7.80));
  CHECK(polygon[2] == Point2(-4.42, 4.64));
  CHECK(polygon[3] == Point2(-0.82, 1.28));
  CHECK(polygon[4] == Point2(2.22, 2.68));
}

TEST_CASE("flip_vertically")
{
  Polygon2 polygon{{-2.22, 2.68}, {0.82, 1.28}, {4.42, 4.64}, {1.98, 7.80}, {0.96, 4.06}};
  flip_vertically(polygon);

  REQUIRE(polygon.size() == 5);
  CHECK(polygon[0] == Point2(0.96, -4.06));
  CHECK(polygon[1] == Point2(1.98, -7.80));
  CHECK(polygon[2] == Point2(4.42, -4.64));
  CHECK(polygon[3] == Point2(0.82, -1.28));
  CHECK(polygon[4] == Point2(-2.22, -2.68));
}

} // namespace dida