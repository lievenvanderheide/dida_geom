#include "dida/detail/vertical_decomposition/tests/test_utils.hpp"

#include <catch2/catch_test_macros.hpp>

#include "dida/detail/vertical_decomposition/sweep_line_builder.hpp"
#include "dida/polygon2.hpp"

namespace dida::detail::vertical_decomposition
{

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
  Polygon2 polygon{
      {-2.41, 3.78}, {-4.93, 1.68}, {1.45, -0.08}, {-2.47, -0.96},
      {1.81, -2.16}, {5.21, -1.82}, {2.65, 2.3},   {-3.69, 1.76},
  };

  ArrayView<const Point2> vertices(polygon);

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
    Edge edge = ray_cast_down(vertices, PolygonRange{2, 4, vertices[2].x(), vertices[6].x()}, {0.42, -1.22});
    REQUIRE(edge.is_valid());
    CHECK(*edge.start_vertex_it == Point2(-2.47, -0.96));
    CHECK(*edge.end_vertex_it == Point2(1.81, -2.16));
  }

  SECTION("Open range, ignore closing edge")
  {
    Edge edge = ray_cast_down(vertices, PolygonRange{2, 5, vertices[2].x(), vertices[7].x()}, {-0.55, 1.58});
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
    Edge edge = ray_cast_down(vertices, PolygonRange{2, 4, ScalarDeg1(-2), ScalarDeg1(3.5)}, {-1.20, -0.24});
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
    Edge edge = ray_cast_down(vertices, PolygonRange{1, 6, ScalarDeg1(-2.5), ScalarDeg1(1.8)}, {0.36, 3.30});
    REQUIRE(edge.is_valid());
    CHECK(*edge.start_vertex_it == Point2(-4.93, 1.68));
    CHECK(*edge.end_vertex_it == Point2(1.45, -0.08));
  }

  SECTION("Open range, with wrap")
  {
    Edge edge = ray_cast_down(vertices, PolygonRange{6, 4, vertices[6].x(), vertices[2].x()}, {-4.26, 1.94});
    REQUIRE(edge.is_valid());
    CHECK(*edge.start_vertex_it == Point2(-4.93, 1.68));
    CHECK(*edge.end_vertex_it == Point2(1.45, -0.08));
  }
}

TEST_CASE("gather_nodes")
{
  auto check_node_set = [](const std::set<const Node*>& node_set, const std::vector<Node>& expected_nodes)
  {
    REQUIRE(node_set.size() == expected_nodes.size());

    size_t i = 0;
    for (const Node* node : node_set)
    {
      CHECK(node == &expected_nodes[i]);
      i++;
    }
  };

  SECTION("Tree like")
  {
    Polygon2 polygon{{-3.56, 1.78}, {0.86, 2.64}, {-1.08, 4.36}, {3.56, 6.18}, {-3.54, 5.70}, {-0.84, 2.98}};
    VerticesView vertices(polygon);

    std::vector<Node> nodes(6);
    nodes[0].direction = HorizontalDirection::left;
    nodes[0].type = NodeType::leaf;
    nodes[0].vertex_it = vertices.begin() + 0;
    nodes[0].lower_opp_edge = Edge::edge_from_index(vertices, 0);
    nodes[0].upper_opp_edge = Edge::edge_from_index(vertices, 5);
    nodes[0].neighbors[0] = &nodes[3];

    nodes[1].direction = HorizontalDirection::left;
    nodes[1].type = NodeType::leaf;
    nodes[1].vertex_it = vertices.begin() + 4;
    nodes[1].lower_opp_edge = Edge::edge_from_index(vertices, 4);
    nodes[1].upper_opp_edge = Edge::edge_from_index(vertices, 3);
    nodes[1].neighbors[0] = &nodes[2];

    nodes[2].direction = HorizontalDirection::right;
    nodes[2].type = NodeType::branch;
    nodes[2].vertex_it = vertices.begin() + 2;
    nodes[2].lower_opp_edge = Edge::edge_from_index(vertices, 4);
    nodes[2].upper_opp_edge = Edge::edge_from_index(vertices, 3);
    nodes[2].neighbors[0] = &nodes[1];
    nodes[2].neighbors[1] = &nodes[3];
    nodes[2].neighbors[2] = &nodes[5];

    nodes[3].direction = HorizontalDirection::left;
    nodes[3].type = NodeType::branch;
    nodes[3].vertex_it = vertices.begin() + 5;
    nodes[3].lower_opp_edge = Edge::edge_from_index(vertices, 0);
    nodes[3].upper_opp_edge = Edge::edge_from_index(vertices, 1);
    nodes[3].neighbors[0] = &nodes[4];
    nodes[3].neighbors[1] = &nodes[0];
    nodes[3].neighbors[2] = &nodes[2];

    nodes[4].direction = HorizontalDirection::right;
    nodes[4].type = NodeType::leaf;
    nodes[4].vertex_it = vertices.begin() + 1;
    nodes[4].lower_opp_edge = Edge::edge_from_index(vertices, 0);
    nodes[4].upper_opp_edge = Edge::edge_from_index(vertices, 1);
    nodes[4].neighbors[0] = &nodes[3];

    nodes[5].direction = HorizontalDirection::right;
    nodes[5].type = NodeType::leaf;
    nodes[5].vertex_it = vertices.begin() + 3;
    nodes[5].lower_opp_edge = Edge::edge_from_index(vertices, 2);
    nodes[5].upper_opp_edge = Edge::edge_from_index(vertices, 3);
    nodes[5].neighbors[0] = &nodes[2];

    SECTION("Start at normal node")
    {
      std::set<const Node*> result = gather_nodes(&nodes[2]);
      check_node_set(result, nodes);
    }

    SECTION("Start at leaf node")
    {
      std::set<const Node*> result = gather_nodes(&nodes[0]);
      check_node_set(result, nodes);
    }
  }

  SECTION("With loops")
  {
    Polygon2 polygon{
        {0.36, 2.36}, {2.54, 4.18}, {1.44, 5.04}, {2.38, 7.48}, {-2.62, 7.34}, {-0.28, 5.26}, {-1.86, 3.22},
    };
    VerticesView vertices(polygon);

    std::vector<Node> nodes(6);
    nodes[0].direction = HorizontalDirection::right;
    nodes[0].type = NodeType::branch;
    nodes[0].vertex_it = vertices.begin() + 4;
    nodes[0].lower_opp_edge = Edge::invalid();
    nodes[0].upper_opp_edge = Edge::invalid();
    nodes[0].neighbors[0] = nullptr;
    nodes[0].neighbors[1] = &nodes[1];
    nodes[0].neighbors[2] = &nodes[4];

    nodes[1].direction = HorizontalDirection::right;
    nodes[1].type = NodeType::branch;
    nodes[1].vertex_it = vertices.begin() + 6;
    nodes[1].lower_opp_edge = Edge::invalid();
    nodes[1].upper_opp_edge = Edge::edge_from_index(vertices, 4);
    nodes[1].neighbors[0] = &nodes[0];
    nodes[1].neighbors[1] = &nodes[5];
    nodes[1].neighbors[2] = &nodes[2];

    nodes[2].direction = HorizontalDirection::right;
    nodes[2].type = NodeType::leaf;
    nodes[2].vertex_it = vertices.begin() + 5;
    nodes[2].lower_opp_edge = Edge::edge_from_index(vertices, 5);
    nodes[2].upper_opp_edge = Edge::edge_from_index(vertices, 4);
    nodes[2].neighbors[0] = &nodes[1];

    nodes[3].direction = HorizontalDirection::left;
    nodes[3].type = NodeType::leaf;
    nodes[3].vertex_it = vertices.begin() + 2;
    nodes[3].lower_opp_edge = Edge::edge_from_index(vertices, 1);
    nodes[3].upper_opp_edge = Edge::edge_from_index(vertices, 2);
    nodes[3].neighbors[0] = &nodes[4];

    nodes[4].direction = HorizontalDirection::left;
    nodes[4].type = NodeType::branch;
    nodes[4].vertex_it = vertices.begin() + 3;
    nodes[4].lower_opp_edge = Edge::edge_from_index(vertices, 1);
    nodes[4].upper_opp_edge = Edge::invalid();
    nodes[4].neighbors[0] = &nodes[5];
    nodes[4].neighbors[1] = &nodes[3];
    nodes[4].neighbors[2] = &nodes[0];

    nodes[5].direction = HorizontalDirection::left;
    nodes[5].type = NodeType::branch;
    nodes[5].vertex_it = vertices.begin() + 1;
    nodes[5].lower_opp_edge = Edge::invalid();
    nodes[5].upper_opp_edge = Edge::invalid();
    nodes[5].neighbors[0] = nullptr;
    nodes[5].neighbors[1] = &nodes[1];
    nodes[5].neighbors[2] = &nodes[4];

    SECTION("Start at normal node")
    {
      std::set<const Node*> result = gather_nodes(&nodes[0]);
      check_node_set(result, nodes);
    }

    SECTION("Start at leaf node")
    {
      std::set<const Node*> result = gather_nodes(&nodes[2]);
      check_node_set(result, nodes);
    }
  }
}

