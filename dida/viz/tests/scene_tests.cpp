#include "dida/viz/scene.hpp"

#include <catch2/catch.hpp>

#include "dida/parser.hpp"

namespace dida::viz
{

/// Returns whether @c a and @c b are equal.
///
/// @param a The first operand of the comparison.
/// @param b The second operand of the comparison.
/// @return True iff @c a and @c b are equal.
bool vertices_equal(const std::vector<Point2>& a, const std::vector<Point2>& b)
{
  return std::equal(a.begin(), a.end(), b.begin(), b.end());
}

TEST_CASE("VizPolygon construction and access")
{
  std::vector<Point2> vertices{{-3.57, 0.92}, {-1.45, -2.2}, {-1.45, -2.2}, {5.83, 4.76}, {0.23, 3.38}};
  VizPolygon polygon("Polygon 1", vertices, true);

  CHECK(polygon.name() == "Polygon 1");
  CHECK(vertices_equal(polygon.vertices(), vertices));
  CHECK(polygon.should_be_convex() == true);
}

TEST_CASE("VizPolygon::is_polygon_valid()")
{
  SECTION("Valid convex polygon")
  {
    std::vector<Point2> vertices{{-2.55, -1.82}, {3.85, 1.8}, {-1.71, 2.84}, {-4.53, 1.44}};
    VizPolygon polygon("Convex polygon", vertices, true);
    CHECK(polygon.is_polygon_valid());
  }

  SECTION("Invalid convex polygon")
  {
    std::vector<Point2> vertices{{-2.55, -1.82}, {3.85, 1.8}, {-2.29, 0.24}, {-4.53, 1.44}};
    VizPolygon polygon("Not actually a convex polygon", vertices, true);
    CHECK_FALSE(polygon.is_polygon_valid());
  }
}

TEST_CASE("Parser VizPolygon2")
{
  SECTION("Valid convex polygon")
  {
    SECTION("No optional whitespace")
    {
      std::vector<Point2> expected_vertices{{-5.25, -1.44}, {0.57, -3.28}, {3.73, 0.1}, {2.85, 4.44}, {-3.83, 1.42}};

      std::shared_ptr<VizPolygon> polygon =
          parse_viz_polygon("ConvexPolygon2 foo{{-5.25,-1.44},{0.57,-3.28},{3.73,0.1},{2.85,4.44},{-3.83,1.42}};");
      REQUIRE(polygon);
      CHECK(polygon->name() == "foo");
      CHECK(vertices_equal(polygon->vertices(), expected_vertices));
      CHECK(polygon->should_be_convex());
    }

    SECTION("With comma after last vertex")
    {
      std::vector<Point2> expected_vertices{{-5.25, -1.44}, {0.57, -3.28}, {3.73, 0.1}, {2.85, 4.44}, {-3.83, 1.42}};

      std::shared_ptr<VizPolygon> polygon =
          parse_viz_polygon("ConvexPolygon2 foo{{-5.25,-1.44},{0.57,-3.28},{3.73,0.1},{2.85,4.44},{-3.83,1.42},};");
      REQUIRE(polygon);
      CHECK(polygon->name() == "foo");
      CHECK(vertices_equal(polygon->vertices(), expected_vertices));
      CHECK(polygon->should_be_convex());
    }

    SECTION("With optional whitespace")
    {
      std::vector<Point2> expected_vertices{{7, 5}, {3, 5}, {1, 2}, {2, 0}, {5, -2}};
      std::shared_ptr<VizPolygon> polygon =
          parse_viz_polygon("ConvexPolygon2 bar_2 { { 7 , 5 } , { 3 , 5} , { 1 , 2 } , { 2 , 0 } , { 5 , -2} } ;");
      REQUIRE(polygon);
      CHECK(polygon->name() == "bar_2");
      CHECK(vertices_equal(polygon->vertices(), expected_vertices));
      CHECK(polygon->should_be_convex());
    }
  }

  SECTION("Invalid type")
  {
    CHECK(!parse_viz_polygon("PollyGone foo {{2, 2}, {4, 5}, {1, 4}};"));
  }

  SECTION("No space after type")
  {
    CHECK(!parse_viz_polygon("ConvexPolygon2foo {{2, 2}, {4, 5}, {1, 4}};"));
  }

  SECTION("Name not a valid identifier")
  {
    CHECK(!parse_viz_polygon("ConvexPolygon2 :-D {{2, 2}, {4, 5}, {1, 4}};"));
  }

  SECTION("Vertex list not valid")
  {
    CHECK(!parse_viz_polygon("ConvexPolygon2 no_way {{cos(theta) * 10, sin(theta) * 5}};"));
  }

  SECTION("No ;")
  {
    CHECK(!parse_viz_polygon("ConvexPolygon2 foo {{2, 2}, {4, 5}, {1, 4}}"));
  }

  SECTION("End of polygon isn't end of string")
  {
    CHECK(!parse_viz_polygon("ConvexPolygon2 foo {{2, 2}, {4, 5}, {1, 4}}; :-P"));
  }

  SECTION("Parse with Parser")
  {
    std::vector<Point2> expected_vertices{{2, 2}, {4, 5}, {1, 4}};
    std::string_view text = "ConvexPolygon2 foo {{2, 2}, {4, 5}, {1, 4}};^!";

    Parser parser(text);
    std::shared_ptr<VizPolygon> polygon = parse_viz_polygon(parser);
    REQUIRE(polygon);
    CHECK(polygon->name() == "foo");
    CHECK(vertices_equal(polygon->vertices(), expected_vertices));
    CHECK(polygon->should_be_convex());

    CHECK(*parser.head() == '^');
  }
}

TEST_CASE("VizScene construction and access")
{
  SECTION("VizScene()")
  {
    VizScene scene;
    CHECK(scene.primitives().empty());
  }

  SECTION("VizScene(std::vector<std::shared_ptr<VizPolygon>>)")
  {
    std::shared_ptr<VizPolygon> polygon_0 = std::make_shared<VizPolygon>(
        "polygon_0", std::vector<Point2>{{-3.19, 1.84}, {-0.71, -1.82}, {3.71, 0.94}, {5.31, 4.56}}, true);

    std::shared_ptr<VizPolygon> polygon_1 = std::make_shared<VizPolygon>(
        "polygon_1",
        std::vector<Point2>{{-2.28, -1.66}, {-1.04, 0.86}, {3.1, -2.26}, {2.1, 2.58}, {1.38, 0.7}, {-1.46, 3.8}}, true);

    std::vector<std::shared_ptr<VizPolygon>> polygons{polygon_0, polygon_1};

    VizScene scene(polygons);
    REQUIRE(scene.primitives().size() == 2);
    CHECK(scene.primitives()[0] == polygon_0);
    CHECK(scene.primitives()[1] == polygon_1);
  }
}

TEST_CASE("VizScene::add_primitive")
{
  VizScene scene;

  bool data_changed_called = false;
  QObject::connect(&scene, &VizScene::data_changed,
                   [&data_changed_called]()
                   {
                     CHECK(!data_changed_called);
                     data_changed_called = true;
                   });

  std::shared_ptr<VizPolygon> polygon = std::make_shared<VizPolygon>(
      "polygon", std::vector<Point2>{{-3.57, 2.24}, {-3.41, -1.12}, {-1.69, 1.6}, {-1.77, 5.38}}, true);
  scene.add_primitive(polygon);

  CHECK(data_changed_called);
}

} // namespace dida::viz