#include "dida/detail/vertical_decomposition/tests/vertical_extension_validation.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_all.hpp>

#include "dida/detail/vertical_decomposition/sweep_line_builder.hpp"
#include "dida/detail/vertical_decomposition/tests/test_utils.hpp"
#include "dida/polygon2.hpp"

namespace dida::detail::vertical_decomposition
{

namespace
{

void flip_horizontally(ArrayView<Point2> vertices, ArrayView<Node> nodes)
{
  for (Point2& v : vertices)
  {
    v = Point2(-v.x(), v.y());
  }

  for (Node& node : nodes)
  {
    node.direction = other_direction(node.direction);
  }
}

} // namespace

TEST_CASE("PolygonLocationLessThan")
{
  Polygon2 polygon{{2.44, 4.02}, {5.94, 6.58}, {2.58, 7.52}, {-1.32, 5.42}};

  SECTION("Different edges")
  {
    PolygonLocation a{1, ScalarDeg1(4.2)};
    PolygonLocation b{2, ScalarDeg1(-.12)};

    PolygonLocationLessThan less_than{polygon};
    CHECK(less_than(a, b));
    CHECK_FALSE(less_than(b, a));
  }

  SECTION("On same edge, edge towards right")
  {
    PolygonLocation a{3, ScalarDeg1(-0.34)};
    PolygonLocation b{3, ScalarDeg1(1.36)};

    PolygonLocationLessThan less_than{polygon};
    CHECK(less_than(a, b));
    CHECK_FALSE(less_than(b, a));
    CHECK_FALSE(less_than(a, a));
  }

  SECTION("On same edge, edge towards left")
  {
    PolygonLocation a{1, ScalarDeg1(4.92)};
    PolygonLocation b{1, ScalarDeg1(2.96)};

    PolygonLocationLessThan less_than{polygon};
    CHECK(less_than(a, b));
    CHECK_FALSE(less_than(b, a));
    CHECK_FALSE(less_than(a, a));
  }
}

TEST_CASE("PolygonRange::split")
{
  Polygon2 polygon{{-4.48, 2.08}, {-2.64, 4.16}, {0.32, 2.40}, {2.98, 4.26}, {-7.36, 7.58}};
  VerticesView vertices(polygon);

  PolygonRange range_without_wrapping{1, 4, ScalarDeg1(-.92), ScalarDeg1(-6.82)};
  PolygonRange range_with_wrapping{4, 5, ScalarDeg1(-6.82), ScalarDeg1(-4.52)};

  SECTION("Split at vertex")
  {
    std::pair<PolygonRange, PolygonRange> result =
        range_without_wrapping.split(vertices, PolygonLocation{3, ScalarDeg1(2.98)});

    CHECK(result.first.first_edge_index == 1);
    CHECK(result.first.num_edges == 2);
    CHECK(result.first.start_point_x == ScalarDeg1(-.92));
    CHECK(result.first.end_point_x == ScalarDeg1(2.98));

    CHECK(result.second.first_edge_index == 3);
    CHECK(result.second.num_edges == 2);
    CHECK(result.second.start_point_x == ScalarDeg1(2.98));
    CHECK(result.second.end_point_x == ScalarDeg1(-6.82));
  }

  SECTION("Split at vertex with wrapping")
  {
    std::pair<PolygonRange, PolygonRange> result =
        range_with_wrapping.split(vertices, PolygonLocation{1, ScalarDeg1(-2.64)});

    CHECK(result.first.first_edge_index == 4);
    CHECK(result.first.num_edges == 2);
    CHECK(result.first.start_point_x == ScalarDeg1(-6.82));
    CHECK(result.first.end_point_x == ScalarDeg1(-2.64));

    CHECK(result.second.first_edge_index == 1);
    CHECK(result.second.num_edges == 3);
    CHECK(result.second.start_point_x == ScalarDeg1(-2.64));
    CHECK(result.second.end_point_x == ScalarDeg1(-4.52));
  }

  SECTION("Split mid edge")
  {
    std::pair<PolygonRange, PolygonRange> result =
        range_without_wrapping.split(vertices, PolygonLocation{3, ScalarDeg1(-4.52)});

    CHECK(result.first.first_edge_index == 1);
    CHECK(result.first.num_edges == 3);
    CHECK(result.first.start_point_x == ScalarDeg1(-.92));
    CHECK(result.first.end_point_x == ScalarDeg1(-4.52));

    CHECK(result.second.first_edge_index == 3);
    CHECK(result.second.num_edges == 2);
    CHECK(result.second.start_point_x == ScalarDeg1(-4.52));
    CHECK(result.second.end_point_x == ScalarDeg1(-6.82));
  }

  SECTION("Split mid edge, with wrapping")
  {
    std::pair<PolygonRange, PolygonRange> result =
        range_with_wrapping.split(vertices, PolygonLocation{1, ScalarDeg1(-0.92)});

    CHECK(result.first.first_edge_index == 4);
    CHECK(result.first.num_edges == 3);
    CHECK(result.first.start_point_x == ScalarDeg1(-6.82));
    CHECK(result.first.end_point_x == ScalarDeg1(-0.92));

    CHECK(result.second.first_edge_index == 1);
    CHECK(result.second.num_edges == 3);
    CHECK(result.second.start_point_x == ScalarDeg1(-0.92));
    CHECK(result.second.end_point_x == ScalarDeg1(-4.52));
  }
}

TEST_CASE("ray_cast_up")
{
  Polygon2 polygon{
      {1.98, -2.24}, {7.44, 0.74}, {2.38, 2.48}, {5.06, 0.68}, {2.26, -0.92}, {0.26, 3.38}, {4.98, 4.52}, {-1.16, 4.02},
  };

  ArrayView<const Point2> vertices(polygon);

  SECTION("Full polygon, hits edge from inside")
  {
    for (size_t i = 0; i < vertices.size(); i++)
    {
      Edge edge = ray_cast_up(vertices, Winding::ccw,
                              PolygonRange{i, vertices.size(), vertices[i].x(), vertices[i].x()}, {4.06, -0.64});
      REQUIRE(edge.is_valid());
      CHECK(*edge.start_vertex_it == Point2(5.06, 0.68));
      CHECK(*edge.end_vertex_it == Point2(2.26, -0.92));
    }
  }

  SECTION("Full polygon, hits edge from outside")
  {
    for (size_t i = 0; i < vertices.size(); i++)
    {
      Edge edge = ray_cast_up(vertices, Winding::ccw,
                              PolygonRange{i, vertices.size(), vertices[i].x(), vertices[i].x()}, {3.26, 1.16});
      CHECK(!edge.is_valid());
    }
  }

  SECTION("Full polygon, no hit")
  {
    for (size_t i = 0; i < vertices.size(); i++)
    {
      Edge edge = ray_cast_up(vertices, Winding::ccw,
                              PolygonRange{i, vertices.size(), vertices[i].x(), vertices[i].x()}, {6.36, 3.32});
      CHECK(!edge.is_valid());
    }
  }

  SECTION("Hits vertex -> return left side edge")
  {
    for (size_t i = 0; i < vertices.size(); i++)
    {
      Edge edge = ray_cast_up(vertices, Winding::ccw,
                              PolygonRange{i, vertices.size(), vertices[i].x(), vertices[i].x()}, {2.26, -1.52});
      REQUIRE(edge.is_valid());
      CHECK(*edge.start_vertex_it == Point2(2.26, -0.92));
      CHECK(*edge.end_vertex_it == Point2(0.26, 3.38));
    }
  }

  SECTION("ray_origin on edge -> ignore edge")
  {
    for (size_t i = 0; i < vertices.size(); i++)
    {
      Edge edge = ray_cast_up(vertices, Winding::ccw,
                              PolygonRange{i, vertices.size(), vertices[i].x(), vertices[i].x()}, {2.26, -0.92});
      CHECK(!edge.is_valid());
    }
  }

  SECTION("Open range, hits edge from inside")
  {
    Edge edge = ray_cast_up(vertices, Winding::ccw, PolygonRange{1, 3, vertices[1].x(), vertices[4].x()}, {4.93, 1.26});
    REQUIRE(edge.is_valid());
    CHECK(*edge.start_vertex_it == Point2(7.44, 0.74));
    CHECK(*edge.end_vertex_it == Point2(2.38, 2.48));
  }

  SECTION("Open range, ignore closing edge")
  {
    Edge edge =
        ray_cast_up(vertices, Winding::ccw, PolygonRange{0, 3, vertices[0].x(), vertices[3].x()}, {4.45, -0.62});
    REQUIRE(!edge.is_valid());
  }

  SECTION("Hits partial first edge")
  {
    Edge edge = ray_cast_up(vertices, Winding::ccw, PolygonRange{3, 3, ScalarDeg1(4), vertices[6].x()}, {3.27, -0.82});
    REQUIRE(edge.is_valid());
    CHECK(*edge.start_vertex_it == Point2(5.06, 0.68));
    CHECK(*edge.end_vertex_it == Point2(2.26, -0.92));
  }

  SECTION("Misses partial first edge")
  {
    Edge edge = ray_cast_up(vertices, Winding::ccw, PolygonRange{3, 3, ScalarDeg1(3), vertices[6].x()}, {3.27, -0.82});
    CHECK(!edge.is_valid());
  }

  SECTION("Hits partial last edge")
  {
    Edge edge = ray_cast_up(vertices, Winding::ccw, PolygonRange{0, 4, vertices[0].x(), ScalarDeg1(3)}, {3.27, -0.82});
    REQUIRE(edge.is_valid());
    CHECK(*edge.start_vertex_it == Point2(5.06, 0.68));
    CHECK(*edge.end_vertex_it == Point2(2.26, -0.92));
  }

  SECTION("Misses partial last edge")
  {
    Edge edge = ray_cast_up(vertices, Winding::ccw, PolygonRange{0, 4, vertices[0].x(), ScalarDeg1(4)}, {3.27, -0.82});
    CHECK(!edge.is_valid());
  }

  SECTION("Open range, with wrap")
  {
    Edge edge =
        ray_cast_up(vertices, Winding::ccw, PolygonRange{6, 6, vertices[6].x(), vertices[4].x()}, {3.48, -0.40});
    REQUIRE(edge.is_valid());
    CHECK(*edge.start_vertex_it == Point2(5.06, 0.68));
    CHECK(*edge.end_vertex_it == Point2(2.26, -0.92));
  }

  SECTION("Clockwise winding")
  {
    std::vector<Point2> vertices_storage{
        {-2.88, 2.24}, {-6.10, 3.46}, {-1.32, 4.30}, {-4.96, 6.12}, {-3.60, 7.16},
        {-0.62, 6.90}, {1.52, 3.80},  {-3.82, 3.44}, {-2.44, 2.76}, {-2.36, 2.36},
    };
    VerticesView vertices(vertices_storage);
    PolygonRange full_range{0, vertices.size(), vertices[0].x(), vertices[0].x()};

    SECTION("Hit")
    {
      Edge edge = ray_cast_up(vertices, Winding::cw, full_range, {-2.74, 3.76});
      REQUIRE(edge.is_valid());
      CHECK(*edge.start_vertex_it == Point2(-6.10, 3.46));
      CHECK(*edge.end_vertex_it == Point2(-1.32, 4.30));
    }

    SECTION("No hit")
    {
      Edge edge = ray_cast_up(vertices, Winding::cw, full_range, {-3.14, 3.30});
      CHECK(!edge.is_valid());
    }
  }
}

TEST_CASE("ray_cast_down")
{
  Polygon2 polygon{
      {-2.41, 3.78}, {-4.93, 1.68}, {1.45, -0.08}, {-2.47, -0.96},
      {1.81, -2.16}, {5.21, -1.82}, {2.65, 2.3},   {-3.69, 1.76},
  };

  ArrayView<const Point2> vertices(polygon);

  SECTION("Full polygon, hits edge from inside")
  {
    for (size_t i = 0; i < vertices.size(); i++)
    {
      Edge edge = ray_cast_down(vertices, Winding::ccw,
                                PolygonRange{i, vertices.size(), vertices[i].x(), vertices[i].x()}, {0.47, 1.12});
      REQUIRE(edge.is_valid());
      CHECK(*edge.start_vertex_it == Point2(-4.93, 1.68));
      CHECK(*edge.end_vertex_it == Point2(1.45, -0.08));
    }
  }

  SECTION("Full polygon, hits edge from outside")
  {
    for (size_t i = 0; i < vertices.size(); i++)
    {
      Edge edge = ray_cast_down(vertices, Winding::ccw,
                                PolygonRange{i, vertices.size(), vertices[i].x(), vertices[i].x()}, {-1.29, 2.38});
      CHECK(!edge.is_valid());
    }
  }

  SECTION("Full polygon, no hit")
  {
    for (size_t i = 0; i < vertices.size(); i++)
    {
      Edge edge = ray_cast_down(vertices, Winding::ccw,
                                PolygonRange{i, vertices.size(), vertices[i].x(), vertices[i].x()}, {-3.63, -0.68});
      CHECK(!edge.is_valid());
    }
  }

  SECTION("Hits vertex -> return right side edge")
  {
    for (size_t i = 0; i < vertices.size(); i++)
    {
      Edge edge = ray_cast_down(vertices, Winding::ccw,
                                PolygonRange{i, vertices.size(), vertices[i].x(), vertices[i].x()}, {1.81, -0.98});
      REQUIRE(edge.is_valid());
      CHECK(*edge.start_vertex_it == Point2(1.81, -2.16));
      CHECK(*edge.end_vertex_it == Point2(5.21, -1.82));
    }
  }

  SECTION("ray_origin on edge -> ignore edge")
  {
    for (size_t i = 0; i < vertices.size(); i++)
    {
      Edge edge = ray_cast_down(vertices, Winding::ccw,
                                PolygonRange{i, vertices.size(), vertices[i].x(), vertices[i].x()}, {2.65, 2.3});
      REQUIRE(edge.is_valid());
      CHECK(*edge.start_vertex_it == Point2(1.81, -2.16));
      CHECK(*edge.end_vertex_it == Point2(5.21, -1.82));
    }
  }

  SECTION("Open range, hits edge from inside")
  {
    Edge edge =
        ray_cast_down(vertices, Winding::ccw, PolygonRange{2, 4, vertices[2].x(), vertices[6].x()}, {0.42, -1.22});
    REQUIRE(edge.is_valid());
    CHECK(*edge.start_vertex_it == Point2(-2.47, -0.96));
    CHECK(*edge.end_vertex_it == Point2(1.81, -2.16));
  }

  SECTION("Open range, ignore closing edge")
  {
    Edge edge =
        ray_cast_down(vertices, Winding::ccw, PolygonRange{2, 5, vertices[2].x(), vertices[7].x()}, {-0.55, 1.58});
    CHECK(!edge.is_valid());
  }

  SECTION("Hits partial first edge")
  {
    Edge edge = ray_cast_down(vertices, Winding::ccw, PolygonRange{1, 3, ScalarDeg1(-4), ScalarDeg1(1)}, {-1.36, 1.41});
    REQUIRE(edge.is_valid());
    CHECK(*edge.start_vertex_it == Point2(-4.93, 1.68));
    CHECK(*edge.end_vertex_it == Point2(1.45, -0.08));
  }

  SECTION("Misses partial first edge")
  {
    Edge edge =
        ray_cast_down(vertices, Winding::ccw, PolygonRange{2, 4, ScalarDeg1(-2), ScalarDeg1(3.5)}, {-1.20, -0.24});
    REQUIRE(edge.is_valid());
    CHECK(*edge.start_vertex_it == Point2(-2.47, -0.96));
    CHECK(*edge.end_vertex_it == Point2(1.81, -2.16));
  }

  SECTION("Hits partial last edge")
  {
    Edge edge =
        ray_cast_down(vertices, Winding::ccw, PolygonRange{1, 3, ScalarDeg1(-4), ScalarDeg1(1)}, {-0.88, -1.08});
    REQUIRE(edge.is_valid());
    CHECK(*edge.start_vertex_it == Point2(-2.47, -0.96));
    CHECK(*edge.end_vertex_it == Point2(1.81, -2.16));
  }

  SECTION("Misses partial last edge")
  {
    Edge edge =
        ray_cast_down(vertices, Winding::ccw, PolygonRange{1, 6, ScalarDeg1(-2.5), ScalarDeg1(1.8)}, {0.36, 3.30});
    REQUIRE(edge.is_valid());
    CHECK(*edge.start_vertex_it == Point2(-4.93, 1.68));
    CHECK(*edge.end_vertex_it == Point2(1.45, -0.08));
  }

  SECTION("Open range, with wrap")
  {
    Edge edge =
        ray_cast_down(vertices, Winding::ccw, PolygonRange{6, 4, vertices[6].x(), vertices[2].x()}, {-4.26, 1.94});
    REQUIRE(edge.is_valid());
    CHECK(*edge.start_vertex_it == Point2(-4.93, 1.68));
    CHECK(*edge.end_vertex_it == Point2(1.45, -0.08));
  }

  SECTION("Clockwise winding")
  {
    std::vector<Point2> vertices_storage{
        {-2.88, 2.24}, {-6.10, 3.46}, {-1.32, 4.30}, {-4.96, 6.12}, {-3.60, 7.16},
        {-0.62, 6.90}, {1.52, 3.80},  {-3.82, 3.44}, {-2.44, 2.76}, {-2.36, 2.36},
    };
    VerticesView vertices(vertices_storage);
    PolygonRange full_range{0, vertices.size(), vertices[0].x(), vertices[0].x()};

    SECTION("Hit")
    {
      Edge edge = ray_cast_down(vertices, Winding::cw, full_range, {-2.74, 3.76});
      REQUIRE(edge.is_valid());
      CHECK(*edge.start_vertex_it == Point2(1.52, 3.80));
      CHECK(*edge.end_vertex_it == Point2(-3.82, 3.44));
    }

    SECTION("No hit")
    {
      Edge edge = ray_cast_down(vertices, Winding::cw, full_range, {-3.14, 3.30});
      CHECK(!edge.is_valid());
    }
  }
}

TEST_CASE("VerticalExtensionContactPoint::operator==")
{
  Node node1, node2;
  VerticalExtensionContactPoint a{VerticalExtensionContactPoint::Type::vertex_downwards, &node1};
  VerticalExtensionContactPoint b{VerticalExtensionContactPoint::Type::vertex_upwards, &node1};
  VerticalExtensionContactPoint c{VerticalExtensionContactPoint::Type::vertex_downwards, &node2};
  CHECK(a == a);
  CHECK_FALSE(a == b);
  CHECK_FALSE(a == c);
}

TEST_CASE("VerticalExtensionContactPoint::operator<")
{
  Node nodes[3];
  VerticalExtensionContactPoint a{VerticalExtensionContactPoint::Type::vertex_upwards, &nodes[1]};
  VerticalExtensionContactPoint b{VerticalExtensionContactPoint::Type::vertex_downwards, &nodes[2]};
  VerticalExtensionContactPoint c{VerticalExtensionContactPoint::Type::leaf, &nodes[0]};
  VerticalExtensionContactPoint d{VerticalExtensionContactPoint::Type::vertex_upwards, &nodes[0]};
  VerticalExtensionContactPoint e{VerticalExtensionContactPoint::Type::vertex_upwards, &nodes[2]};
  CHECK_FALSE(a < a);
  CHECK_FALSE(a < b);
  CHECK(a < c);
  CHECK_FALSE(a < d);
  CHECK(a < e);
}

TEST_CASE("contact_point_type_to_string")
{
  CHECK(contact_point_type_to_string(VerticalExtensionContactPoint::Type::vertex_downwards) == "vertex_downwards");
  CHECK(contact_point_type_to_string(VerticalExtensionContactPoint::Type::vertex_upwards) == "vertex_upwards");
  CHECK(contact_point_type_to_string(VerticalExtensionContactPoint::Type::lower_opp_edge) == "lower_opp_edge");
  CHECK(contact_point_type_to_string(VerticalExtensionContactPoint::Type::upper_opp_edge) == "upper_opp_edge");
  CHECK(contact_point_type_to_string(VerticalExtensionContactPoint::Type::leaf) == "leaf");
}

TEST_CASE("vertical_extension_contact_points")
{
  SECTION("Chain 1")
  {
    std::vector<Point2> vertices_storage{
        {-3.00, 2.64}, {-0.30, 3.64}, {-3.12, 5.62}, {-5.92, 3.10}, {-2.76, 0.64}, {0.78, 2.34}, {-0.02, 0.62},
        {2.88, 0.70},  {2.06, 2.08},  {5.34, 1.70},  {6.74, 3.64},  {4.40, 6.00},  {3.20, 4.00}, {4.68, 3.20},
    };
    VerticesView vertices(vertices_storage);

    std::vector<Node> nodes(10);
    nodes[0].direction = HorizontalDirection::left;
    nodes[0].type = NodeType::leaf;
    nodes[0].vertex_it = vertices.begin() + 3;
    nodes[0].lower_opp_edge = Edge::edge_from_index(vertices, 3);
    nodes[0].upper_opp_edge = Edge::edge_from_index(vertices, 2);
    nodes[0].neighbors[0] = &nodes[1];

    nodes[1].direction = HorizontalDirection::right;
    nodes[1].type = NodeType::branch;
    nodes[1].vertex_it = vertices.begin() + 0;
    nodes[1].lower_opp_edge = Edge::edge_from_index(vertices, 3);
    nodes[1].upper_opp_edge = Edge::edge_from_index(vertices, 1);
    nodes[1].neighbors[0] = &nodes[0];
    nodes[1].neighbors[1] = &nodes[4];
    nodes[1].neighbors[2] = &nodes[2];

    nodes[2].direction = HorizontalDirection::right;
    nodes[2].type = NodeType::leaf;
    nodes[2].vertex_it = vertices.begin() + 1;
    nodes[2].lower_opp_edge = Edge::edge_from_index(vertices, 0);
    nodes[2].upper_opp_edge = Edge::edge_from_index(vertices, 1);
    nodes[2].neighbors[0] = &nodes[1];

    nodes[3].direction = HorizontalDirection::left;
    nodes[3].type = NodeType::leaf;
    nodes[3].vertex_it = vertices.begin() + 6;
    nodes[3].lower_opp_edge = Edge::edge_from_index(vertices, 6);
    nodes[3].upper_opp_edge = Edge::edge_from_index(vertices, 5);
    nodes[3].neighbors[0] = &nodes[4];

    nodes[4].direction = HorizontalDirection::left;
    nodes[4].type = NodeType::branch;
    nodes[4].vertex_it = vertices.begin() + 5;
    nodes[4].lower_opp_edge = Edge::edge_from_index(vertices, 6);
    nodes[4].upper_opp_edge = Edge::invalid();
    nodes[4].neighbors[0] = &nodes[5];
    nodes[4].neighbors[1] = &nodes[3];
    nodes[4].neighbors[2] = &nodes[1];

    nodes[5].direction = HorizontalDirection::right;
    nodes[5].type = NodeType::branch;
    nodes[5].vertex_it = vertices.begin() + 8;
    nodes[5].lower_opp_edge = Edge::edge_from_index(vertices, 6);
    nodes[5].upper_opp_edge = Edge::invalid();
    nodes[5].neighbors[0] = &nodes[4];
    nodes[5].neighbors[1] = &nodes[6];
    nodes[5].neighbors[2] = &nodes[8];

    nodes[6].direction = HorizontalDirection::right;
    nodes[6].type = NodeType::leaf;
    nodes[6].vertex_it = vertices.begin() + 7;
    nodes[6].lower_opp_edge = Edge::edge_from_index(vertices, 6);
    nodes[6].upper_opp_edge = Edge::edge_from_index(vertices, 7);
    nodes[6].neighbors[0] = &nodes[5];

    nodes[7].direction = HorizontalDirection::left;
    nodes[7].type = NodeType::leaf;
    nodes[7].vertex_it = vertices.begin() + 12;
    nodes[7].lower_opp_edge = Edge::edge_from_index(vertices, 12);
    nodes[7].upper_opp_edge = Edge::edge_from_index(vertices, 11);
    nodes[7].neighbors[0] = &nodes[8];

    nodes[8].direction = HorizontalDirection::left;
    nodes[8].type = NodeType::branch;
    nodes[8].vertex_it = vertices.begin() + 13;
    nodes[8].lower_opp_edge = Edge::edge_from_index(vertices, 8);
    nodes[8].upper_opp_edge = Edge::edge_from_index(vertices, 10);
    nodes[8].neighbors[0] = &nodes[9];
    nodes[8].neighbors[1] = &nodes[5];
    nodes[8].neighbors[2] = &nodes[7];

    nodes[9].direction = HorizontalDirection::right;
    nodes[9].type = NodeType::leaf;
    nodes[9].vertex_it = vertices.begin() + 10;
    nodes[9].lower_opp_edge = Edge::edge_from_index(vertices, 9);
    nodes[9].upper_opp_edge = Edge::edge_from_index(vertices, 10);
    nodes[9].neighbors[0] = &nodes[8];

    Winding winding = GENERATE(Winding::ccw, Winding::cw);
    if (winding == Winding::cw)
    {
      flip_horizontally(vertices_storage, nodes);
    }

    ChainDecomposition chain_decomposition{&nodes[1], &nodes[8]};

    std::vector<VerticalExtensionContactPoint> contact_points =
        vertical_extension_contact_points(chain_decomposition, winding);

    REQUIRE(contact_points.size() == 18);

    CHECK(contact_points[0].type == VerticalExtensionContactPoint::Type::vertex_upwards);
    CHECK(contact_points[0].node == &nodes[1]);

    CHECK(contact_points[1].type == VerticalExtensionContactPoint::Type::leaf);
    CHECK(contact_points[1].node == &nodes[2]);

    CHECK(contact_points[2].type == VerticalExtensionContactPoint::Type::upper_opp_edge);
    CHECK(contact_points[2].node == &nodes[1]);

    CHECK(contact_points[3].type == VerticalExtensionContactPoint::Type::leaf);
    CHECK(contact_points[3].node == &nodes[0]);

    CHECK(contact_points[4].type == VerticalExtensionContactPoint::Type::lower_opp_edge);
    CHECK(contact_points[4].node == &nodes[1]);

    CHECK(contact_points[5].type == VerticalExtensionContactPoint::Type::vertex_upwards);
    CHECK(contact_points[5].node == &nodes[4]);

    CHECK(contact_points[6].type == VerticalExtensionContactPoint::Type::vertex_downwards);
    CHECK(contact_points[6].node == &nodes[4]);

    CHECK(contact_points[7].type == VerticalExtensionContactPoint::Type::leaf);
    CHECK(contact_points[7].node == &nodes[3]);

    CHECK(contact_points[8].type == VerticalExtensionContactPoint::Type::lower_opp_edge);
    CHECK(contact_points[8].node == &nodes[4]);

    CHECK(contact_points[9].type == VerticalExtensionContactPoint::Type::lower_opp_edge);
    CHECK(contact_points[9].node == &nodes[5]);

    CHECK(contact_points[10].type == VerticalExtensionContactPoint::Type::leaf);
    CHECK(contact_points[10].node == &nodes[6]);

    CHECK(contact_points[11].type == VerticalExtensionContactPoint::Type::vertex_downwards);
    CHECK(contact_points[11].node == &nodes[5]);

    CHECK(contact_points[12].type == VerticalExtensionContactPoint::Type::vertex_upwards);
    CHECK(contact_points[12].node == &nodes[5]);

    CHECK(contact_points[13].type == VerticalExtensionContactPoint::Type::lower_opp_edge);
    CHECK(contact_points[13].node == &nodes[8]);

    CHECK(contact_points[14].type == VerticalExtensionContactPoint::Type::leaf);
    CHECK(contact_points[14].node == &nodes[9]);

    CHECK(contact_points[15].type == VerticalExtensionContactPoint::Type::upper_opp_edge);
    CHECK(contact_points[15].node == &nodes[8]);

    CHECK(contact_points[16].type == VerticalExtensionContactPoint::Type::leaf);
    CHECK(contact_points[16].node == &nodes[7]);

    CHECK(contact_points[17].type == VerticalExtensionContactPoint::Type::vertex_upwards);
    CHECK(contact_points[17].node == &nodes[8]);
  }

  SECTION("Chain 2")
  {
    // The same chain as in the previous section, rotated by 180 degrees. This way, we're covering all the cases.

    std::vector<Point2> vertices_storage{
        {3.00, -2.64},  {0.30, -3.64},  {3.12, -5.62},  {5.92, -3.10},  {2.76, -0.64},  {-0.78, -2.34}, {0.02, -0.62},
        {-2.88, -0.70}, {-2.06, -2.08}, {-5.34, -1.70}, {-6.74, -3.64}, {-4.40, -6.00}, {-3.20, -4.00}, {-4.68, -3.20},
    };
    VerticesView vertices(vertices_storage);

    std::vector<Node> nodes(10);
    nodes[0].direction = HorizontalDirection::right;
    nodes[0].type = NodeType::leaf;
    nodes[0].vertex_it = vertices.begin() + 3;
    nodes[0].lower_opp_edge = Edge::edge_from_index(vertices, 2);
    nodes[0].upper_opp_edge = Edge::edge_from_index(vertices, 3);
    nodes[0].neighbors[0] = &nodes[1];

    nodes[1].direction = HorizontalDirection::left;
    nodes[1].type = NodeType::branch;
    nodes[1].vertex_it = vertices.begin() + 0;
    nodes[1].lower_opp_edge = Edge::edge_from_index(vertices, 1);
    nodes[1].upper_opp_edge = Edge::edge_from_index(vertices, 3);
    nodes[1].neighbors[0] = &nodes[0];
    nodes[1].neighbors[1] = &nodes[2];
    nodes[1].neighbors[2] = &nodes[4];

    nodes[2].direction = HorizontalDirection::left;
    nodes[2].type = NodeType::leaf;
    nodes[2].vertex_it = vertices.begin() + 1;
    nodes[2].lower_opp_edge = Edge::edge_from_index(vertices, 1);
    nodes[2].upper_opp_edge = Edge::edge_from_index(vertices, 0);
    nodes[2].neighbors[0] = &nodes[1];

    nodes[3].direction = HorizontalDirection::right;
    nodes[3].type = NodeType::leaf;
    nodes[3].vertex_it = vertices.begin() + 6;
    nodes[3].lower_opp_edge = Edge::edge_from_index(vertices, 5);
    nodes[3].upper_opp_edge = Edge::edge_from_index(vertices, 6);
    nodes[3].neighbors[0] = &nodes[4];

    nodes[4].direction = HorizontalDirection::right;
    nodes[4].type = NodeType::branch;
    nodes[4].vertex_it = vertices.begin() + 5;
    nodes[4].lower_opp_edge = Edge::invalid();
    nodes[4].upper_opp_edge = Edge::edge_from_index(vertices, 6);
    nodes[4].neighbors[0] = &nodes[5];
    nodes[4].neighbors[1] = &nodes[1];
    nodes[4].neighbors[2] = &nodes[3];

    nodes[5].direction = HorizontalDirection::left;
    nodes[5].type = NodeType::branch;
    nodes[5].vertex_it = vertices.begin() + 8;
    nodes[5].lower_opp_edge = Edge::invalid();
    nodes[5].upper_opp_edge = Edge::edge_from_index(vertices, 6);
    nodes[5].neighbors[0] = &nodes[4];
    nodes[5].neighbors[1] = &nodes[8];
    nodes[5].neighbors[2] = &nodes[6];

    nodes[6].direction = HorizontalDirection::left;
    nodes[6].type = NodeType::leaf;
    nodes[6].vertex_it = vertices.begin() + 7;
    nodes[6].lower_opp_edge = Edge::edge_from_index(vertices, 7);
    nodes[6].upper_opp_edge = Edge::edge_from_index(vertices, 6);
    nodes[6].neighbors[0] = &nodes[5];

    nodes[7].direction = HorizontalDirection::right;
    nodes[7].type = NodeType::leaf;
    nodes[7].vertex_it = vertices.begin() + 12;
    nodes[7].lower_opp_edge = Edge::edge_from_index(vertices, 11);
    nodes[7].upper_opp_edge = Edge::edge_from_index(vertices, 12);
    nodes[7].neighbors[0] = &nodes[8];

    nodes[8].direction = HorizontalDirection::right;
    nodes[8].type = NodeType::branch;
    nodes[8].vertex_it = vertices.begin() + 13;
    nodes[8].lower_opp_edge = Edge::edge_from_index(vertices, 10);
    nodes[8].upper_opp_edge = Edge::edge_from_index(vertices, 8);
    nodes[8].neighbors[0] = &nodes[9];
    nodes[8].neighbors[1] = &nodes[7];
    nodes[8].neighbors[2] = &nodes[5];

    nodes[9].direction = HorizontalDirection::left;
    nodes[9].type = NodeType::leaf;
    nodes[9].vertex_it = vertices.begin() + 10;
    nodes[9].lower_opp_edge = Edge::edge_from_index(vertices, 10);
    nodes[9].upper_opp_edge = Edge::edge_from_index(vertices, 9);
    nodes[9].neighbors[0] = &nodes[8];

    ChainDecomposition chain_decomposition{&nodes[1], &nodes[8]};

    Winding winding = GENERATE(Winding::ccw, Winding::cw);
    if (winding == Winding::cw)
    {
      flip_horizontally(vertices_storage, nodes);
    }

    std::vector<VerticalExtensionContactPoint> contact_points =
        vertical_extension_contact_points(chain_decomposition, winding);

    REQUIRE(contact_points.size() == 18);

    CHECK(contact_points[0].type == VerticalExtensionContactPoint::Type::vertex_downwards);
    CHECK(contact_points[0].node == &nodes[1]);

    CHECK(contact_points[1].type == VerticalExtensionContactPoint::Type::leaf);
    CHECK(contact_points[1].node == &nodes[2]);

    CHECK(contact_points[2].type == VerticalExtensionContactPoint::Type::lower_opp_edge);
    CHECK(contact_points[2].node == &nodes[1]);

    CHECK(contact_points[3].type == VerticalExtensionContactPoint::Type::leaf);
    CHECK(contact_points[3].node == &nodes[0]);

    CHECK(contact_points[4].type == VerticalExtensionContactPoint::Type::upper_opp_edge);
    CHECK(contact_points[4].node == &nodes[1]);

    CHECK(contact_points[5].type == VerticalExtensionContactPoint::Type::vertex_downwards);
    CHECK(contact_points[5].node == &nodes[4]);

    CHECK(contact_points[6].type == VerticalExtensionContactPoint::Type::vertex_upwards);
    CHECK(contact_points[6].node == &nodes[4]);

    CHECK(contact_points[7].type == VerticalExtensionContactPoint::Type::leaf);
    CHECK(contact_points[7].node == &nodes[3]);

    CHECK(contact_points[8].type == VerticalExtensionContactPoint::Type::upper_opp_edge);
    CHECK(contact_points[8].node == &nodes[4]);

    CHECK(contact_points[9].type == VerticalExtensionContactPoint::Type::upper_opp_edge);
    CHECK(contact_points[9].node == &nodes[5]);

    CHECK(contact_points[10].type == VerticalExtensionContactPoint::Type::leaf);
    CHECK(contact_points[10].node == &nodes[6]);

    CHECK(contact_points[11].type == VerticalExtensionContactPoint::Type::vertex_upwards);
    CHECK(contact_points[11].node == &nodes[5]);

    CHECK(contact_points[12].type == VerticalExtensionContactPoint::Type::vertex_downwards);
    CHECK(contact_points[12].node == &nodes[5]);

    CHECK(contact_points[13].type == VerticalExtensionContactPoint::Type::upper_opp_edge);
    CHECK(contact_points[13].node == &nodes[8]);

    CHECK(contact_points[14].type == VerticalExtensionContactPoint::Type::leaf);
    CHECK(contact_points[14].node == &nodes[9]);

    CHECK(contact_points[15].type == VerticalExtensionContactPoint::Type::lower_opp_edge);
    CHECK(contact_points[15].node == &nodes[8]);

    CHECK(contact_points[16].type == VerticalExtensionContactPoint::Type::leaf);
    CHECK(contact_points[16].node == &nodes[7]);

    CHECK(contact_points[17].type == VerticalExtensionContactPoint::Type::vertex_downwards);
    CHECK(contact_points[17].node == &nodes[8]);
  }
}

TEST_CASE("split_chain_decomposition_into_islands")
{
  auto flip_x_if_necessary = [](Winding winding, ScalarDeg1 x) { return winding == Winding::ccw ? x : -x; };

  SECTION("Split at contact point of type 'vertex_downwards'")
  {
    std::vector<Point2> vertices_storage{
        {2.40, 4.74},  {4.06, 5.68},  {-1.22, 6.18}, {-0.06, 5.00},
        {-3.96, 4.14}, {-2.34, 3.46}, {-2.98, 2.74}, {3.48, 3.94},
    };
    VerticesView vertices(vertices_storage);

    std::vector<Node> nodes(6);
    nodes[0].direction = HorizontalDirection::left;
    nodes[0].type = NodeType::leaf;
    nodes[0].vertex_it = vertices.begin() + 4;
    nodes[0].lower_opp_edge = Edge::edge_from_index(vertices, 4);
    nodes[0].upper_opp_edge = Edge::edge_from_index(vertices, 3);
    nodes[0].neighbors[0] = &nodes[1];

    nodes[1].direction = HorizontalDirection::left;
    nodes[1].type = NodeType::branch;
    nodes[1].vertex_it = vertices.begin() + 5;
    nodes[1].lower_opp_edge = Edge::invalid();
    nodes[1].upper_opp_edge = Edge::edge_from_index(vertices, 3);
    nodes[1].neighbors[0] = &nodes[3];
    nodes[1].neighbors[1] = nullptr;
    nodes[1].neighbors[2] = &nodes[0];

    nodes[2].direction = HorizontalDirection::left;
    nodes[2].type = NodeType::leaf;
    nodes[2].vertex_it = vertices.begin() + 2;
    nodes[2].lower_opp_edge = Edge::edge_from_index(vertices, 2);
    nodes[2].upper_opp_edge = Edge::edge_from_index(vertices, 1);
    nodes[2].neighbors[0] = &nodes[3];

    nodes[3].direction = HorizontalDirection::left;
    nodes[3].type = NodeType::branch;
    nodes[3].vertex_it = vertices.begin() + 3;
    nodes[3].lower_opp_edge = Edge::invalid();
    nodes[3].upper_opp_edge = Edge::edge_from_index(vertices, 1);
    nodes[3].neighbors[0] = &nodes[4];
    nodes[3].neighbors[1] = &nodes[1];
    nodes[3].neighbors[2] = &nodes[2];

    nodes[4].direction = HorizontalDirection::right;
    nodes[4].type = NodeType::branch;
    nodes[4].vertex_it = vertices.begin() + 0;
    nodes[4].lower_opp_edge = Edge::invalid();
    nodes[4].upper_opp_edge = Edge::edge_from_index(vertices, 1);
    nodes[4].neighbors[0] = &nodes[3];
    nodes[4].neighbors[1] = nullptr;
    nodes[4].neighbors[2] = &nodes[5];

    nodes[5].direction = HorizontalDirection::right;
    nodes[5].type = NodeType::leaf;
    nodes[5].vertex_it = vertices.begin() + 1;
    nodes[5].lower_opp_edge = Edge::edge_from_index(vertices, 0);
    nodes[5].upper_opp_edge = Edge::edge_from_index(vertices, 1);
    nodes[5].neighbors[0] = &nodes[4];

    ChainDecomposition chain_decomposition{&nodes[4], &nodes[1]};

    Winding winding = GENERATE(Winding::ccw, Winding::cw);
    if (winding == Winding::cw)
    {
      flip_horizontally(vertices_storage, nodes);
    }

    std::vector<VerticalExtensionContactPoint> contact_points =
        vertical_extension_contact_points(chain_decomposition, winding);
    REQUIRE(contact_points.size() == 10);

    std::vector<ChainDecompositionIsland> islands =
        split_chain_decomposition_into_islands(vertices, winding, chain_decomposition, contact_points);

    REQUIRE(islands.size() == 2);

    CHECK(islands[0].contact_points.begin() == contact_points.data());
    CHECK(islands[0].contact_points.end() == contact_points.data() + 6);
    CHECK(islands[0].range.first_edge_index == 0);
    CHECK(islands[0].range.num_edges == 3);
    CHECK(islands[0].range.start_point_x == flip_x_if_necessary(winding, ScalarDeg1(2.40)));
    CHECK(islands[0].range.end_point_x == flip_x_if_necessary(winding, ScalarDeg1(-0.06)));

    CHECK(islands[1].contact_points.begin() == contact_points.data() + 7);
    CHECK(islands[1].contact_points.end() == contact_points.data() + 10);
    CHECK(islands[1].range.first_edge_index == 3);
    CHECK(islands[1].range.num_edges == 2);
    CHECK(islands[1].range.start_point_x == flip_x_if_necessary(winding, ScalarDeg1(-0.06)));
    CHECK(islands[1].range.end_point_x == flip_x_if_necessary(winding, ScalarDeg1(-2.34)));
  }

  SECTION("Split at contact point of type 'vertex_upwards'")
  {
    std::vector<Point2> vertices_storage{
        {-1.20, 4.86}, {-2.50, 4.24}, {0.56, 2.96}, {-0.24, 2.16},
        {2.66, 2.28},  {2.04, 3.26},  {3.44, 4.68}, {-1.92, 5.72},
    };
    VerticesView vertices(vertices_storage);

    std::vector<Node> nodes(6);
    nodes[0].direction = HorizontalDirection::left;
    nodes[0].type = NodeType::leaf;
    nodes[0].vertex_it = vertices.begin() + 1;
    nodes[0].lower_opp_edge = Edge::edge_from_index(vertices, 1);
    nodes[0].upper_opp_edge = Edge::edge_from_index(vertices, 0);
    nodes[0].neighbors[0] = &nodes[1];

    nodes[1].direction = HorizontalDirection::left;
    nodes[1].type = NodeType::branch;
    nodes[1].vertex_it = vertices.begin() + 0;
    nodes[1].lower_opp_edge = Edge::edge_from_index(vertices, 1);
    nodes[1].upper_opp_edge = Edge::invalid();
    nodes[1].neighbors[0] = &nodes[3];
    nodes[1].neighbors[1] = &nodes[0];
    nodes[1].neighbors[2] = nullptr;

    nodes[2].direction = HorizontalDirection::left;
    nodes[2].type = NodeType::leaf;
    nodes[2].vertex_it = vertices.begin() + 3;
    nodes[2].lower_opp_edge = Edge::edge_from_index(vertices, 3);
    nodes[2].upper_opp_edge = Edge::edge_from_index(vertices, 2);
    nodes[2].neighbors[0] = &nodes[3];

    nodes[3].direction = HorizontalDirection::left;
    nodes[3].type = NodeType::branch;
    nodes[3].vertex_it = vertices.begin() + 2;
    nodes[3].lower_opp_edge = Edge::edge_from_index(vertices, 3);
    nodes[3].upper_opp_edge = Edge::invalid();
    nodes[3].neighbors[0] = &nodes[4];
    nodes[3].neighbors[1] = &nodes[2];
    nodes[3].neighbors[2] = &nodes[1];

    nodes[4].direction = HorizontalDirection::right;
    nodes[4].type = NodeType::branch;
    nodes[4].vertex_it = vertices.begin() + 5;
    nodes[4].lower_opp_edge = Edge::edge_from_index(vertices, 3);
    nodes[4].upper_opp_edge = Edge::invalid();
    nodes[4].neighbors[0] = &nodes[3];
    nodes[4].neighbors[1] = &nodes[5];
    nodes[4].neighbors[2] = nullptr;

    nodes[5].direction = HorizontalDirection::right;
    nodes[5].type = NodeType::leaf;
    nodes[5].vertex_it = vertices.begin() + 4;
    nodes[5].lower_opp_edge = Edge::edge_from_index(vertices, 3);
    nodes[5].upper_opp_edge = Edge::edge_from_index(vertices, 4);
    nodes[5].neighbors[0] = &nodes[4];

    ChainDecomposition chain_decomposition{&nodes[1], &nodes[4]};

    Winding winding = GENERATE(Winding::ccw, Winding::cw);
    if (winding == Winding::cw)
    {
      flip_horizontally(vertices_storage, nodes);
    }

    std::vector<VerticalExtensionContactPoint> contact_points =
        vertical_extension_contact_points(chain_decomposition, winding);
    REQUIRE(contact_points.size() == 10);

    std::vector<ChainDecompositionIsland> islands =
        split_chain_decomposition_into_islands(vertices, winding, chain_decomposition, contact_points);

    REQUIRE(islands.size() == 2);

    CHECK(islands[0].contact_points.begin() == contact_points.data());
    CHECK(islands[0].contact_points.end() == contact_points.data() + 3);
    CHECK(islands[0].range.first_edge_index == 0);
    CHECK(islands[0].range.num_edges == 2);
    CHECK(islands[0].range.start_point_x == flip_x_if_necessary(winding, ScalarDeg1(-1.20)));
    CHECK(islands[0].range.end_point_x == flip_x_if_necessary(winding, ScalarDeg1(0.56)));

    CHECK(islands[1].contact_points.begin() == contact_points.data() + 4);
    CHECK(islands[1].contact_points.end() == contact_points.data() + 10);
    CHECK(islands[1].range.first_edge_index == 2);
    CHECK(islands[1].range.num_edges == 3);
    CHECK(islands[1].range.start_point_x == flip_x_if_necessary(winding, ScalarDeg1(0.56)));
    CHECK(islands[1].range.end_point_x == flip_x_if_necessary(winding, ScalarDeg1(2.04)));
  }

  SECTION("Split at contact point of type 'lower_opp_edge'")
  {
    std::vector<Point2> vertices_storage{
        {-5.10, 3.10},  {-3.62, 4.28}, {-4.80, 6.04}, {-8.00, 3.60}, {-5.68, -0.34},
        {-1.84, -0.34}, {0.76, 1.74},  {-1.62, 2.64}, {-1.00, 3.22},
    };

    VerticesView vertices(vertices_storage);

    std::vector<Node> nodes(5);
    nodes[0].direction = HorizontalDirection::left;
    nodes[0].type = NodeType::leaf;
    nodes[0].vertex_it = vertices.begin() + 3;
    nodes[0].lower_opp_edge = Edge::edge_from_index(vertices, 3);
    nodes[0].upper_opp_edge = Edge::edge_from_index(vertices, 2);
    nodes[0].neighbors[0] = &nodes[1];

    nodes[1].direction = HorizontalDirection::right;
    nodes[1].type = NodeType::branch;
    nodes[1].vertex_it = vertices.begin() + 0;
    nodes[1].lower_opp_edge = Edge::edge_from_index(vertices, 4);
    nodes[1].upper_opp_edge = Edge::edge_from_index(vertices, 2);
    nodes[1].neighbors[0] = &nodes[0];
    nodes[1].neighbors[1] = &nodes[3];
    nodes[1].neighbors[2] = &nodes[2];

    nodes[2].direction = HorizontalDirection::right;
    nodes[2].type = NodeType::leaf;
    nodes[2].vertex_it = vertices.begin() + 1;
    nodes[2].lower_opp_edge = Edge::edge_from_index(vertices, 0);
    nodes[2].upper_opp_edge = Edge::edge_from_index(vertices, 1);
    nodes[2].neighbors[0] = &nodes[1];

    nodes[3].direction = HorizontalDirection::right;
    nodes[3].type = NodeType::branch;
    nodes[3].vertex_it = vertices.begin() + 7;
    nodes[3].lower_opp_edge = Edge::edge_from_index(vertices, 5);
    nodes[3].upper_opp_edge = Edge::invalid();
    nodes[3].neighbors[0] = &nodes[1];
    nodes[3].neighbors[1] = &nodes[4];
    nodes[3].neighbors[2] = nullptr;

    nodes[4].direction = HorizontalDirection::right;
    nodes[4].type = NodeType::leaf;
    nodes[4].vertex_it = vertices.begin() + 6;
    nodes[4].lower_opp_edge = Edge::edge_from_index(vertices, 5);
    nodes[4].upper_opp_edge = Edge::edge_from_index(vertices, 6);
    nodes[4].neighbors[0] = &nodes[3];

    ChainDecomposition chain_decomposition{&nodes[1], &nodes[3]};

    Winding winding = GENERATE(Winding::ccw, Winding::cw);
    if (winding == Winding::cw)
    {
      flip_horizontally(vertices_storage, nodes);
    }

    std::vector<VerticalExtensionContactPoint> contact_points =
        vertical_extension_contact_points(chain_decomposition, winding);
    REQUIRE(contact_points.size() == 8);

    std::vector<ChainDecompositionIsland> islands =
        split_chain_decomposition_into_islands(vertices, winding, chain_decomposition, contact_points);

    REQUIRE(islands.size() == 2);

    CHECK(islands[0].contact_points.begin() == contact_points.data());
    CHECK(islands[0].contact_points.end() == contact_points.data() + 4);
    CHECK(islands[0].range.first_edge_index == 0);
    CHECK(islands[0].range.num_edges == 5);
    CHECK(islands[0].range.start_point_x == flip_x_if_necessary(winding, ScalarDeg1(-5.10)));
    CHECK(islands[0].range.end_point_x == flip_x_if_necessary(winding, ScalarDeg1(-5.10)));

    CHECK(islands[1].contact_points.begin() == contact_points.data() + 5);
    CHECK(islands[1].contact_points.end() == contact_points.data() + 8);
    CHECK(islands[1].range.first_edge_index == 4);
    CHECK(islands[1].range.num_edges == 3);
    CHECK(islands[1].range.start_point_x == flip_x_if_necessary(winding, ScalarDeg1(-5.10)));
    CHECK(islands[1].range.end_point_x == flip_x_if_necessary(winding, ScalarDeg1(-1.62)));
  }

  SECTION("Split at contact point of type 'upper_opp_edge'")
  {
    std::vector<Point2> vertices_storage{
        {4.28, 6.00}, {2.54, 5.00},  {3.38, 3.68}, {7.64, 5.94},
        {4.06, 9.02}, {-0.26, 7.54}, {0.56, 6.62}, {-1.74, 5.66},
    };
    VerticesView vertices(vertices_storage);

    std::vector<Node> nodes(6);
    nodes[0].direction = HorizontalDirection::right;
    nodes[0].type = NodeType::branch;
    nodes[0].vertex_it = vertices.begin() + 7;
    nodes[0].lower_opp_edge = Edge::invalid();
    nodes[0].upper_opp_edge = Edge::invalid();
    nodes[0].neighbors[0] = nullptr;
    nodes[0].neighbors[1] = nullptr;
    nodes[0].neighbors[1] = &nodes[2];

    nodes[1].direction = HorizontalDirection::left;
    nodes[1].type = NodeType::leaf;
    nodes[1].vertex_it = vertices.begin() + 5;
    nodes[1].lower_opp_edge = Edge::edge_from_index(vertices, 5);
    nodes[1].upper_opp_edge = Edge::edge_from_index(vertices, 4);
    nodes[1].neighbors[0] = &nodes[2];

    nodes[2].direction = HorizontalDirection::left;
    nodes[2].type = NodeType::branch;
    nodes[2].vertex_it = vertices.begin() + 6;
    nodes[2].lower_opp_edge = Edge::invalid();
    nodes[2].upper_opp_edge = Edge::edge_from_index(vertices, 4);
    nodes[2].neighbors[0] = &nodes[4];
    nodes[2].neighbors[1] = &nodes[0];
    nodes[2].neighbors[2] = &nodes[1];

    nodes[3].direction = HorizontalDirection::left;
    nodes[3].type = NodeType::leaf;
    nodes[3].vertex_it = vertices.begin() + 1;
    nodes[3].lower_opp_edge = Edge::edge_from_index(vertices, 1);
    nodes[3].upper_opp_edge = Edge::edge_from_index(vertices, 0);
    nodes[3].neighbors[0] = &nodes[4];

    nodes[4].direction = HorizontalDirection::left;
    nodes[4].type = NodeType::branch;
    nodes[4].vertex_it = vertices.begin() + 0;
    nodes[4].lower_opp_edge = Edge::edge_from_index(vertices, 2);
    nodes[4].upper_opp_edge = Edge::edge_from_index(vertices, 3);
    nodes[4].neighbors[0] = &nodes[5];
    nodes[4].neighbors[1] = &nodes[3];
    nodes[4].neighbors[2] = &nodes[2];

    nodes[5].direction = HorizontalDirection::right;
    nodes[5].type = NodeType::leaf;
    nodes[5].vertex_it = vertices.begin() + 3;
    nodes[5].lower_opp_edge = Edge::edge_from_index(vertices, 2);
    nodes[5].upper_opp_edge = Edge::edge_from_index(vertices, 3);
    nodes[5].neighbors[0] = &nodes[4];

    ChainDecomposition chain_decomposition{&nodes[4], &nodes[0]};

    Winding winding = GENERATE(Winding::ccw, Winding::cw);
    if (winding == Winding::cw)
    {
      flip_horizontally(vertices_storage, nodes);
    }

    std::vector<VerticalExtensionContactPoint> contact_points =
        vertical_extension_contact_points(chain_decomposition, winding);
    REQUIRE(contact_points.size() == 10);

    std::vector<ChainDecompositionIsland> islands =
        split_chain_decomposition_into_islands(vertices, winding, chain_decomposition, contact_points);

    REQUIRE(islands.size() == 2);

    CHECK(islands[0].contact_points.begin() == contact_points.data());
    CHECK(islands[0].contact_points.end() == contact_points.data() + 4);
    CHECK(islands[0].range.first_edge_index == 0);
    CHECK(islands[0].range.num_edges == 4);
    CHECK(islands[0].range.start_point_x == flip_x_if_necessary(winding, ScalarDeg1(4.28)));
    CHECK(islands[0].range.end_point_x == flip_x_if_necessary(winding, ScalarDeg1(4.28)));

    CHECK(islands[1].contact_points.begin() == contact_points.data() + 5);
    CHECK(islands[1].contact_points.end() == contact_points.data() + 8);
    CHECK(islands[1].range.first_edge_index == 3);
    CHECK(islands[1].range.num_edges == 3);
    CHECK(islands[1].range.start_point_x == flip_x_if_necessary(winding, ScalarDeg1(4.28)));
    CHECK(islands[1].range.end_point_x == flip_x_if_necessary(winding, ScalarDeg1(0.56)));
  }

  SECTION("Infinite vertical extensions which don't agree with ray-cast result")
  {
    SECTION("Contact point on lower_opp_edge")
    {
      std::vector<Point2> vertices_storage{
          {-0.06, 4.80}, {1.58, 5.86}, {-2.78, 6.52}, {-3.82, 4.32},
          {-0.84, 1.72}, {4.16, 2.64}, {-1.02, 3.96}, {0.80, 4.26},
      };
      VerticesView vertices(vertices_storage);

      std::vector<Node> nodes(6);
      nodes[0].direction = HorizontalDirection::left;
      nodes[0].type = NodeType::leaf;
      nodes[0].vertex_it = vertices.begin() + 3;
      nodes[0].lower_opp_edge = Edge::edge_from_index(vertices, 3);
      nodes[0].upper_opp_edge = Edge::edge_from_index(vertices, 2);
      nodes[0].neighbors[0] = &nodes[1];

      nodes[1].direction = HorizontalDirection::right;
      nodes[1].type = NodeType::branch;
      nodes[1].vertex_it = vertices.begin() + 6;
      nodes[1].lower_opp_edge = Edge::edge_from_index(vertices, 3);
      nodes[1].upper_opp_edge = Edge::edge_from_index(vertices, 1);
      nodes[1].neighbors[0] = &nodes[0];
      nodes[1].neighbors[1] = &nodes[5];
      nodes[1].neighbors[2] = &nodes[2];

      nodes[2].direction = HorizontalDirection::right;
      nodes[2].type = NodeType::branch;
      nodes[2].vertex_it = vertices.begin() + 0;
      nodes[2].lower_opp_edge = Edge::edge_from_index(vertices, 4);
      nodes[2].upper_opp_edge = Edge::edge_from_index(vertices, 1);
      nodes[2].neighbors[0] = &nodes[1];
      nodes[2].neighbors[1] = &nodes[3];
      nodes[2].neighbors[2] = &nodes[4];

      nodes[3].direction = HorizontalDirection::left;
      nodes[3].type = NodeType::branch;
      nodes[3].vertex_it = vertices.begin() + 7;
      nodes[3].lower_opp_edge = Edge::invalid();
      nodes[3].upper_opp_edge = Edge::invalid();
      nodes[3].neighbors[0] = nullptr;
      nodes[3].neighbors[1] = nullptr;
      nodes[3].neighbors[2] = &nodes[2];

      nodes[4].direction = HorizontalDirection::right;
      nodes[4].type = NodeType::leaf;
      nodes[4].vertex_it = vertices.begin() + 1;
      nodes[4].lower_opp_edge = Edge::edge_from_index(vertices, 0);
      nodes[4].upper_opp_edge = Edge::edge_from_index(vertices, 1);
      nodes[4].neighbors[0] = &nodes[2];

      nodes[5].direction = HorizontalDirection::right;
      nodes[5].type = NodeType::leaf;
      nodes[5].vertex_it = vertices.begin() + 5;
      nodes[5].lower_opp_edge = Edge::edge_from_index(vertices, 4);
      nodes[5].upper_opp_edge = Edge::edge_from_index(vertices, 5);
      nodes[5].neighbors[0] = &nodes[1];

      ChainDecomposition chain_decomposition{&nodes[2], &nodes[3]};

      Winding winding = GENERATE(Winding::ccw, Winding::cw);
      if (winding == Winding::cw)
      {
        flip_horizontally(vertices_storage, nodes);
      }

      std::vector<VerticalExtensionContactPoint> contact_points =
          vertical_extension_contact_points(chain_decomposition, winding);
      REQUIRE(contact_points.size() == 11);

      std::vector<ChainDecompositionIsland> islands =
          split_chain_decomposition_into_islands(vertices, winding, chain_decomposition, contact_points);

      REQUIRE(islands.size() == 1);
      CHECK(islands[0].contact_points.begin() == contact_points.data());
      CHECK(islands[0].contact_points.end() == contact_points.data() + 10);
      CHECK(islands[0].range.first_edge_index == 0);
      CHECK(islands[0].range.num_edges == 7);
      CHECK(islands[0].range.start_point_x == flip_x_if_necessary(winding, ScalarDeg1(-.06)));
      CHECK(islands[0].range.end_point_x == flip_x_if_necessary(winding, ScalarDeg1(.8)));
    }

    SECTION("Contact point on upper_opp_edge")
    {
      std::vector<Point2> vertices_storage{
          {4.88, 8.22}, {3.34, 7.14}, {6.26, 5.08}, {10.54, 7.50}, {4.08, 9.62}, {5.30, 8.76}, {4.26, 8.50},
      };
      VerticesView vertices(vertices_storage);

      std::vector<Node> nodes(6);
      nodes[0].direction = HorizontalDirection::left;
      nodes[0].type = NodeType::leaf;
      nodes[0].vertex_it = vertices.begin() + 1;
      nodes[0].lower_opp_edge = Edge::edge_from_index(vertices, 1);
      nodes[0].upper_opp_edge = Edge::edge_from_index(vertices, 0);
      nodes[0].neighbors[0] = &nodes[3];

      nodes[1].direction = HorizontalDirection::left;
      nodes[1].type = NodeType::leaf;
      nodes[1].vertex_it = vertices.begin() + 4;
      nodes[1].lower_opp_edge = Edge::edge_from_index(vertices, 4);
      nodes[1].upper_opp_edge = Edge::edge_from_index(vertices, 3);
      nodes[1].neighbors[0] = &nodes[4];

      nodes[2].direction = HorizontalDirection::right;
      nodes[2].type = NodeType::branch;
      nodes[2].vertex_it = vertices.begin() + 6;
      nodes[2].lower_opp_edge = Edge::invalid();
      nodes[2].upper_opp_edge = Edge::invalid();
      nodes[2].neighbors[0] = nullptr;
      nodes[2].neighbors[1] = &nodes[3];
      nodes[2].neighbors[2] = nullptr;

      nodes[3].direction = HorizontalDirection::left;
      nodes[3].type = NodeType::branch;
      nodes[3].vertex_it = vertices.begin() + 0;
      nodes[3].lower_opp_edge = Edge::edge_from_index(vertices, 1);
      nodes[3].upper_opp_edge = Edge::edge_from_index(vertices, 3);
      nodes[3].neighbors[0] = &nodes[4];
      nodes[3].neighbors[1] = &nodes[0];
      nodes[3].neighbors[2] = &nodes[2];

      nodes[4].direction = HorizontalDirection::left;
      nodes[4].type = NodeType::branch;
      nodes[4].vertex_it = vertices.begin() + 5;
      nodes[4].lower_opp_edge = Edge::edge_from_index(vertices, 1);
      nodes[4].upper_opp_edge = Edge::edge_from_index(vertices, 3);
      nodes[4].neighbors[0] = &nodes[5];
      nodes[4].neighbors[1] = &nodes[3];
      nodes[4].neighbors[2] = &nodes[1];

      nodes[5].direction = HorizontalDirection::right;
      nodes[5].type = NodeType::leaf;
      nodes[5].vertex_it = vertices.begin() + 3;
      nodes[5].lower_opp_edge = Edge::edge_from_index(vertices, 2);
      nodes[5].upper_opp_edge = Edge::edge_from_index(vertices, 3);
      nodes[5].neighbors[0] = &nodes[4];

      ChainDecomposition chain_decomposition{&nodes[3], &nodes[2]};

      Winding winding = GENERATE(Winding::ccw, Winding::cw);
      if (winding == Winding::cw)
      {
        flip_horizontally(vertices_storage, nodes);
      }

      std::vector<VerticalExtensionContactPoint> contact_points =
          vertical_extension_contact_points(chain_decomposition, winding);
      REQUIRE(contact_points.size() == 11);

      std::vector<ChainDecompositionIsland> islands =
          split_chain_decomposition_into_islands(vertices, winding, chain_decomposition, contact_points);

      REQUIRE(islands.size() == 1);
      CHECK(islands[0].contact_points.begin() == contact_points.data());
      CHECK(islands[0].contact_points.end() == contact_points.data() + 10);
      CHECK(islands[0].range.first_edge_index == 0);
      CHECK(islands[0].range.num_edges == 6);
      CHECK(islands[0].range.start_point_x == flip_x_if_necessary(winding, ScalarDeg1(4.88)));
      CHECK(islands[0].range.end_point_x == flip_x_if_necessary(winding, ScalarDeg1(4.26)));
    }

    SECTION("Contact points on vertices")
    {
      std::vector<Point2> vertices_storage{
          {-2.50, 4.54}, {-3.14, 3.68}, {0.58, 3.86},  {-0.30, 3.00}, {4.10, 4.66},
          {0.88, 5.28},  {1.84, 6.30},  {-0.08, 5.20}, {-3.62, 5.96},
      };
      VerticesView vertices(vertices_storage);

      std::vector<Node> nodes(8);
      nodes[0].direction = HorizontalDirection::right;
      nodes[0].type = NodeType::branch;
      nodes[0].vertex_it = vertices.begin() + 8;
      nodes[0].lower_opp_edge = Edge::invalid();
      nodes[0].upper_opp_edge = Edge::invalid();
      nodes[0].neighbors[0] = nullptr;
      nodes[0].neighbors[1] = &nodes[2];
      nodes[0].neighbors[2] = nullptr;

      nodes[1].direction = HorizontalDirection::left;
      nodes[1].type = NodeType::leaf;
      nodes[1].vertex_it = vertices.begin() + 1;
      nodes[1].lower_opp_edge = Edge::edge_from_index(vertices, 1);
      nodes[1].upper_opp_edge = Edge::edge_from_index(vertices, 0);
      nodes[1].neighbors[0] = &nodes[2];

      nodes[2].direction = HorizontalDirection::left;
      nodes[2].type = NodeType::branch;
      nodes[2].vertex_it = vertices.begin() + 0;
      nodes[2].lower_opp_edge = Edge::edge_from_index(vertices, 1);
      nodes[2].upper_opp_edge = Edge::edge_from_index(vertices, 7);
      nodes[2].neighbors[0] = &nodes[4];
      nodes[2].neighbors[1] = &nodes[1];
      nodes[2].neighbors[2] = &nodes[0];

      nodes[3].direction = HorizontalDirection::left;
      nodes[3].type = NodeType::leaf;
      nodes[3].vertex_it = vertices.begin() + 3;
      nodes[3].lower_opp_edge = Edge::edge_from_index(vertices, 3);
      nodes[3].upper_opp_edge = Edge::edge_from_index(vertices, 2);
      nodes[3].neighbors[0] = &nodes[4];

      nodes[4].direction = HorizontalDirection::left;
      nodes[4].type = NodeType::branch;
      nodes[4].vertex_it = vertices.begin() + 2;
      nodes[4].lower_opp_edge = Edge::invalid();
      nodes[4].upper_opp_edge = Edge::edge_from_index(vertices, 6);
      nodes[4].neighbors[0] = &nodes[5];
      nodes[4].neighbors[1] = &nodes[3];
      nodes[4].neighbors[2] = &nodes[2];

      nodes[5].direction = HorizontalDirection::right;
      nodes[5].type = NodeType::branch;
      nodes[5].vertex_it = vertices.begin() + 5;
      nodes[5].lower_opp_edge = Edge::edge_from_index(vertices, 3);
      nodes[5].upper_opp_edge = Edge::invalid();
      nodes[5].neighbors[0] = &nodes[4];
      nodes[5].neighbors[1] = &nodes[7];
      nodes[5].neighbors[2] = &nodes[6];

      nodes[6].direction = HorizontalDirection::right;
      nodes[6].type = NodeType::leaf;
      nodes[6].vertex_it = vertices.begin() + 6;
      nodes[6].lower_opp_edge = Edge::edge_from_index(vertices, 5);
      nodes[6].upper_opp_edge = Edge::edge_from_index(vertices, 6);
      nodes[6].neighbors[0] = &nodes[5];

      nodes[7].direction = HorizontalDirection::right;
      nodes[7].type = NodeType::leaf;
      nodes[7].vertex_it = vertices.begin() + 4;
      nodes[7].lower_opp_edge = Edge::edge_from_index(vertices, 3);
      nodes[7].upper_opp_edge = Edge::edge_from_index(vertices, 4);
      nodes[7].neighbors[0] = &nodes[5];

      ChainDecomposition chain_decomposition{&nodes[2], &nodes[0]};

      Winding winding = GENERATE(Winding::ccw, Winding::cw);
      if(winding == Winding::cw)
      {
        flip_horizontally(vertices_storage, nodes);
      }

      std::vector<VerticalExtensionContactPoint> contact_points =
          vertical_extension_contact_points(chain_decomposition, winding);
      REQUIRE(contact_points.size() == 14);

      std::vector<ChainDecompositionIsland> islands =
          split_chain_decomposition_into_islands(vertices, winding, chain_decomposition, contact_points);

      REQUIRE(islands.size() == 1);
      CHECK(islands[0].contact_points.begin() == contact_points.data());
      CHECK(islands[0].contact_points.end() == contact_points.data() + 12);
      CHECK(islands[0].range.first_edge_index == 0);
      CHECK(islands[0].range.num_edges == 8);
      CHECK(islands[0].range.start_point_x == flip_x_if_necessary(winding, ScalarDeg1(-2.50)));
      CHECK(islands[0].range.end_point_x == flip_x_if_necessary(winding, ScalarDeg1(-2.50)));
    }
  }

  SECTION("Split at multiple locations, in one pass")
  {
    std::vector<Point2> vertices_storage{
        {-3.60, 3.22}, {-5.20, 2.88}, {-1.28, 0.98}, {-2.52, 0.12}, {2.54, 0.30},
        {1.24, 1.12},  {5.10, 2.92},  {2.64, 3.66},  {3.32, 4.42},  {-4.18, 4.28},
    };
    VerticesView vertices(vertices_storage);

    std::vector<Node> nodes(8);
    nodes[0].direction = HorizontalDirection::left;
    nodes[0].type = NodeType::leaf;
    nodes[0].vertex_it = vertices.begin() + 1;
    nodes[0].lower_opp_edge = Edge::edge_from_index(vertices, 1);
    nodes[0].upper_opp_edge = Edge::edge_from_index(vertices, 0);
    nodes[0].neighbors[0] = &nodes[1];

    nodes[1].direction = HorizontalDirection::left;
    nodes[1].type = NodeType::branch;
    nodes[1].vertex_it = vertices.begin() + 0;
    nodes[1].lower_opp_edge = Edge::edge_from_index(vertices, 1);
    nodes[1].upper_opp_edge = Edge::invalid();
    nodes[1].neighbors[0] = &nodes[3];
    nodes[1].neighbors[1] = &nodes[0];
    nodes[1].neighbors[2] = nullptr;

    nodes[2].direction = HorizontalDirection::left;
    nodes[2].type = NodeType::leaf;
    nodes[2].vertex_it = vertices.begin() + 3;
    nodes[2].lower_opp_edge = Edge::edge_from_index(vertices, 3);
    nodes[2].upper_opp_edge = Edge::edge_from_index(vertices, 2);
    nodes[2].neighbors[0] = &nodes[3];

    nodes[3].direction = HorizontalDirection::left;
    nodes[3].type = NodeType::branch;
    nodes[3].vertex_it = vertices.begin() + 2;
    nodes[3].lower_opp_edge = Edge::edge_from_index(vertices, 3);
    nodes[3].upper_opp_edge = Edge::invalid();
    nodes[3].neighbors[0] = &nodes[4];
    nodes[3].neighbors[1] = &nodes[2];
    nodes[3].neighbors[2] = &nodes[1];

    nodes[4].direction = HorizontalDirection::right;
    nodes[4].type = NodeType::branch;
    nodes[4].vertex_it = vertices.begin() + 5;
    nodes[4].lower_opp_edge = Edge::edge_from_index(vertices, 3);
    nodes[4].upper_opp_edge = Edge::invalid();
    nodes[4].neighbors[0] = &nodes[3];
    nodes[4].neighbors[1] = &nodes[5];
    nodes[4].neighbors[2] = &nodes[6];

    nodes[5].direction = HorizontalDirection::right;
    nodes[5].type = NodeType::leaf;
    nodes[5].vertex_it = vertices.begin() + 4;
    nodes[5].lower_opp_edge = Edge::edge_from_index(vertices, 3);
    nodes[5].upper_opp_edge = Edge::edge_from_index(vertices, 4);
    nodes[5].neighbors[0] = &nodes[4];

    nodes[6].direction = HorizontalDirection::right;
    nodes[6].type = NodeType::branch;
    nodes[6].vertex_it = vertices.begin() + 7;
    nodes[6].lower_opp_edge = Edge::edge_from_index(vertices, 5);
    nodes[6].upper_opp_edge = Edge::invalid();
    nodes[6].neighbors[0] = &nodes[4];
    nodes[6].neighbors[1] = &nodes[7];
    nodes[6].neighbors[2] = nullptr;

    nodes[7].direction = HorizontalDirection::right;
    nodes[7].type = NodeType::leaf;
    nodes[7].vertex_it = vertices.begin() + 6;
    nodes[7].lower_opp_edge = Edge::edge_from_index(vertices, 5);
    nodes[7].upper_opp_edge = Edge::edge_from_index(vertices, 6);
    nodes[7].neighbors[0] = &nodes[6];

    ChainDecomposition chain_decomposition{&nodes[1], &nodes[6]};

    Winding winding = GENERATE(Winding::ccw, Winding::cw);
    if (winding == Winding::cw)
    {
      flip_horizontally(vertices_storage, nodes);
    }

    std::vector<VerticalExtensionContactPoint> contact_points =
        vertical_extension_contact_points(chain_decomposition, winding);
    REQUIRE(contact_points.size() == 14);

    std::vector<ChainDecompositionIsland> islands =
        split_chain_decomposition_into_islands(vertices, winding, chain_decomposition, contact_points);

    REQUIRE(islands.size() == 3);

    CHECK(islands[0].contact_points.begin() == contact_points.data());
    CHECK(islands[0].contact_points.end() == contact_points.data() + 3);
    CHECK(islands[0].range.first_edge_index == 0);
    CHECK(islands[0].range.num_edges == 2);
    CHECK(islands[0].range.start_point_x == flip_x_if_necessary(winding, ScalarDeg1(-3.60)));
    CHECK(islands[0].range.end_point_x == flip_x_if_necessary(winding, ScalarDeg1(-1.28)));

    CHECK(islands[1].contact_points.begin() == contact_points.data() + 4);
    CHECK(islands[1].contact_points.end() == contact_points.data() + 10);
    CHECK(islands[1].range.first_edge_index == 2);
    CHECK(islands[1].range.num_edges == 3);
    CHECK(islands[1].range.start_point_x == flip_x_if_necessary(winding, ScalarDeg1(-1.28)));
    CHECK(islands[1].range.end_point_x == flip_x_if_necessary(winding, ScalarDeg1(1.24)));

    CHECK(islands[2].contact_points.begin() == contact_points.data() + 11);
    CHECK(islands[2].contact_points.end() == contact_points.data() + 14);
    CHECK(islands[2].range.first_edge_index == 5);
    CHECK(islands[2].range.num_edges == 2);
    CHECK(islands[2].range.start_point_x == flip_x_if_necessary(winding, ScalarDeg1(1.24)));
    CHECK(islands[2].range.end_point_x == flip_x_if_necessary(winding, ScalarDeg1(2.64)));
  }

  SECTION("Split at multiple locations, needs recursion")
  {
    std::vector<Point2> vertices_storage{
        {1.52, 1.80}, {6.12, 2.14}, {9.18, 5.06}, {6.64, 8.32}, {3.70, 4.94}, {5.88, 3.18}, {7.60, 4.68},
        {6.44, 6.50}, {5.00, 5.20}, {6.24, 4.06}, {4.42, 5.04}, {6.48, 7.28}, {8.34, 4.92}, {5.96, 2.80},
    };
    VerticesView vertices(vertices_storage);

    std::vector<Node> nodes(8);
    nodes[0].direction = HorizontalDirection::right;
    nodes[0].type = NodeType::branch;
    nodes[0].vertex_it = vertices.begin();
    nodes[0].lower_opp_edge = Edge::invalid();
    nodes[0].upper_opp_edge = Edge::invalid();
    nodes[0].neighbors[0] = nullptr;
    nodes[0].neighbors[1] = nullptr;
    nodes[0].neighbors[2] = &nodes[1];

    nodes[1].direction = HorizontalDirection::left;
    nodes[1].type = NodeType::outer_branch;
    nodes[1].vertex_it = vertices.begin() + 9;
    nodes[1].lower_opp_edge = Edge::edge_from_index(vertices, 1);
    nodes[1].upper_opp_edge = Edge::edge_from_index(vertices, 3);
    nodes[1].neighbors[0] = &nodes[2];
    nodes[1].neighbors[1] = &nodes[0];
    nodes[1].neighbors[2] = &nodes[3];

    nodes[2].direction = HorizontalDirection::right;
    nodes[2].type = NodeType::leaf;
    nodes[2].vertex_it = vertices.begin() + 2;
    nodes[2].lower_opp_edge = Edge::edge_from_index(vertices, 1);
    nodes[2].upper_opp_edge = Edge::edge_from_index(vertices, 2);
    nodes[2].neighbors[0] = &nodes[1];

    nodes[3].direction = HorizontalDirection::right;
    nodes[3].type = NodeType::outer_branch;
    nodes[3].vertex_it = vertices.begin() + 9;
    nodes[3].lower_opp_edge = Edge::edge_from_index(vertices, 5);
    nodes[3].upper_opp_edge = Edge::edge_from_index(vertices, 3);
    nodes[3].neighbors[0] = &nodes[4];
    nodes[3].neighbors[1] = &nodes[5];
    nodes[3].neighbors[2] = &nodes[1];

    nodes[4].direction = HorizontalDirection::left;
    nodes[4].type = NodeType::leaf;
    nodes[4].vertex_it = vertices.begin() + 4;
    nodes[4].lower_opp_edge = Edge::edge_from_index(vertices, 4);
    nodes[4].upper_opp_edge = Edge::edge_from_index(vertices, 3);
    nodes[4].neighbors[0] = &nodes[3];

    nodes[5].direction = HorizontalDirection::left;
    nodes[5].type = NodeType::branch;
    nodes[5].vertex_it = vertices.begin() + 9;
    nodes[5].lower_opp_edge = Edge::edge_from_index(vertices, 5);
    nodes[5].upper_opp_edge = Edge::edge_from_index(vertices, 7);
    nodes[5].neighbors[0] = &nodes[6];
    nodes[5].neighbors[1] = &nodes[3];
    nodes[5].neighbors[2] = &nodes[7];

    nodes[6].direction = HorizontalDirection::right;
    nodes[6].type = NodeType::leaf;
    nodes[6].vertex_it = vertices.begin() + 6;
    nodes[6].lower_opp_edge = Edge::edge_from_index(vertices, 5);
    nodes[6].upper_opp_edge = Edge::edge_from_index(vertices, 6);
    nodes[6].neighbors[0] = &nodes[5];

    nodes[7].direction = HorizontalDirection::left;
    nodes[7].type = NodeType::leaf;
    nodes[7].vertex_it = vertices.begin() + 8;
    nodes[7].lower_opp_edge = Edge::edge_from_index(vertices, 8);
    nodes[7].upper_opp_edge = Edge::edge_from_index(vertices, 7);
    nodes[7].neighbors[0] = &nodes[5];

    ChainDecomposition chain_decomposition{&nodes[0], &nodes[5]};

    Winding winding = GENERATE(Winding::ccw, Winding::cw);
    if (winding == Winding::cw)
    {
      flip_horizontally(vertices_storage, nodes);
    }

    std::vector<VerticalExtensionContactPoint> contact_points =
        vertical_extension_contact_points(chain_decomposition, winding);
    REQUIRE(contact_points.size() == 12);

    std::vector<ChainDecompositionIsland> islands =
        split_chain_decomposition_into_islands(vertices, winding, chain_decomposition, contact_points);

    REQUIRE(islands.size() == 3);

    CHECK(islands[0].contact_points.begin() == contact_points.data() + 2);
    CHECK(islands[0].contact_points.end() == contact_points.data() + 3);
    CHECK(islands[0].range.first_edge_index == 1);
    CHECK(islands[0].range.num_edges == 3);
    CHECK(islands[0].range.start_point_x == flip_x_if_necessary(winding, ScalarDeg1(6.24)));
    CHECK(islands[0].range.end_point_x == flip_x_if_necessary(winding, ScalarDeg1(6.24)));

    CHECK(islands[1].contact_points.begin() == contact_points.data() + 5);
    CHECK(islands[1].contact_points.end() == contact_points.data() + 6);
    CHECK(islands[1].range.first_edge_index == 3);
    CHECK(islands[1].range.num_edges == 3);
    CHECK(islands[1].range.start_point_x == flip_x_if_necessary(winding, ScalarDeg1(6.24)));
    CHECK(islands[1].range.end_point_x == flip_x_if_necessary(winding, ScalarDeg1(6.24)));

    CHECK(islands[2].contact_points.begin() == contact_points.data() + 8);
    CHECK(islands[2].contact_points.end() == contact_points.data() + 12);
    CHECK(islands[2].range.first_edge_index == 5);
    CHECK(islands[2].range.num_edges == 4);
    CHECK(islands[2].range.start_point_x == flip_x_if_necessary(winding, ScalarDeg1(6.24)));
    CHECK(islands[2].range.end_point_x == flip_x_if_necessary(winding, ScalarDeg1(6.24)));
  }
}

} // namespace dida::detail::vertical_decomposition