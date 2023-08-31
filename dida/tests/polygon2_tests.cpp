#include "dida/polygon2.hpp"

#include <catch2/catch.hpp>
#include <sstream>

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
  Polygon2 polygon{{-2.72, 0.42}, {-5.2, -2.58}, {1.3, -3.76}, {3.78, 2.12}, {2.76, 2.92}};

  PolygonView2 view = polygon;
  CHECK(std::equal(view.begin(), view.end(), polygon.begin(), polygon.end()));
}

TEST_CASE("Polygon2::operator ArrayView<const Point2>")
{
  Polygon2 polygon{
      {-5.16, 0.44}, {-3.24, 2.74}, {-1.72, 1.26}, {-3.08, 0.00}, {-0.06, 0.26},
      {-0.26, 2.54}, {-1.78, 2.32}, {1.44, 4.14},  {-5.28, 4.32}, {-4.16, 3.14},
  };

  ArrayView<const Point2> view = polygon;
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
  for (size_t i = 0; i < view.size(); i++)
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
  for (size_t i = 0; i < view.size(); i++)
  {
    CHECK(view[i] == vertices[i]);
  }

  CHECK(std::equal(view.begin(), view.end(), vertices.begin(), vertices.end()));
}

TEST_CASE("PolygonView2::operator ArrayView<const Point2>")
{
  std::vector<Point2> vertices{{-2.24, 1.94}, {0.08, 4.58}, {3.60, 1.92}, {3.50, 8.72},
                               {-1.56, 7.60}, {1.84, 6.92}, {-3.10, 4.92}};
  PolygonView2 polygon_view(vertices);
  ArrayView<const Point2> array_view(polygon_view);

  CHECK(polygon_view.size() == array_view.size());
  CHECK(std::equal(polygon_view.begin(), polygon_view.end(), array_view.begin(), array_view.end()));
}

