#include "dida/polygon2_utils.hpp"

#include <catch2/catch_test_macros.hpp>

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

TEST_CASE("geometrically_equal")
{
  Polygon2 a{{-2.38, 1.64}, {2.84, 4.86}, {5.10, 2.96}, {5.36, 9.02}, {-5.08, 6.72}, {-0.12, 5.32}};

  SECTION("Equal")
  {
    Polygon2 b = a;
    for (size_t i = 0; i < b.size(); i++)
    {
      CHECK(geometrically_equal(a, b));

      std::vector<Point2>& b_vertices = b.unsafe_mutable_vertices();
      std::rotate(b_vertices.begin(), b_vertices.begin() + 1, b_vertices.end());
    }
  }

  SECTION("Not equal, same number of vertices")
  {
    Polygon2 b{{-2.90, 0.32}, {2.84, 4.86}, {5.10, 2.96}, {5.36, 9.02}, {-5.08, 6.72}, {-0.12, 5.32}};

    for (size_t i = 0; i < b.size(); i++)
    {
      CHECK(!geometrically_equal(a, b));

      std::vector<Point2>& b_vertices = b.unsafe_mutable_vertices();
      std::rotate(b_vertices.begin(), b_vertices.begin() + 1, b_vertices.end());
    }
  }

  SECTION("Not equal, extra vertex")
  {
    Polygon2 b{{-2.38, 1.64}, {0.9, 0.8}, {2.84, 4.86}, {5.10, 2.96}, {5.36, 9.02}, {-5.08, 6.72}, {-0.12, 5.32}};

    for (size_t i = 0; i < b.size(); i++)
    {
      CHECK(!geometrically_equal(a, b));
      CHECK(!geometrically_equal(b, a));

      std::vector<Point2>& b_vertices = b.unsafe_mutable_vertices();
      std::rotate(b_vertices.begin(), b_vertices.begin() + 1, b_vertices.end());
    }
  }
}

TEST_CASE("triangulate")
{
  // The algorithms to compute the vertical decomposition and to triangulate it are already thoroguhly tested, so a
  // simple sanity check is enough.

  Polygon2 polygon{{-5.26, 2.34}, {-3.02, 5.46}, {-1.22, 1.94}, {3.20, 6.74}, {-6.94, 6.22}};
  std::vector<Triangle2> triangulation = triangulate(polygon);

  // Note that there's only 1 possible triangulation of the polygon we're using.
  std::vector<Triangle2> expected_triangulation{
      Triangle2(std::array{polygon[4], polygon[0], polygon[1]}),
      Triangle2(std::array{polygon[1], polygon[2], polygon[3]}),
      Triangle2(std::array{polygon[4], polygon[1], polygon[3]}),
  };

  CHECK(std::is_permutation(triangulation.begin(), triangulation.end(), expected_triangulation.begin(),
                            expected_triangulation.end(), geometrically_equal));
}

} // namespace dida