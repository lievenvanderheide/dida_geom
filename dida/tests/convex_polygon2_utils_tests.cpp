#include "dida/convex_polygon2_utils.hpp"

#include <catch2/catch.hpp>

namespace dida
{

namespace
{

ConvexPolygonView2::const_iterator ref_leftmost_vertex(ConvexPolygonView2 polygon)
{
  return std::min_element(polygon.begin(), polygon.end(), lex_less_than);
}

ConvexPolygonView2::const_iterator ref_rightmost_vertex(ConvexPolygonView2 polygon)
{
  return std::max_element(polygon.begin(), polygon.end(), lex_less_than);
}

} // namespace

TEST_CASE("leftmost_vertex/rightmost_vertex")
{
  auto test = [](std::vector<Point2> vertices)
  {
    for (size_t i = 0; i < vertices.size(); i++)
    {
      ConvexPolygonView2 polygon(vertices);
      CHECK(leftmost_vertex(polygon) == ref_leftmost_vertex(polygon));
      CHECK(rightmost_vertex(polygon) == ref_rightmost_vertex(polygon));

      std::rotate(vertices.begin(), vertices.begin() + 1, vertices.end());
    }
  };

  SECTION("General")
  {
    test({{-4.18, 1.66},
          {-3.66, -0.48},
          {-2.12, -3.6},
          {1.96, -4.5},
          {6.3, -2.36},
          {11.08, 2.14},
          {10.94, 6.52},
          {7.9, 8.78},
          {2.76, 8.4},
          {-2.94, 4.82}});
  }

  SECTION("Many more vertices on lower arc")
  {
    test({{-6.72, -1.86},
          {-6.38, -2.58},
          {-5.62, -3.66},
          {-4.68, -4.4},
          {-3.66, -4.84},
          {-2.54, -4.88},
          {-1.44, -4.78},
          {-0.32, -4.42},
          {0.74, -3.54},
          {1.5, -2.6},
          {1.72, -1.74},
          {1.72, -0.8},
          {-1.74, 2.06},
          {-4.44, 1.38}});
  }

  SECTION("Many more vertices on upper arc")
  {
    test({
        {1.72, -2.84}, {3.62, -4.82}, {7.76, -5.32}, {11.84, -2.44}, {11.86, -1.46}, {11.56, -0.24}, {11.14, 0.54},
        {10.42, 1.26}, {9.64, 1.8},   {8.68, 2.34},  {7.7, 2.66},    {6.34, 2.84},   {5.52, 2.78},   {4.84, 2.6},
        {4.12, 2.18},  {3.48, 1.64},  {2.8, 0.7},    {2.52, 0.14},   {2.14, -0.64},  {1.84, -1.66},
    });
  }

  SECTION("With vertical edges")
  {
    test({{-4, 3}, {-4, 2}, {-2, 1}, {1, 3}, {1, 5}, {-1, 6}});
  }
}

} // namespace dida