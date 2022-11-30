#include "dida/area.hpp"

#include <catch2/catch.hpp>

namespace dida
{

TEST_CASE("area")
{
  ConvexPolygon2 polygon{{-4, 1}, {-2, -4}, {4, -6}, {6, -3}, {4, 3}, {-2, 2}};
  CHECK(area(polygon) == Approx(60));
}

} // namespace dida