#include "dida/polygon2.hpp"

#include <catch2/catch.hpp>

namespace dida
{

TEST_CASE("Polygon2T(Storage) and access")
{
  std::vector<Point2> vertices{{-2.72, 0.42}, {-5.2, -2.58}, {1.3, -3.76}, {3.78, 2.12}, {2.76, 2.92}};
  Polygon2 polygon(vertices);

  REQUIRE(polygon.size() == vertices.size());
  for (size_t i = 0; i < polygon.size(); i++)
  {
    CHECK(polygon[i] == vertices[i]);
  }

  CHECK(std::equal(polygon.begin(), polygon.end(), vertices.begin(), vertices.end()));
}

TEST_CASE("Polygon2T(std::initializer_list) and access")
{
  std::initializer_list<Point2> vertices{{-2.72, 0.42}, {-5.2, -2.58}, {1.3, -3.76}, {3.78, 2.12}, {2.76, 2.92}};
  Polygon2 polygon(vertices);

  REQUIRE(polygon.size() == vertices.size());
  for (size_t i = 0; i < polygon.size(); i++)
  {
    CHECK(polygon[i] == *(vertices.begin() + i));
  }

  CHECK(std::equal(polygon.begin(), polygon.end(), vertices.begin(), vertices.end()));
}

TEST_CASE("Polygon2T::unsafe_from_vertices and access")
{
  std::vector<Point2> vertices{{-2.72, 0.42}, {-5.2, -2.58}, {1.3, -3.76}, {3.78, 2.12}, {2.76, 2.92}};
  Polygon2 polygon = Polygon2::unsafe_from_vertices(vertices);

  REQUIRE(polygon.size() == vertices.size());
  for (size_t i = 0; i < polygon.size(); i++)
  {
    CHECK(polygon[i] == vertices[i]);
  }

  CHECK(std::equal(polygon.begin(), polygon.end(), vertices.begin(), vertices.end()));
}

TEST_CASE("Polygon2T::operator PolygonView2")
{
  std::vector<Point2> vertices{{-2.72, 0.42}, {-5.2, -2.58}, {1.3, -3.76}, {3.78, 2.12}, {2.76, 2.92}};

  Polygon2 polygon(vertices);
  PolygonView2 view = polygon;

  CHECK(std::equal(view.begin(), view.end(), polygon.begin(), polygon.end()));
}

TEST_CASE("Polygon2T::unsafe_mutable_vertices")
{
  Polygon2 polygon{{-2.72, 0.42}, {-5.2, -2.58}, {1.3, -3.76}, {3.78, 2.12}, {2.76, 2.92}};

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

TEST_CASE("PolygonView2(ArrayView<const Point2>) and access")
{
  std::vector<Point2> vertices{{12.18, -1.16}, {2.84, 0.9}, {-1.94, -0.32}, {4.56, -3.18}};
  PolygonView2 view(vertices);

  REQUIRE(view.size() == vertices.size());
  for(size_t i = 0; i < view.size(); i++)
  {
    CHECK(view[i] == vertices[i]);
  }

  CHECK(std::equal(view.begin(), view.end(), vertices.begin(), vertices.end()));
}

TEST_CASE("PolygonView2::unsafe_from_vertices and access")
{
  std::vector<Point2> vertices{{12.18, -1.16}, {2.84, 0.9}, {-1.94, -0.32}, {4.56, -3.18}};
  PolygonView2 view = PolygonView2::unsafe_from_vertices(vertices);

  REQUIRE(view.size() == vertices.size());
  for(size_t i = 0; i < view.size(); i++)
  {
    CHECK(view[i] == vertices[i]);
  }

  CHECK(std::equal(view.begin(), view.end(), vertices.begin(), vertices.end()));
}

} // namespace dida