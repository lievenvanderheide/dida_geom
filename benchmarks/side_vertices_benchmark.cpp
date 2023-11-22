#include "dida/convex_polygon2_utils.hpp"

#include <catch2/benchmark/catch_benchmark_all.hpp>
#include <catch2/catch_test_macros.hpp>
#include <cmath>

namespace dida::benchmarks
{

ConvexPolygon2 circle_polygon(Point2 center, double radius, size_t num_vertices, double angle_offset)
{
  std::vector<Point2> vertices(num_vertices);

  for (size_t i = 0; i < num_vertices; i++)
  {
    double angle = angle_offset + static_cast<double>(i) / static_cast<double>(num_vertices);
    Vector2 radius_vector(radius * std::cos(angle), radius * std::sin(angle));
    vertices[i] = center + radius_vector;
  }

  return ConvexPolygon2(std::move(vertices));
}

TEST_CASE("leftmost_vertex benchmark")
{
  {
    ConvexPolygon2 polygon = circle_polygon(Point2(2, 6), 10, 3, .3 * M_PI);
    BENCHMARK("3 vertices")
    {
      return leftmost_vertex(polygon);
    };
  }

  {
    ConvexPolygon2 polygon = circle_polygon(Point2(2, 6), 10, 4, .3 * M_PI);
    BENCHMARK("4 vertices")
    {
      return leftmost_vertex(polygon);
    };
  }

  {
    ConvexPolygon2 polygon = circle_polygon(Point2(2, 6), 10, 4, .3 * M_PI);
    BENCHMARK("5 vertices")
    {
      return leftmost_vertex(polygon);
    };
  }

  {
    ConvexPolygon2 polygon = circle_polygon(Point2(2, 6), 10, 20, .3 * M_PI);
    BENCHMARK("20 vertices")
    {
      return leftmost_vertex(polygon);
    };
  }

  {
    ConvexPolygon2 polygon = circle_polygon(Point2(2, 6), 10, 100, .3 * M_PI);
    BENCHMARK("100 vertices")
    {
      return leftmost_vertex(polygon);
    };
  }
}

} // namespace dida::benchmarks