TEST_CASE("vertical_extension_contact_points")
{
  SECTION("Chain 1")
  {
    Polygon2 polygon{
        {-3.00, 2.64}, {-0.30, 3.64}, {-3.12, 5.62}, {-5.92, 3.10}, {-2.76, 0.64}, {0.78, 2.34}, {-0.02, 0.62},
        {2.88, 0.70},  {2.06, 2.08},  {5.34, 1.70},  {6.74, 3.64},  {4.40, 6.00},  {3.20, 4.00}, {4.68, 3.20},
    };

    VerticesView vertices(polygon);

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

    ChainDecomposition chain_decomposition{&nodes[1], &nodes[8]};

    std::vector<VerticalExtensionContactPoint> contact_points = vertical_extension_contact_points(chain_decomposition);

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

    Polygon2 polygon{
        {3.00, -2.64},  {0.30, -3.64},  {3.12, -5.62},  {5.92, -3.10},  {2.76, -0.64},  {-0.78, -2.34}, {0.02, -0.62},
        {-2.88, -0.70}, {-2.06, -2.08}, {-5.34, -1.70}, {-6.74, -3.64}, {-4.40, -6.00}, {-3.20, -4.00}, {-4.68, -3.20},
    };

    VerticesView vertices(polygon);

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

    std::vector<VerticalExtensionContactPoint> contact_points = vertical_extension_contact_points(chain_decomposition);

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
  SECTION("Split at contact point of type 'vertex_downwards'")
  {
    Polygon2 polygon{
        {2.40, 4.74},  {4.06, 5.68},  {-1.22, 6.18}, {-0.06, 5.00},
        {-3.96, 4.14}, {-2.34, 3.46}, {-2.98, 2.74}, {3.48, 3.94},
    };
    VerticesView vertices(polygon);

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

    std::vector<VerticalExtensionContactPoint> contact_points = vertical_extension_contact_points(chain_decomposition);
    REQUIRE(contact_points.size() == 10);

    std::vector<ChainDecompositionIsland> islands =
        split_chain_decomposition_into_islands(vertices, chain_decomposition, contact_points);

    REQUIRE(islands.size() == 2);

    CHECK(islands[0].contact_points.begin() == contact_points.data());
    CHECK(islands[0].contact_points.end() == contact_points.data() + 6);
    CHECK(islands[0].range.first_edge_index == 0);
    CHECK(islands[0].range.num_edges == 3);
    CHECK(islands[0].range.start_point_x == ScalarDeg1(2.40));
    CHECK(islands[0].range.end_point_x == ScalarDeg1(-0.06));

    CHECK(islands[1].contact_points.begin() == contact_points.data() + 7);
    CHECK(islands[1].contact_points.end() == contact_points.data() + 10);
    CHECK(islands[1].range.first_edge_index == 3);
    CHECK(islands[1].range.num_edges == 2);
    CHECK(islands[1].range.start_point_x == ScalarDeg1(-0.06));
    CHECK(islands[1].range.end_point_x == ScalarDeg1(-2.34));
  }

  SECTION("Split at contact point of type 'vertex_upwards'")
  {
    Polygon2 polygon{
        {-1.20, 4.86}, {-2.50, 4.24}, {0.56, 2.96}, {-0.24, 2.16},
        {2.66, 2.28},  {2.04, 3.26},  {3.44, 4.68}, {-1.92, 5.72},
    };
    VerticesView vertices(polygon);

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

    std::vector<VerticalExtensionContactPoint> contact_points = vertical_extension_contact_points(chain_decomposition);
    REQUIRE(contact_points.size() == 10);

    std::vector<ChainDecompositionIsland> islands =
        split_chain_decomposition_into_islands(vertices, chain_decomposition, contact_points);

    REQUIRE(islands.size() == 2);

    CHECK(islands[0].contact_points.begin() == contact_points.data());
    CHECK(islands[0].contact_points.end() == contact_points.data() + 3);
    CHECK(islands[0].range.first_edge_index == 0);
    CHECK(islands[0].range.num_edges == 2);
    CHECK(islands[0].range.start_point_x == ScalarDeg1(-1.20));
    CHECK(islands[0].range.end_point_x == ScalarDeg1(0.56));

    CHECK(islands[1].contact_points.begin() == contact_points.data() + 4);
    CHECK(islands[1].contact_points.end() == contact_points.data() + 10);
    CHECK(islands[1].range.first_edge_index == 2);
    CHECK(islands[1].range.num_edges == 3);
    CHECK(islands[1].range.start_point_x == ScalarDeg1(0.56));
    CHECK(islands[1].range.end_point_x == ScalarDeg1(2.04));
  }

  SECTION("Split at contact point of type 'lower_opp_edge'")
  {
    Polygon2 polygon{
        {-5.10, 3.10},  {-3.62, 4.28}, {-4.80, 6.04}, {-8.00, 3.60}, {-5.68, -0.34},
        {-1.84, -0.34}, {0.76, 1.74},  {-1.62, 2.64}, {-1.00, 3.22},
    };

    VerticesView vertices(polygon);

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

    std::vector<VerticalExtensionContactPoint> contact_points = vertical_extension_contact_points(chain_decomposition);
    REQUIRE(contact_points.size() == 8);

    std::vector<ChainDecompositionIsland> islands =
        split_chain_decomposition_into_islands(vertices, chain_decomposition, contact_points);

    REQUIRE(islands.size() == 2);

    CHECK(islands[0].contact_points.begin() == contact_points.data());
    CHECK(islands[0].contact_points.end() == contact_points.data() + 4);
    CHECK(islands[0].range.first_edge_index == 0);
    CHECK(islands[0].range.num_edges == 5);
    CHECK(islands[0].range.start_point_x == ScalarDeg1(-5.10));
    CHECK(islands[0].range.end_point_x == ScalarDeg1(-5.10));

    CHECK(islands[1].contact_points.begin() == contact_points.data() + 5);
    CHECK(islands[1].contact_points.end() == contact_points.data() + 8);
    CHECK(islands[1].range.first_edge_index == 4);
    CHECK(islands[1].range.num_edges == 3);
    CHECK(islands[1].range.start_point_x == ScalarDeg1(-5.10));
    CHECK(islands[1].range.end_point_x == ScalarDeg1(-1.62));
  }

  SECTION("Split at contact point of type 'upper_opp_edge'")
  {
    Polygon2 polygon{
        {4.28, 6.00}, {2.54, 5.00},  {3.38, 3.68}, {7.64, 5.94},
        {4.06, 9.02}, {-0.26, 7.54}, {0.56, 6.62}, {-1.74, 5.66},
    };
    VerticesView vertices(polygon);

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

    std::vector<VerticalExtensionContactPoint> contact_points = vertical_extension_contact_points(chain_decomposition);
    REQUIRE(contact_points.size() == 10);

    std::vector<ChainDecompositionIsland> islands =
        split_chain_decomposition_into_islands(vertices, chain_decomposition, contact_points);

    REQUIRE(islands.size() == 2);

    CHECK(islands[0].contact_points.begin() == contact_points.data());
    CHECK(islands[0].contact_points.end() == contact_points.data() + 4);
    CHECK(islands[0].range.first_edge_index == 0);
    CHECK(islands[0].range.num_edges == 4);
    CHECK(islands[0].range.start_point_x == ScalarDeg1(4.28));
    CHECK(islands[0].range.end_point_x == ScalarDeg1(4.28));

    CHECK(islands[1].contact_points.begin() == contact_points.data() + 5);
    CHECK(islands[1].contact_points.end() == contact_points.data() + 8);
    CHECK(islands[1].range.first_edge_index == 3);
    CHECK(islands[1].range.num_edges == 3);
    CHECK(islands[1].range.start_point_x == ScalarDeg1(4.28));
    CHECK(islands[1].range.end_point_x == ScalarDeg1(0.56));
  }

  SECTION("Infinite vertical extensions which don't agree with ray-cast result")
  {
    SECTION("Contact point on upper_opp_edge")
    {
    }

    SECTION("Contact point on upper_opp_edge")
    {
      Polygon2 polygon{
          {4.88, 8.22}, {3.34, 7.14}, {6.26, 5.08}, {10.54, 7.50}, {4.08, 9.62}, {5.30, 8.76}, {4.26, 8.50},
      };
      VerticesView vertices(polygon);

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

      std::vector<VerticalExtensionContactPoint> contact_points =
          vertical_extension_contact_points(chain_decomposition);
      REQUIRE(contact_points.size() == 11);

      std::vector<ChainDecompositionIsland> islands =
          split_chain_decomposition_into_islands(vertices, chain_decomposition, contact_points);

      REQUIRE(islands.size() == 1);
      CHECK(islands[0].contact_points.begin() == contact_points.data());
      CHECK(islands[0].contact_points.end() == contact_points.data() + 10);
      CHECK(islands[0].range.first_edge_index == 0);
      CHECK(islands[0].range.num_edges == 6);
      CHECK(islands[0].range.start_point_x == ScalarDeg1(4.88));
      CHECK(islands[0].range.end_point_x == ScalarDeg1(4.26));
    }

    SECTION("Contact points on vertices")
    {
      Polygon2 polygon{
          {-2.50, 4.54}, {-3.14, 3.68}, {0.58, 3.86},  {-0.30, 3.00}, {4.10, 4.66},
          {0.88, 5.28},  {1.84, 6.30},  {-0.08, 5.20}, {-3.62, 5.96},
      };
      VerticesView vertices(polygon);

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

      std::vector<VerticalExtensionContactPoint> contact_points =
          vertical_extension_contact_points(chain_decomposition);
      REQUIRE(contact_points.size() == 14);

      std::vector<ChainDecompositionIsland> islands =
          split_chain_decomposition_into_islands(vertices, chain_decomposition, contact_points);

      REQUIRE(islands.size() == 1);
      CHECK(islands[0].contact_points.begin() == contact_points.data());
      CHECK(islands[0].contact_points.end() == contact_points.data() + 12);
    }
  }

  SECTION("Split at multiple locations, in one pass")
  {
    Polygon2 polygon{
        {-3.60, 3.22}, {-5.20, 2.88}, {-1.28, 0.98}, {-2.52, 0.12}, {2.54, 0.30},
        {1.24, 1.12},  {5.10, 2.92},  {2.64, 3.66},  {3.32, 4.42},  {-4.18, 4.28},
    };
    VerticesView vertices(polygon);

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

    std::vector<VerticalExtensionContactPoint> contact_points = vertical_extension_contact_points(chain_decomposition);
    REQUIRE(contact_points.size() == 14);

    std::vector<ChainDecompositionIsland> islands =
        split_chain_decomposition_into_islands(vertices, chain_decomposition, contact_points);

    REQUIRE(islands.size() == 3);

    CHECK(islands[0].contact_points.begin() == contact_points.data());
    CHECK(islands[0].contact_points.end() == contact_points.data() + 3);
    CHECK(islands[0].range.first_edge_index == 0);
    CHECK(islands[0].range.num_edges == 2);
    CHECK(islands[0].range.start_point_x == ScalarDeg1(-3.60));
    CHECK(islands[0].range.end_point_x == ScalarDeg1(-1.28));

    CHECK(islands[1].contact_points.begin() == contact_points.data() + 4);
    CHECK(islands[1].contact_points.end() == contact_points.data() + 10);
    CHECK(islands[1].range.first_edge_index == 2);
    CHECK(islands[1].range.num_edges == 3);
    CHECK(islands[1].range.start_point_x == ScalarDeg1(-1.28));
    CHECK(islands[1].range.end_point_x == ScalarDeg1(1.24));

    CHECK(islands[2].contact_points.begin() == contact_points.data() + 11);
    CHECK(islands[2].contact_points.end() == contact_points.data() + 14);
    CHECK(islands[2].range.first_edge_index == 5);
    CHECK(islands[2].range.num_edges == 2);
    CHECK(islands[2].range.start_point_x == ScalarDeg1(1.24));
    CHECK(islands[2].range.end_point_x == ScalarDeg1(2.64));
  }

  SECTION("Split at multiple locations, needs recursion")
  {
    Polygon2 polygon{
        {1.52, 1.80}, {6.12, 2.14}, {9.18, 5.06}, {6.64, 8.32}, {3.70, 4.94}, {5.88, 3.18}, {7.60, 4.68},
        {6.44, 6.50}, {5.00, 5.20}, {6.24, 4.06}, {4.42, 5.04}, {6.48, 7.28}, {8.34, 4.92}, {5.96, 2.80},
    };
    VerticesView vertices(polygon);

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

    std::vector<VerticalExtensionContactPoint> contact_points = vertical_extension_contact_points(chain_decomposition);
    REQUIRE(contact_points.size() == 12);

    std::vector<ChainDecompositionIsland> islands =
        split_chain_decomposition_into_islands(vertices, chain_decomposition, contact_points);

    REQUIRE(islands.size() == 3);

    CHECK(islands[0].contact_points.begin() == contact_points.data() + 2);
    CHECK(islands[0].contact_points.end() == contact_points.data() + 3);
    CHECK(islands[0].range.first_edge_index == 1);
    CHECK(islands[0].range.num_edges == 3);
    CHECK(islands[0].range.start_point_x == ScalarDeg1(6.24));
    CHECK(islands[0].range.end_point_x == ScalarDeg1(6.24));

    CHECK(islands[1].contact_points.begin() == contact_points.data() + 5);
    CHECK(islands[1].contact_points.end() == contact_points.data() + 6);
    CHECK(islands[1].range.first_edge_index == 3);
    CHECK(islands[1].range.num_edges == 3);
    CHECK(islands[1].range.start_point_x == ScalarDeg1(6.24));
    CHECK(islands[1].range.end_point_x == ScalarDeg1(6.24));

    CHECK(islands[2].contact_points.begin() == contact_points.data() + 8);
    CHECK(islands[2].contact_points.end() == contact_points.data() + 12);
    CHECK(islands[2].range.first_edge_index == 5);
    CHECK(islands[2].range.num_edges == 4);
    CHECK(islands[2].range.start_point_x == ScalarDeg1(6.24));
    CHECK(islands[2].range.end_point_x == ScalarDeg1(6.24));
  }
}

TEST_CASE("validate_node_opp_edges")
{
  Polygon2 polygon{
      {-5.14, 3.94}, {-3.98, 1.78}, {-2.54, 0.90}, {-3.10, 2.32}, {-1.54, 1.22}, {0.36, 1.44},
      {1.52, 0.62},  {3.66, 2.08},  {3.14, 0.90},  {4.82, 2.00},  {6.08, 4.30},  {4.22, 5.10},
      {1.76, 2.84},  {0.28, 3.38},  {-1.14, 2.70}, {-2.88, 3.86},
  };
  VerticesView vertices(polygon);

  PolygonRange full_range{0, polygon.size(), polygon[0].x(), polygon[0].x()};

  SECTION("Non-leaf node")
  {
    Node node;
    node.direction == HorizontalDirection::right;
    node.type = NodeType::branch;
    node.vertex_it = vertices.begin() + 3;
    node.lower_opp_edge = Edge::edge_from_index(vertices, 1);
    node.upper_opp_edge = Edge::edge_from_index(vertices, 15);
    node.neighbors[0] = nullptr;
    node.neighbors[1] = nullptr;
    node.neighbors[2] = nullptr;

    PolygonRange no_lower_opp_edge_range{5, 12, ScalarDeg1(1.1), ScalarDeg1(-4.5)};
    PolygonRange no_upper_opp_edge_range{1, 8, ScalarDeg1(-3.72), ScalarDeg1(4.36)};

    SECTION("Valid")
    {
      CHECK(validate_node_opp_edges(vertices, full_range, &node));
    }

    SECTION("lower_opp_edge invalid")
    {
      node.lower_opp_edge = Edge::edge_from_index(vertices, 2);
      CHECK_FALSE(validate_node_opp_edges(vertices, full_range, &node));
    }

    SECTION("upper_opp_edge invalid")
    {
      node.upper_opp_edge = Edge::edge_from_index(vertices, 14);
      CHECK_FALSE(validate_node_opp_edges(vertices, full_range, &node));
    }

    SECTION("Valid, no lower_opp_edge")
    {
      node.lower_opp_edge = Edge::invalid();
      CHECK(validate_node_opp_edges(vertices, no_lower_opp_edge_range, &node));
    }

    SECTION("Valid no upper_opp_edge")
    {
      node.upper_opp_edge = Edge::invalid();
      CHECK(validate_node_opp_edges(vertices, no_upper_opp_edge_range, &node));
    }
  }

  SECTION("Leaf node")
  {
    SECTION("Left leaf")
    {
      Node node;
      node.direction = HorizontalDirection::left;
      node.type = NodeType::leaf;
      node.vertex_it = vertices.begin();
      node.lower_opp_edge = Edge::edge_from_index(vertices, 0);
      node.upper_opp_edge = Edge::edge_from_index(vertices, 15);
      node.neighbors[0] = nullptr;
      node.neighbors[1] = nullptr;
      node.neighbors[2] = nullptr;

      SECTION("Valid")
      {
        CHECK(validate_node_opp_edges(vertices, full_range, &node));
      }

      SECTION("lower_opp_edge different")
      {
        node.lower_opp_edge = Edge::edge_from_index(vertices, 1);
        CHECK_FALSE(validate_node_opp_edges(vertices, full_range, &node));
      }

      SECTION("upper_opp_edge different")
      {
        node.upper_opp_edge = Edge::edge_from_index(vertices, 1);
        CHECK_FALSE(validate_node_opp_edges(vertices, full_range, &node));
      }
    }

    SECTION("Right leaf")
    {
      Node node;
      node.direction = HorizontalDirection::right;
      node.type = NodeType::leaf;
      node.vertex_it = vertices.begin() + 10;
      node.lower_opp_edge = Edge::edge_from_index(vertices, 9);
      node.upper_opp_edge = Edge::edge_from_index(vertices, 10);
      node.neighbors[0] = nullptr;
      node.neighbors[1] = nullptr;
      node.neighbors[2] = nullptr;

      SECTION("Valid")
      {
        CHECK(validate_node_opp_edges(vertices, full_range, &node));
      }

      SECTION("lower_opp_edge different")
      {
        node.lower_opp_edge = Edge::edge_from_index(vertices, 11);
        CHECK_FALSE(validate_node_opp_edges(vertices, full_range, &node));
      }

      SECTION("upper_opp_edge different")
      {
        node.upper_opp_edge = Edge::edge_from_index(vertices, 11);
        CHECK_FALSE(validate_node_opp_edges(vertices, full_range, &node));
      }
    }
  }
}

TEST_CASE("node_branch_boundary_vertices")
{
  SECTION("Node towards left")
  {
    Polygon2 polygon{
        {-4.48, 0.66}, {-2.42, 2.48}, {2.14, 1.86}, {5.92, 3.28}, {5.06, 5.14}, {1.16, 6.80},  {-1.66, 7.48},
        {-3.96, 6.54}, {-1.30, 5.66}, {1.08, 5.74}, {2.38, 4.48}, {0.90, 3.60}, {-1.04, 4.34}, {-3.04, 3.46},
    };
    VerticesView vertices(polygon);

    Node node;
    node.direction = HorizontalDirection::left;
    node.type = NodeType::branch;
    node.vertex_it = vertices.begin() + 10;
    node.lower_opp_edge = Edge::edge_from_index(vertices, 2);
    node.upper_opp_edge = Edge::edge_from_index(vertices, 4);
    node.neighbors[0] = nullptr;
    node.neighbors[1] = nullptr;
    node.neighbors[2] = nullptr;

    SECTION("Branch 0")
    {
      NodeBranchBoundaryVertices result = node_branch_boundary_vertices(ChainDecomposition{nullptr, nullptr}, &node, 0);
      CHECK(result.lower_boundary_vertex_it == vertices.begin() + 2);
      CHECK(result.upper_boundary_vertex_it == vertices.begin() + 5);
    }

    SECTION("Branch 1")
    {
      NodeBranchBoundaryVertices result = node_branch_boundary_vertices(ChainDecomposition{nullptr, nullptr}, &node, 1);
      CHECK(result.lower_boundary_vertex_it == vertices.begin() + 3);
      CHECK(result.upper_boundary_vertex_it == vertices.begin() + 10);
    }

    SECTION("Branch 2")
    {
      NodeBranchBoundaryVertices result = node_branch_boundary_vertices(ChainDecomposition{nullptr, nullptr}, &node, 2);
      CHECK(result.lower_boundary_vertex_it == vertices.begin() + 10);
      CHECK(result.upper_boundary_vertex_it == vertices.begin() + 4);
    }

    SECTION("Node is chain fist node, branch 1")
    {
      NodeBranchBoundaryVertices result = node_branch_boundary_vertices(ChainDecomposition{&node, nullptr}, &node, 1);
      CHECK(result.lower_boundary_vertex_it == vertices.begin() + 3);
      CHECK(result.upper_boundary_vertex_it == vertices.begin() + 10);
    }

    SECTION("Node is chain first node, branch 2")
    {
      NodeBranchBoundaryVertices result = node_branch_boundary_vertices(ChainDecomposition{&node, nullptr}, &node, 2);
      CHECK(result.lower_boundary_vertex_it == nullptr);
      CHECK(result.upper_boundary_vertex_it == vertices.begin() + 4);
    }

    SECTION("Node is chain last node, branch 1")
    {
      NodeBranchBoundaryVertices result = node_branch_boundary_vertices(ChainDecomposition{nullptr, &node}, &node, 1);
      CHECK(result.lower_boundary_vertex_it == vertices.begin() + 3);
      CHECK(result.upper_boundary_vertex_it == nullptr);
    }

    SECTION("Node is chain last node, branch 2")
    {
      NodeBranchBoundaryVertices result = node_branch_boundary_vertices(ChainDecomposition{nullptr, &node}, &node, 2);
      CHECK(result.lower_boundary_vertex_it == vertices.begin() + 10);
      CHECK(result.upper_boundary_vertex_it == vertices.begin() + 4);
    }
  }

  SECTION("Node towards right")
  {
    Polygon2 polygon{
        {-4.64, 2.18}, {-0.92, 2.84}, {1.36, 1.44}, {6.34, 0.32}, {4.24, 2.96},  {0.70, 4.32},
        {3.88, 6.22},  {5.10, 6.12},  {3.48, 7.76}, {1.92, 6.46}, {-0.72, 7.66}, {-4.28, 5.32},
    };
    VerticesView vertices(polygon);

    Node node;
    node.direction = HorizontalDirection::right;
    node.type = NodeType::branch;
    node.vertex_it = vertices.begin() + 5;
    node.lower_opp_edge = Edge::edge_from_index(vertices, 1);
    node.upper_opp_edge = Edge::edge_from_index(vertices, 9);
    node.neighbors[0] = nullptr;
    node.neighbors[1] = nullptr;
    node.neighbors[2] = nullptr;

    SECTION("Branch 0")
    {
      NodeBranchBoundaryVertices result = node_branch_boundary_vertices(ChainDecomposition{nullptr, nullptr}, &node, 0);
      CHECK(result.lower_boundary_vertex_it == vertices.begin() + 2);
      CHECK(result.upper_boundary_vertex_it == vertices.begin() + 9);
    }

    SECTION("Branch 1")
    {
      NodeBranchBoundaryVertices result = node_branch_boundary_vertices(ChainDecomposition{nullptr, nullptr}, &node, 1);
      CHECK(result.lower_boundary_vertex_it == vertices.begin() + 1);
      CHECK(result.upper_boundary_vertex_it == vertices.begin() + 5);
    }

    SECTION("Branch 2")
    {
      NodeBranchBoundaryVertices result = node_branch_boundary_vertices(ChainDecomposition{nullptr, nullptr}, &node, 2);
      CHECK(result.lower_boundary_vertex_it == vertices.begin() + 5);
      CHECK(result.upper_boundary_vertex_it == vertices.begin() + 10);
    }

    SECTION("Node is chain fist node, branch 1")
    {
      NodeBranchBoundaryVertices result = node_branch_boundary_vertices(ChainDecomposition{&node, nullptr}, &node, 1);
      CHECK(result.lower_boundary_vertex_it == vertices.begin() + 1);
      CHECK(result.upper_boundary_vertex_it == nullptr);
    }

    SECTION("Node is chain first node, branch 2")
    {
      NodeBranchBoundaryVertices result = node_branch_boundary_vertices(ChainDecomposition{&node, nullptr}, &node, 2);
      CHECK(result.lower_boundary_vertex_it == vertices.begin() + 5);
      CHECK(result.upper_boundary_vertex_it == vertices.begin() + 10);
    }

    SECTION("Node is chain last node, branch 1")
    {
      NodeBranchBoundaryVertices result = node_branch_boundary_vertices(ChainDecomposition{nullptr, &node}, &node, 1);
      CHECK(result.lower_boundary_vertex_it == vertices.begin() + 1);
      CHECK(result.upper_boundary_vertex_it == vertices.begin() + 5);
    }

    SECTION("Node is chain last node, branch 2")
    {
      NodeBranchBoundaryVertices result = node_branch_boundary_vertices(ChainDecomposition{nullptr, &node}, &node, 2);
      CHECK(result.lower_boundary_vertex_it == nullptr);
      CHECK(result.upper_boundary_vertex_it == vertices.begin() + 10);
    }
  }

  SECTION("Leaf nodes")
  {
    Polygon2 polygon{
        {-5.26, 1.80}, {-3.14, 1.56}, {-0.74, 2.54}, {1.46, 1.12},
        {3.94, 2.90},  {-0.58, 5.86}, {-2.92, 3.84}, {-4.32, 4.22},
    };
    VerticesView vertices(polygon);

    SECTION("Towards left")
    {
      Node node;
      node.direction = HorizontalDirection::left;
      node.type = NodeType::leaf;
      node.vertex_it = vertices.begin();
      node.lower_opp_edge = Edge::edge_from_index(vertices, 0);
      node.upper_opp_edge = Edge::edge_from_index(vertices, 7);
      node.neighbors[0] = nullptr;
      node.neighbors[1] = nullptr;
      node.neighbors[2] = nullptr;

      NodeBranchBoundaryVertices result = node_branch_boundary_vertices(ChainDecomposition{nullptr, nullptr}, &node, 0);
      CHECK(result.lower_boundary_vertex_it == vertices.begin());
      CHECK(result.upper_boundary_vertex_it == vertices.begin());
    }

    SECTION("Towards right")
    {
      Node node;
      node.direction = HorizontalDirection::right;
      node.type = NodeType::leaf;
      node.vertex_it = vertices.begin() + 4;
      node.lower_opp_edge = Edge::edge_from_index(vertices, 3);
      node.upper_opp_edge = Edge::edge_from_index(vertices, 5);
      node.neighbors[0] = nullptr;
      node.neighbors[1] = nullptr;
      node.neighbors[2] = nullptr;

      NodeBranchBoundaryVertices result = node_branch_boundary_vertices(ChainDecomposition{nullptr, nullptr}, &node, 0);
      CHECK(result.lower_boundary_vertex_it == vertices.begin() + 4);
      CHECK(result.upper_boundary_vertex_it == vertices.begin() + 4);
    }
  }
}

TEST_CASE("validate_node_neighbors")
{
  Polygon2 polygon{
      {-3.12, 5.96}, {-1.92, 4.82}, {-4.46, 3.42}, {-3.00, 2.24}, {-0.18, 2.98}, {0.90, 1.86},
      {3.92, 2.42},  {6.46, 4.36},  {0.42, 8.30},  {1.92, 5.60},  {-0.18, 6.60}, {-0.96, 5.84},
  };
  VerticesView vertices(polygon);

  std::vector<Node> nodes(6);
  nodes[0].direction = HorizontalDirection::left;
  nodes[0].type = NodeType::leaf;
  nodes[0].vertex_it = vertices.begin() + 2;
  nodes[0].lower_opp_edge = Edge::edge_from_index(vertices, 2);
  nodes[0].upper_opp_edge = Edge::edge_from_index(vertices, 1);
  nodes[0].neighbors[0] = &nodes[2];

  nodes[1].direction = HorizontalDirection::left;
  nodes[1].type = NodeType::leaf;
  nodes[1].vertex_it = vertices.begin() + 0;
  nodes[1].lower_opp_edge = Edge::edge_from_index(vertices, 0);
  nodes[1].upper_opp_edge = Edge::edge_from_index(vertices, 11);
  nodes[1].neighbors[0] = &nodes[2];

  nodes[2].direction = HorizontalDirection::left;
  nodes[2].type = NodeType::branch;
  nodes[2].vertex_it = vertices.begin() + 1;
  nodes[2].lower_opp_edge = Edge::edge_from_index(vertices, 3);
  nodes[2].upper_opp_edge = Edge::edge_from_index(vertices, 11);
  nodes[2].neighbors[0] = &nodes[4];
  nodes[2].neighbors[1] = &nodes[0];
  nodes[2].neighbors[2] = &nodes[1];

  nodes[3].direction = HorizontalDirection::left;
  nodes[3].type = NodeType::leaf;
  nodes[3].vertex_it = vertices.begin() + 8;
  nodes[3].lower_opp_edge = Edge::edge_from_index(vertices, 8);
  nodes[3].upper_opp_edge = Edge::edge_from_index(vertices, 7);
  nodes[3].neighbors[0] = &nodes[4];

  nodes[4].direction = HorizontalDirection::left;
  nodes[4].type = NodeType::branch;
  nodes[4].vertex_it = vertices.begin() + 9;
  nodes[4].lower_opp_edge = Edge::edge_from_index(vertices, 5);
  nodes[4].upper_opp_edge = Edge::edge_from_index(vertices, 7);
  nodes[4].neighbors[0] = &nodes[5];
  nodes[4].neighbors[1] = &nodes[2];
  nodes[4].neighbors[2] = &nodes[3];

  nodes[5].direction = HorizontalDirection::right;
  nodes[5].type = NodeType::leaf;
  nodes[5].vertex_it = vertices.begin() + 7;
  nodes[5].lower_opp_edge = Edge::edge_from_index(vertices, 6);
  nodes[5].upper_opp_edge = Edge::edge_from_index(vertices, 7);
  nodes[5].neighbors[0] = &nodes[4];

  SECTION("Valid, with upper and lower boundary")
  {
    CHECK(validate_node_neighbors(vertices, ChainDecomposition{nullptr, nullptr}, &nodes[2]));
  }

  SECTION("Left node has incorrect outgoing direction")
  {
    SECTION("Branch 0")
    {
      nodes[2].direction = HorizontalDirection::right;
      CHECK_FALSE(validate_node_neighbors(vertices, ChainDecomposition{nullptr, nullptr}, &nodes[2]));
    }

    SECTION("Branch 2")
    {
      std::swap(nodes[2].neighbors[0], nodes[2].neighbors[2]);
      CHECK_FALSE(validate_node_neighbors(vertices, ChainDecomposition{nullptr, nullptr}, &nodes[2]));
    }
  }

  SECTION("Right node has wrong outgoing direction")
  {
    SECTION("Branch 0")
    {
      std::swap(nodes[4].neighbors[0], nodes[4].neighbors[1]);
      CHECK_FALSE(validate_node_neighbors(vertices, ChainDecomposition{nullptr, nullptr}, &nodes[2]));
    }

    SECTION("Branch 1")
    {
      nodes[4].direction = HorizontalDirection::right;
      CHECK_FALSE(validate_node_neighbors(vertices, ChainDecomposition{nullptr, nullptr}, &nodes[2]));
    }
  }

  SECTION("Missing neighbor, with upper and lower boundary")
  {
    nodes[2].neighbors[0] = nullptr;
    CHECK_FALSE(validate_node_neighbors(vertices, ChainDecomposition{nullptr, nullptr}, &nodes[2]));

    nodes[4].neighbors[1] = nullptr;
    CHECK_FALSE(validate_node_neighbors(vertices, ChainDecomposition{nullptr, nullptr}, &nodes[4]));
  }

  SECTION("Inconsistent lower boundary")
  {
    nodes[2].lower_opp_edge = Edge::invalid();
    CHECK_FALSE(validate_node_neighbors(vertices, ChainDecomposition{nullptr, nullptr}, &nodes[2]));
    CHECK_FALSE(validate_node_neighbors(vertices, ChainDecomposition{nullptr, nullptr}, &nodes[0]));
  }

  SECTION("Inconsistent upper boundary")
  {
    nodes[2].upper_opp_edge = Edge::invalid();
    CHECK_FALSE(validate_node_neighbors(vertices, ChainDecomposition{nullptr, nullptr}, &nodes[2]));
    CHECK_FALSE(validate_node_neighbors(vertices, ChainDecomposition{nullptr, nullptr}, &nodes[1]));
  }

  SECTION("Valid, no upper boundary")
  {
    nodes[2].upper_opp_edge = Edge::invalid();
    nodes[2].neighbors[2] = nullptr;

    CHECK(validate_node_neighbors(vertices, ChainDecomposition{&nodes[2], &nodes[4]}, &nodes[2]));
  }

  SECTION("Missing neighbor, no upper boundary")
  {
    nodes[2].upper_opp_edge = Edge::invalid();
    nodes[2].neighbors[0] = nullptr;
    nodes[2].neighbors[2] = nullptr;

    CHECK_FALSE(validate_node_neighbors(vertices, ChainDecomposition{&nodes[2], &nodes[4]}, &nodes[2]));
  }

  SECTION("Neighbor should be nullptr but is set")
  {
    nodes[4].upper_opp_edge = Edge::invalid();

    // Make node[5] a chain-end node.
    nodes[5].direction = HorizontalDirection::left;
    nodes[5].type = NodeType::branch;
    nodes[5].vertex_it = vertices.begin() + 7;
    nodes[5].lower_opp_edge = Edge::invalid();
    nodes[5].upper_opp_edge = Edge::invalid();
    nodes[5].neighbors[0] = nullptr;
    nodes[5].neighbors[1] = nullptr;
    nodes[5].neighbors[2] = &nodes[4];

    CHECK_FALSE(validate_node_neighbors(vertices, ChainDecomposition{&nodes[4], &nodes[5]}, &nodes[4]));
  }

  SECTION("Valid, no lower boundary")
  {
    nodes[2].lower_opp_edge = Edge::invalid();
    nodes[2].neighbors[1] = nullptr;

    nodes[4].lower_opp_edge = Edge::invalid();
    nodes[4].neighbors[0] = nullptr;
    nodes[4].neighbors[2] = nullptr;

    CHECK(validate_node_neighbors(vertices, ChainDecomposition{&nodes[4], &nodes[2]}, &nodes[2]));
  }

  SECTION("Missing neighbor, no lower boundary")
  {
    nodes[2].lower_opp_edge = Edge::invalid();
    nodes[2].neighbors[0] = nullptr;
    nodes[2].neighbors[1] = nullptr;

    nodes[4].lower_opp_edge = Edge::invalid();
    nodes[4].neighbors[0] = nullptr;
    nodes[4].neighbors[2] = nullptr;

    CHECK_FALSE(validate_node_neighbors(vertices, ChainDecomposition{&nodes[4], &nodes[2]}, &nodes[2]));
  }

  SECTION("Neighbor doesn't link back")
  {
    nodes[4].neighbors[1] = nullptr;
    CHECK_FALSE(validate_node_neighbors(vertices, ChainDecomposition{nullptr, nullptr}, &nodes[2]));
  }

  SECTION("Lower boundary not monotone")
  {
    std::vector<Point2>& vertices_mut = polygon.unsafe_mutable_vertices();
    std::swap(vertices_mut[4], vertices_mut[5]);

    CHECK_FALSE(validate_node_neighbors(vertices, ChainDecomposition{nullptr, nullptr}, &nodes[2]));
  }

  SECTION("Upper boundary not monotone")
  {
    std::vector<Point2>& vertices_mut = polygon.unsafe_mutable_vertices();
    std::swap(vertices_mut[10], vertices_mut[11]);

    CHECK_FALSE(validate_node_neighbors(vertices, ChainDecomposition{nullptr, nullptr}, &nodes[2]));
  }

  SECTION("Leaf, valid")
  {
    CHECK(validate_node_neighbors(vertices, ChainDecomposition{nullptr, nullptr}, &nodes[0]));
  }

  SECTION("Leaf, neighbor doesn't link back")
  {
    nodes[2].neighbors[1] = nullptr;
    CHECK_FALSE(validate_node_neighbors(vertices, ChainDecomposition{nullptr, nullptr}, &nodes[0]));
  }
}

TEST_CASE("initial_chain_decompositions")
{
  Polygon2 polygon{
      {-4.16, 6.38}, {-2.46, 6.38}, {-0.74, 5.90}, {-0.94, 4.24}, {-3.12, 3.84}, {-1.76, 2.58},
      {2.02, 2.92},  {4.08, 4.18},  {2.42, 4.20},  {1.18, 4.98},  {2.48, 6.12},  {4.34, 6.48},
      {2.36, 7.84},  {0.86, 8.34},  {0.44, 7.38},  {-0.64, 8.30}, {-2.62, 7.64},
  };
  VerticesView vertices(polygon);

  SECTION("First chain starts at vertices 0")
  {
    NodePool node_pool;
    std::vector<ChainDecomposition> result = initial_chain_decompositions(vertices, node_pool);

    REQUIRE(result.size() == 4);

    CHECK(result[0].first_node->vertex_it == vertices.begin());
    CHECK(result[0].last_node->vertex_it == vertices.begin() + 4);
    CHECK(validate_chain_decomposition(vertices, result[0]));

    CHECK(result[1].first_node->vertex_it == vertices.begin() + 4);
    CHECK(result[1].last_node->vertex_it == vertices.begin() + 7);
    CHECK(validate_chain_decomposition(vertices, result[1]));

    CHECK(result[2].first_node->vertex_it == vertices.begin() + 7);
    CHECK(result[2].last_node->vertex_it == vertices.begin() + 11);
    CHECK(validate_chain_decomposition(vertices, result[2]));

    CHECK(result[3].first_node->vertex_it == vertices.begin() + 11);
    CHECK(result[3].last_node->vertex_it == vertices.begin());
    CHECK(validate_chain_decomposition(vertices, result[3]));
  }

  SECTION("First chain doens't start at vertex 0")
  {
    std::vector<Point2>& vertices_mut = polygon.unsafe_mutable_vertices();
    std::rotate(vertices_mut.begin(), vertices_mut.begin() + 1, vertices_mut.end());

    NodePool node_pool;
    std::vector<ChainDecomposition> result = initial_chain_decompositions(vertices, node_pool);

    REQUIRE(result.size() == 4);

    CHECK(result[0].first_node->vertex_it == vertices.begin() + 3);
    CHECK(result[0].last_node->vertex_it == vertices.begin() + 6);
    CHECK(validate_chain_decomposition(vertices, result[0]));

    CHECK(result[1].first_node->vertex_it == vertices.begin() + 6);
    CHECK(result[1].last_node->vertex_it == vertices.begin() + 10);
    CHECK(validate_chain_decomposition(vertices, result[1]));

    CHECK(result[2].first_node->vertex_it == vertices.begin() + 10);
    CHECK(result[2].last_node->vertex_it == vertices.begin() + 16);
    CHECK(validate_chain_decomposition(vertices, result[2]));

    CHECK(result[3].first_node->vertex_it == vertices.begin() + 16);
    CHECK(result[3].last_node->vertex_it == vertices.begin() + 3);
    CHECK(validate_chain_decomposition(vertices, result[3]));
  }
}

} // namespace dida::detail::vertical_decomposition