TEST_CASE("validate_polygon_vertices")
{
  SECTION("Valid general")
  {
    std::vector<Point2> vertices{
        {-2.12, 1.82}, {2.62, 3.38},  {5.10, -0.42}, {2.90, -1.34}, {0.22, 0.68},  {2.24, 1.88},
        {3.34, 0.32},  {2.50, 2.64},  {-0.86, 0.72}, {2.64, -2.50}, {6.88, -0.50}, {3.44, 4.98},
        {6.44, 5.52},  {3.70, 6.26},  {6.78, 7.52},  {7.90, 5.48},  {5.06, 4.38},  {8.12, 4.56},
        {7.52, 8.50},  {1.62, 7.56},  {1.34, 4.94},  {-4.54, 8.58}, {-2.28, 5.90}, {-6.16, 5.98},
        {-1.10, 4.96}, {-2.14, 6.56}, {0.12, 4.88},  {-4.22, 3.80}, {-1.06, 3.78},
    };

    CHECK(validate_polygon_vertices(vertices));
  }

  SECTION("Fewer than 3 vertices")
  {
    std::vector<Point2> vertices{{-0.32, 5.44}, {5.00, 2.10}};
    CHECK(!validate_polygon_vertices(vertices));
  }

  SECTION("Duplicated vertices")
  {
    std::vector<Point2> vertices{
        {-4.34, 3.66}, {1.94, 2.16}, {5.62, 5.72}, {5.62, 5.72},
        {7.26, 4.36},  {5.68, 8.50}, {2.42, 4.38}, {-1.02, 6.62},
    };
    CHECK(!validate_polygon_vertices(vertices));
  }

  SECTION("Duplicated first and last vertices")
  {
    std::vector<Point2> vertices{
        {-4.34, 3.66}, {1.94, 2.16}, {5.62, 5.72},  {7.26, 4.36},
        {5.68, 8.50},  {2.42, 4.38}, {-1.02, 6.62}, {-4.34, 3.66},
    };
    CHECK(!validate_polygon_vertices(vertices));
  }

  SECTION("Incorrect winding")
  {
    std::vector<Point2> vertices{
        {-2.08, 2.52}, {-3.64, 1.56}, {-6.26, 2.94}, {-3.28, 6.50},  {-4.62, 3.10},
        {1.18, 6.62},  {-2.52, 0.26}, {1.82, 1.86},  {-4.62, -0.82},
    };

    CHECK(!validate_polygon_vertices(vertices));
  }

  SECTION("Self intersecting, appear event on edge")
  {
    std::vector<Point2> vertices{{2, 2}, {6, 0}, {7, 2}, {4, 1}, {7, 4}};
    CHECK(!validate_polygon_vertices(vertices));
  }

  SECTION("Self intersecting, edge crosses with lower neighbor on transition")
  {
    std::vector<Point2> vertices{
        {-5.78, 3.08}, {-3.20, 0.68}, {2.52, 1.88}, {4.00, 6.78}, {-1.12, -0.20}, {3.82, 8.04},
    };
    CHECK(!validate_polygon_vertices(vertices));
  }

  SECTION("Self intersecting, transition vertex on lower neighbor")
  {
    std::vector<Point2> vertices{{1, 2}, {8, 2}, {7, 4}, {4, 2}, {2, 4}};
    CHECK(!validate_polygon_vertices(vertices));
  }

  SECTION("Self intersecting, edge crosses with upper neighbor on transition")
  {
    std::vector<Point2> vertices{
        {-5.82, 2.74}, {6.84, 1.40}, {6.20, 8.02}, {-0.70, 8.86}, {7.02, 9.60}, {3.64, 8.64},
    };
    CHECK(!validate_polygon_vertices(vertices));
  }

  SECTION("Self intersecting, transition vertex on upper neighbor")
  {
    std::vector<Point2> vertices{{-4, 2}, {-2, 1}, {-1, 2}, {1, 1}, {2, 2}};
    CHECK(!validate_polygon_vertices(vertices));
  }

  SECTION("Self intersecting, vanishing edges not adjacent in active segments list")
  {
    std::vector<Point2> vertices{
        {-5.86, 3.02}, {-1.46, 0.72}, {5.54, 2.48}, {0.92, 4.90},
        {6.42, 7.58},  {0.42, 2.90},  {7.98, 1.10}, {7.20, 8.88},
    };
    CHECK(!validate_polygon_vertices(vertices));
  }

  SECTION("Self intersecting, edge crosses with lower neighbor on vanish")
  {
    std::vector<Point2> vertices{
        {-4.96, 4.16}, {0.74, 2.36},  {5.06, -0.58}, {0.92, 3.90}, {7.24, 2.12},
        {1.58, -0.98}, {6.86, -1.18}, {8.10, 2.72},  {0.62, 6.22},
    };
    CHECK(!validate_polygon_vertices(vertices));
  }

  SECTION("Self intersecting, vanish vertex on lower neigbor")
  {
    std::vector<Point2> vertices{{1, 1}, {6, 1}, {4, 3}, {8, 3}, {4, -1}, {9, 3}, {8, 4}, {3, 4}};
    CHECK(!validate_polygon_vertices(vertices));
  }

  SECTION("Self intersecting, edge crosses with upper neighbor on vanish")
  {
    std::vector<Point2> vertices{
        {-6.96, 3.46}, {-1.08, 1.32}, {6.74, 4.08}, {0.02, 8.80}, {2.56, 5.00}, {-1.14, 3.48}, {1.48, 7.22},
    };
    CHECK(!validate_polygon_vertices(vertices));
  }

  SECTION("Self intersecting, vanish vertex on upper neigbor")
  {
    std::vector<Point2> vertices{{5, 2}, {12, 2}, {7, 7}, {4, 4}, {9, 5}};
    CHECK(!validate_polygon_vertices(vertices));
  }
}

TEST_CASE("PolygonView2 printing")
{
  Polygon2 polygon{{-3, 1}, {0, 1}, {0, 4}, {3, 7}, {-3, 8}};

  std::stringstream s;
  s << polygon;
  CHECK(s.str() == "{{-3, 1}, {0, 1}, {0, 4}, {3, 7}, {-3, 8}}");
}

} // namespace dida