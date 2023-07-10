#include "dida/detail/tests/vertical_decomposition_test_utils.hpp"

#include <catch2/catch.hpp>

namespace dida::detail::vertical_decomposition
{

TEST_CASE("ray_cast_up")
{
  std::vector<Point2> vertices_storage{
      {1.98, -2.24}, {7.44, 0.74}, {2.38, 2.48}, {5.06, 0.68}, {2.26, -0.92}, {0.26, 3.38}, {4.98, 4.52}, {-1.16, 4.02},
  };

  ArrayView<const Point2> vertices(vertices_storage);

  SECTION("Full polygon, hits edge from inside")
  {
    for (size_t i = 0; i < vertices.size(); i++)
    {
      Edge edge =
          ray_cast_up(vertices, PolygonRange{i, vertices.size(), vertices[i].x(), vertices[i].x()}, {4.06, -0.64});
      REQUIRE(edge.is_valid());
      CHECK(*edge.start_vertex_it == Point2(5.06, 0.68));
      CHECK(*edge.end_vertex_it == Point2(2.26, -0.92));
    }
  }

  SECTION("Full polygon, hits edge from outside")
  {
    for (size_t i = 0; i < vertices.size(); i++)
    {
      Edge edge =
          ray_cast_up(vertices, PolygonRange{i, vertices.size(), vertices[i].x(), vertices[i].x()}, {3.26, 1.16});
      CHECK(!edge.is_valid());
    }
  }

  SECTION("Full polygon, no hit")
  {
    for (size_t i = 0; i < vertices.size(); i++)
    {
      Edge edge =
          ray_cast_up(vertices, PolygonRange{i, vertices.size(), vertices[i].x(), vertices[i].x()}, {6.36, 3.32});
      CHECK(!edge.is_valid());
    }
  }

  SECTION("Hits vertex -> return left side edge")
  {
    for (size_t i = 0; i < vertices.size(); i++)
    {
      Edge edge =
          ray_cast_up(vertices, PolygonRange{i, vertices.size(), vertices[i].x(), vertices[i].x()}, {2.26, -1.52});
      REQUIRE(edge.is_valid());
      CHECK(*edge.start_vertex_it == Point2(2.26, -0.92));
      CHECK(*edge.end_vertex_it == Point2(0.26, 3.38));
    }
  }

  SECTION("ray_origin on edge -> ignore edge")
  {
    for (size_t i = 0; i < vertices.size(); i++)
    {
      Edge edge =
          ray_cast_up(vertices, PolygonRange{i, vertices.size(), vertices[i].x(), vertices[i].x()}, {2.26, -0.92});
      CHECK(!edge.is_valid());
    }
  }

  SECTION("Open range, hits edge from inside")
  {
    Edge edge = ray_cast_up(vertices, PolygonRange{1, 3, vertices[1].x(), vertices[4].x()}, {4.93, 1.26});
    REQUIRE(edge.is_valid());
    CHECK(*edge.start_vertex_it == Point2(7.44, 0.74));
    CHECK(*edge.end_vertex_it == Point2(2.38, 2.48));
  }

  SECTION("Open range, ignore closing edge")
  {
    Edge edge = ray_cast_up(vertices, PolygonRange{0, 3, vertices[0].x(), vertices[3].x()}, {4.45, -0.62});
    REQUIRE(!edge.is_valid());
  }

  SECTION("Hits partial first edge")
  {
    Edge edge = ray_cast_up(vertices, PolygonRange{3, 3, ScalarDeg1(4), vertices[6].x()}, {3.27, -0.82});
    REQUIRE(edge.is_valid());
    CHECK(*edge.start_vertex_it == Point2(5.06, 0.68));
    CHECK(*edge.end_vertex_it == Point2(2.26, -0.92));
  }

  SECTION("Misses partial first edge")
  {
    Edge edge = ray_cast_up(vertices, PolygonRange{3, 3, ScalarDeg1(3), vertices[6].x()}, {3.27, -0.82});
    CHECK(!edge.is_valid());
  }

  SECTION("Hits partial last edge")
  {
    Edge edge = ray_cast_up(vertices, PolygonRange{0, 4, vertices[0].x(), ScalarDeg1(3)}, {3.27, -0.82});
    REQUIRE(edge.is_valid());
    CHECK(*edge.start_vertex_it == Point2(5.06, 0.68));
    CHECK(*edge.end_vertex_it == Point2(2.26, -0.92));
  }

  SECTION("Misses partial last edge")
  {
    Edge edge = ray_cast_up(vertices, PolygonRange{0, 4, vertices[0].x(), ScalarDeg1(4)}, {3.27, -0.82});
    CHECK(!edge.is_valid());
  }

  SECTION("Open range, with wrap")
  {
    Edge edge = ray_cast_up(vertices, PolygonRange{6, 6, vertices[6].x(), vertices[4].x()}, {3.48, -0.40});
    REQUIRE(edge.is_valid());
    CHECK(*edge.start_vertex_it == Point2(5.06, 0.68));
    CHECK(*edge.end_vertex_it == Point2(2.26, -0.92));
  }
}

TEST_CASE("ray_cast_down")
{
  std::vector<Point2> vertices_storage{
      {-2.41, 3.78}, {-4.93, 1.68}, {1.45, -0.08}, {-2.47, -0.96},
      {1.81, -2.16}, {5.21, -1.82}, {2.65, 2.3},   {-3.69, 1.76},
  };

  ArrayView<const Point2> vertices(vertices_storage);

  SECTION("Full polygon, hits edge from inside")
  {
    for (size_t i = 0; i < vertices.size(); i++)
    {
      Edge edge =
          ray_cast_down(vertices, PolygonRange{i, vertices.size(), vertices[i].x(), vertices[i].x()}, {0.47, 1.12});
      REQUIRE(edge.is_valid());
      CHECK(*edge.start_vertex_it == Point2(-4.93, 1.68));
      CHECK(*edge.end_vertex_it == Point2(1.45, -0.08));
    }
  }

  SECTION("Full polygon, hits edge from outside")
  {
    for (size_t i = 0; i < vertices.size(); i++)
    {
      Edge edge =
          ray_cast_down(vertices, PolygonRange{i, vertices.size(), vertices[i].x(), vertices[i].x()}, {-1.29, 2.38});
      CHECK(!edge.is_valid());
    }
  }

  SECTION("Full polygon, no hit")
  {
    for (size_t i = 0; i < vertices.size(); i++)
    {
      Edge edge =
          ray_cast_down(vertices, PolygonRange{i, vertices.size(), vertices[i].x(), vertices[i].x()}, {-3.63, -0.68});
      CHECK(!edge.is_valid());
    }
  }

  SECTION("Hits vertex -> return right side edge")
  {
    for (size_t i = 0; i < vertices.size(); i++)
    {
      Edge edge =
          ray_cast_down(vertices, PolygonRange{i, vertices.size(), vertices[i].x(), vertices[i].x()}, {1.81, -0.98});
      REQUIRE(edge.is_valid());
      CHECK(*edge.start_vertex_it == Point2(1.81, -2.16));
      CHECK(*edge.end_vertex_it == Point2(5.21, -1.82));
    }
  }

  SECTION("ray_origin on edge -> ignore edge")
  {
    for (size_t i = 0; i < vertices.size(); i++)
    {
      Edge edge =
          ray_cast_down(vertices, PolygonRange{i, vertices.size(), vertices[i].x(), vertices[i].x()}, {2.65, 2.3});
      REQUIRE(edge.is_valid());
      CHECK(*edge.start_vertex_it == Point2(1.81, -2.16));
      CHECK(*edge.end_vertex_it == Point2(5.21, -1.82));
    }
  }

  SECTION("Open range, hits edge from inside")
  {
    Edge edge =
        ray_cast_down(vertices, PolygonRange{2, 4, vertices[2].x(), vertices[6].x()}, {0.42, -1.22});
    REQUIRE(edge.is_valid());
    CHECK(*edge.start_vertex_it == Point2(-2.47, -0.96));
    CHECK(*edge.end_vertex_it == Point2(1.81, -2.16));
  }

  SECTION("Open range, ignore closing edge")
  {
    Edge edge =
        ray_cast_down(vertices, PolygonRange{2, 5, vertices[2].x(), vertices[7].x()}, {-0.55, 1.58});
    CHECK(!edge.is_valid());
  }

  SECTION("Hits partial first edge")
  {
    Edge edge = ray_cast_down(vertices, PolygonRange{1, 3, ScalarDeg1(-4), ScalarDeg1(1)}, {-1.36, 1.41});
    REQUIRE(edge.is_valid());
    CHECK(*edge.start_vertex_it == Point2(-4.93, 1.68));
    CHECK(*edge.end_vertex_it == Point2(1.45, -0.08));
  }

  SECTION("Misses partial first edge")
  {
    Edge edge =
        ray_cast_down(vertices, PolygonRange{2, 4, ScalarDeg1(-2), ScalarDeg1(3.5)}, {-1.20, -0.24});
    REQUIRE(edge.is_valid());
    CHECK(*edge.start_vertex_it == Point2(-2.47, -0.96));
    CHECK(*edge.end_vertex_it == Point2(1.81, -2.16));
  }

  SECTION("Hits partial last edge")
  {
    Edge edge = ray_cast_down(vertices, PolygonRange{1, 3, ScalarDeg1(-4), ScalarDeg1(1)}, {-0.88, -1.08});
    REQUIRE(edge.is_valid());
    CHECK(*edge.start_vertex_it == Point2(-2.47, -0.96));
    CHECK(*edge.end_vertex_it == Point2(1.81, -2.16));
  }

  SECTION("Misses partial last edge")
  {
    Edge edge =
        ray_cast_down(vertices, PolygonRange{1, 6, ScalarDeg1(-2.5), ScalarDeg1(1.8)}, {0.36, 3.30});
    REQUIRE(edge.is_valid());
    CHECK(*edge.start_vertex_it == Point2(-4.93, 1.68));
    CHECK(*edge.end_vertex_it == Point2(1.45, -0.08));
  }

  SECTION("Open range, with wrap")
  {
    Edge edge =
        ray_cast_down(vertices, PolygonRange{6, 4, vertices[6].x(), vertices[2].x()}, {-4.26, 1.94});
    REQUIRE(edge.is_valid());
    CHECK(*edge.start_vertex_it == Point2(-4.93, 1.68));
    CHECK(*edge.end_vertex_it == Point2(1.45, -0.08));
  }
}

} // namespace dida::detail::vertical_decomposition