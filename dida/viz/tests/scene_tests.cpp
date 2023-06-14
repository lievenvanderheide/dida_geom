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

  SECTION("Valid non-convex polygon")
  {
    std::vector<Point2> vertices{
        {-8.08, 2.56}, {-2.52, 4.78}, {-1.62, 0.80}, {3.74, 3.82}, {0.14, 5.02}, {-5.66, 8.92}, {-4.38, 6.72},
    };
    VizPolygon polygon("Valid polygon", vertices, false);
    CHECK(polygon.is_polygon_valid());
  }

  SECTION("Invalid non-convex polygon")
  {
    std::vector<Point2> vertices{{-3.28, 1.58}, {1.20, 5.26}, {2.86, 1.52}, {5.12, 5.70}, {5.66, 3.60}, {-4.76, 4.08}};
    VizPolygon polygon("Self intersecting polygon", vertices, false);
    CHECK_FALSE(polygon.is_polygon_valid());
  }
}

TEST_CASE("add_vertex")
{
  VizPolygon polygon("polygon", {}, true);

  size_t will_add_vertex_num_calls = 0;
  size_t vertex_added_num_calls = 0;
  size_t data_changed_num_calls = 0;

  QObject::connect(&polygon, &VizPolygon::will_add_vertex,
                   [&will_add_vertex_num_calls, &polygon](size_t index)
                   {
                     CHECK(polygon.vertices().size() == index);
                     CHECK(index == will_add_vertex_num_calls);

                     will_add_vertex_num_calls++;
                   });

  QObject::connect(&polygon, &VizPolygon::vertex_added,
                   [&vertex_added_num_calls, &polygon](size_t index)
                   {
                     CHECK(polygon.vertices().size() == index + 1);
                     CHECK(index == vertex_added_num_calls);

                     vertex_added_num_calls++;
                   });

  QObject::connect(&polygon, &VizPolygon::data_changed,
                   [&data_changed_num_calls, &polygon]()
                   {
                     data_changed_num_calls++;

                     CHECK(polygon.vertices().size() == data_changed_num_calls);
                   });

  Point2 vertices[]{{1.30, 0.82}, {5.58, 1.48}, {3.42, 2.76}};

  for (size_t i = 0; i < 3; i++)
  {
    polygon.add_vertex(vertices[i]);
  }

  CHECK(will_add_vertex_num_calls == 3);
  CHECK(vertex_added_num_calls == 3);
  CHECK(data_changed_num_calls == 3);
}

TEST_CASE("parse_viz_polygon")
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

  std::shared_ptr<VizPolygon> polygon = std::make_shared<VizPolygon>(
      "polygon", std::vector<Point2>{{-3.57, 2.24}, {-3.41, -1.12}, {-1.69, 1.6}, {-1.77, 5.38}}, true);

  bool will_add_primitive_called = false;
  bool primitive_added_called = false;
  bool data_changed_called = false;

  QObject::connect(&scene, &VizScene::will_add_primitive,
                   [&scene, &will_add_primitive_called]()
                   {
                     CHECK(!will_add_primitive_called);
                     will_add_primitive_called = true;

                     CHECK(scene.primitives().empty());
                   });

  QObject::connect(&scene, &VizScene::primitive_added,
                   [&scene, &polygon, &primitive_added_called]()
                   {
                     CHECK(!primitive_added_called);
                     primitive_added_called = true;

                     REQUIRE(scene.primitives().size() == 1);
                     CHECK(scene.primitives()[0] == polygon);
                   });

  QObject::connect(&scene, &VizScene::data_changed,
                   [&scene, &polygon, &data_changed_called]()
                   {
                     CHECK(!data_changed_called);
                     data_changed_called = true;

                     REQUIRE(scene.primitives().size() == 1);
                     CHECK(scene.primitives()[0] == polygon);
                   });

  scene.add_primitive(polygon);

  CHECK(will_add_primitive_called);
  CHECK(primitive_added_called);
  CHECK(data_changed_called);
}

} // namespace dida::viz