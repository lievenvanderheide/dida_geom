#include "dida/convex_polygon2.hpp"

#include <catch2/catch.hpp>

namespace dida
{

TEST_CASE("ConvexPolygon2T(Storage) and access")
{
  std::vector<Point2> vertices{{-2.72, 0.42}, {-5.2, -2.58}, {1.3, -3.76}, {3.78, 2.12}, {2.76, 2.92}};
  ConvexPolygon2 polygon(vertices);

  REQUIRE(polygon.size() == vertices.size());
  for (size_t i = 0; i < polygon.size(); i++)
  {
    CHECK(polygon[i] == vertices[i]);
  }

  CHECK(std::equal(polygon.begin(), polygon.end(), vertices.begin(), vertices.end()));
}

TEST_CASE("ConvexPolygon2T(std::initializer_list) and access")
{
  std::initializer_list<Point2> vertices{{-2.72, 0.42}, {-5.2, -2.58}, {1.3, -3.76}, {3.78, 2.12}, {2.76, 2.92}};
  ConvexPolygon2 polygon(vertices);

  REQUIRE(polygon.size() == vertices.size());
  for (size_t i = 0; i < polygon.size(); i++)
  {
    CHECK(polygon[i] == *(vertices.begin() + i));
  }

  CHECK(std::equal(polygon.begin(), polygon.end(), vertices.begin(), vertices.end()));
}

TEST_CASE("ConvexPolygon2T::unsafe_from_vertices and access")
{
  std::vector<Point2> vertices{{-2.72, 0.42}, {-5.2, -2.58}, {1.3, -3.76}, {3.78, 2.12}, {2.76, 2.92}};
  ConvexPolygon2 polygon = ConvexPolygon2::unsafe_from_vertices(vertices);

  REQUIRE(polygon.size() == vertices.size());
  for (size_t i = 0; i < polygon.size(); i++)
  {
    CHECK(polygon[i] == vertices[i]);
  }

  CHECK(std::equal(polygon.begin(), polygon.end(), vertices.begin(), vertices.end()));
}

TEST_CASE("ConvexPolygon2T::operator ConvexPolygonView2")
{
  ConvexPolygon2 polygon{{-2.72, 0.42}, {-5.2, -2.58}, {1.3, -3.76}, {3.78, 2.12}, {2.76, 2.92}};

  ConvexPolygonView2 view = polygon;
  CHECK(std::equal(view.begin(), view.end(), polygon.begin(), polygon.end()));
}

TEST_CASE("ConvexPolygon2T::operator ArrayView<const Point2>")
{
  ConvexPolygon2 polygon{{1.90, 6.14}, {-1.66, 7.20}, {-3.94, 5.12}, {-1.70, 2.36}, {-0.02, 3.00}};

  ArrayView<const Point2> view = polygon;
  CHECK(std::equal(view.begin(), view.end(), polygon.begin(), polygon.end()));
}

TEST_CASE("ConvexPolygon2T::unsafe_mutable_vertices")
{
  ConvexPolygon2 polygon{{-2.72, 0.42}, {-5.2, -2.58}, {1.3, -3.76}, {3.78, 2.12}, {2.76, 2.92}};

  SECTION("Change vertices")
  {
    std::vector<Point2> new_vertices{{-3.38, 4.86}, {-4.96, -4.66}, {2.4, -3.38}, {4.84, 0.88}, {6.1, 5.58}};

    std::vector<Point2>& vertices = polygon.unsafe_mutable_vertices();
    std::copy(new_vertices.begin(), new_vertices.end(), vertices.begin());

    CHECK(std::equal(polygon.begin(), polygon.end(), new_vertices.begin(), new_vertices.end()));
  }

  SECTION("Resize and change vertices")
  {
    std::vector<Point2> new_vertices{{1.68, -2.68}, {2.74, -3.1}, {4.9, -3.5},  {7.46, -3.14}, {10.36, 0.48},
                                     {9.62, 3.86},  {5.5, 4.06},  {2.58, 1.48}, {1.8, -0.48}};

    std::vector<Point2>& vertices = polygon.unsafe_mutable_vertices();
    vertices.resize(new_vertices.size());
    std::copy(new_vertices.begin(), new_vertices.end(), vertices.begin());

    CHECK(std::equal(polygon.begin(), polygon.end(), new_vertices.begin(), new_vertices.end()));
  }
}

TEST_CASE("ConvexPolygonView2(ArrayView<const Point2>) and access")
{
  std::vector<Point2> vertices{{12.18, -1.16}, {2.84, 0.9}, {-1.94, -0.32}, {4.56, -3.18}};
  ConvexPolygonView2 view(vertices);

  REQUIRE(view.size() == vertices.size());
  for (size_t i = 0; i < view.size(); i++)
  {
    CHECK(view[i] == vertices[i]);
  }

  CHECK(std::equal(view.begin(), view.end(), vertices.begin(), vertices.end()));
}

TEST_CASE("ConvexPolygonView2::unsafe_from_vertices and access")
{
  std::vector<Point2> vertices{{12.18, -1.16}, {2.84, 0.9}, {-1.94, -0.32}, {4.56, -3.18}};
  ConvexPolygonView2 view = ConvexPolygonView2::unsafe_from_vertices(vertices);

  REQUIRE(view.size() == vertices.size());
  for (size_t i = 0; i < view.size(); i++)
  {
    CHECK(view[i] == vertices[i]);
  }

  CHECK(std::equal(view.begin(), view.end(), vertices.begin(), vertices.end()));
}

TEST_CASE("validate_convex_polygon_vertices")
{
  auto test = [](std::vector<Point2> vertices, bool expected_result)
  {
    for (size_t i = 0; i < vertices.size(); i++)
    {
      CHECK(validate_convex_polygon_vertices(vertices) == expected_result);
      std::rotate(vertices.begin(), vertices.begin() + 1, vertices.end());
    }
  };

  SECTION("Valid polygon")
  {
    test({{-7.56, 0.96},
          {-7.2, -1.9},
          {-5.6, -4.38},
          {-2.62, -5.32},
          {4.56, -2.38},
          {4.5, 1.46},
          {3.2, 4.2},
          {-0.58, 5.38},
          {-4.8, 3.94}},
         true);
  }

  SECTION("Triangle")
  {
    test({{0.98, -3.32}, {6.2, -3.4}, {8.58, 2.92}}, true);
  }

  SECTION("Too few vertices")
  {
    test({{-0.24, -2.18}, {9.86, 1.2}}, false);
  }

  SECTION("Duplicated vertices")
  {
    test({{6.86, -7.22}, {6.86, -7.22}, {12.38, -4.54}, {10.46, 2.84}, {4.96, -5.34}, {5.42, -6.64}}, false);
  }

  SECTION("Non convex")
  {
    test({{6.58, -2.26}, {5.56, -3.7}, {12.68, -1.16}, {8.98, 3.38}, {0.28, -2.94}}, false);
  }

  SECTION("Wrong winding")
  {
    test({{1.4, -1.6}, {4.72, 1.16}, {13.48, -0.34}, {11.52, -7.22}, {5.9, -5.62}}, false);
  }

  SECTION("Winds around twice")
  {
    test({{-0.9, -2.86},
          {-0.08, -6.14},
          {6.22, -7.14},
          {10.6, -0.28},
          {1.92, 0.46},
          {-1.9, -4.84},
          {3.22, -10.62},
          {11.14, -7.12},
          {5.24, 1.62}},
         false);
  }

  SECTION("All vertices on vertical line")
  {
    test({{3, 1}, {3, -3}, {3, 6}, {3, 4}}, false);
  }
}

} // namespace dida