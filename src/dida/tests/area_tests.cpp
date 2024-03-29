#include "dida/area.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

namespace dida
{

TEST_CASE("area")
{
  ConvexPolygon2 polygon{{-4, 1}, {-2, -4}, {4, -6}, {6, -3}, {4, 3}, {-2, 2}};
  CHECK(area(polygon) == Catch::Approx(60));
}

TEST_CASE("intersection_area(ConvexPolygonView2, ConvexPolygonView2)")
{
  SECTION("Disjoint")
  {
    ConvexPolygon2 a{{-1, 4}, {3, 3}, {2, 5}};
    ConvexPolygon2 b{{0, 2}, {1, -2}, {1, 3}};
    CHECK(intersection_area(a, b) == 0);
  }

  SECTION("A leftmost in B")
  {
    ConvexPolygon2 a{{-3, 3}, {3, -3}, {7, -1}, {9, 3}, {2, 5}};
    ConvexPolygon2 b{{8, 5}, {-1, 6}, {-4, 3}, {-3, 0}, {9, -3}, {10, 2}};
    ConvexPolygon2 intersection_polygon{{-3, 3}, {1, -1}, {5, -2}, {7, -1}, {9, 3}, {2, 5}};
    CHECK(intersection_area(a, b) == area(intersection_polygon));
  }

  SECTION("B leftmost in A")
  {
    ConvexPolygon2 a{{9, 5}, {7, 6}, {1, 5}, {-3, 3}, {-2, -1}, {2, -2}, {8, 1}, {10, 3}};
    ConvexPolygon2 b{{1, 8}, {-2, 2}, {-1, -1}, {1, -2}, {5, -3}, {7, 3}};
    ConvexPolygon2 intersection_polygon{{0, -1.5}, {2, -2}, {6, 0},  {7, 3},  {4, 5.5},
                                        {1, 5},    {-1, 4}, {-2, 2}, {-1, -1}};
    CHECK(intersection_area(a, b) == area(intersection_polygon));
  }

  SECTION("Both leftmost vertices outside other")
  {
    ConvexPolygon2 a{{-2, -1}, {3, -3}, {8, -3}, {5, 3}, {-2, 3}};
    ConvexPolygon2 b{{-3, 1}, {7, -4}, {8, 0}, {0, 4}};
    ConvexPolygon2 intersection_polygon{{-1, 3}, {-2, 2}, {-2, .5}, {5, -3}, {7.25, -3}, {7.5, -2}, {6, 1}, {2, 3}};
    CHECK(intersection_area(a, b) == area(intersection_polygon));
  }
}

TEST_CASE("intersection_over_union(ConvexPolygonView2, ConvexPolygonView2")
{
  ConvexPolygon2 a{{-3, 1}, {2, 1}, {2, 4}, {-3, 4}};
  ConvexPolygon2 b{{-1, -1}, {5, -1}, {5, 3}, {-1, 3}};
  CHECK(intersection_over_union(a, b) == 6.0 / 33.0);
}

} // namespace dida