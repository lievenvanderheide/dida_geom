#include "dida/box2.hpp"

#include <catch2/catch_test_macros.hpp>
#include <sstream>

namespace dida
{

TEST_CASE("Box2(Point2, Point2) and access")
{
  Box2 box(Point2(-4.19, -5.96), Point2(8.66, 9.78));
  CHECK(box.min() == Point2(-4.19, -5.96));
  CHECK(box.max() == Point2(8.66, 9.78));
}

TEST_CASE("Box2::diag")
{
  Box2 box(Point2(-626, -84), Point2(823, 669));
  CHECK(box.diag() == Vector2(823 - -626, 669 - -84));
}

TEST_CASE("Box2 printing")
{
  std::stringstream s;
  s << Box2(Point2(-626, -84), Point2(823, 669));
  CHECK(s.str() == "{{-626, -84}, {823, 669}}");
}

} // namespace dida