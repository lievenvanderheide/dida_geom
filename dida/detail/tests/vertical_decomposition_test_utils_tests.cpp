#include "dida/detail/tests/vertical_decomposition_test_utils.hpp"

#include <catch2/catch.hpp>

#include "dida/detail/vertical_decomposition_sweep_line_builder.hpp"
#include "dida/polygon2.hpp"

namespace dida::detail::vertical_decomposition
{

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
    nodes[0].is_leaf = true;
    nodes[0].vertex_it = vertices.begin() + 0;
    nodes[0].lower_opp_edge = Edge::edge_from_index(vertices, 0);
    nodes[0].upper_opp_edge = Edge::edge_from_index(vertices, 5);
    nodes[0].neighbors[0] = &nodes[3];

    nodes[1].direction = HorizontalDirection::left;
    nodes[1].is_leaf = true;
    nodes[1].vertex_it = vertices.begin() + 4;
    nodes[1].lower_opp_edge = Edge::edge_from_index(vertices, 4);
    nodes[1].upper_opp_edge = Edge::edge_from_index(vertices, 3);
    nodes[1].neighbors[0] = &nodes[2];

    nodes[2].direction = HorizontalDirection::right;
    nodes[2].is_leaf = false;
    nodes[2].vertex_it = vertices.begin() + 2;
    nodes[2].lower_opp_edge = Edge::edge_from_index(vertices, 4);
    nodes[2].upper_opp_edge = Edge::edge_from_index(vertices, 3);
    nodes[2].neighbors[0] = &nodes[1];
    nodes[2].neighbors[1] = &nodes[3];
    nodes[2].neighbors[2] = &nodes[5];

    nodes[3].direction = HorizontalDirection::left;
    nodes[3].is_leaf = false;
    nodes[3].vertex_it = vertices.begin() + 5;
    nodes[3].lower_opp_edge = Edge::edge_from_index(vertices, 0);
    nodes[3].upper_opp_edge = Edge::edge_from_index(vertices, 1);
    nodes[3].neighbors[0] = &nodes[4];
    nodes[3].neighbors[1] = &nodes[0];
    nodes[3].neighbors[2] = &nodes[2];

    nodes[4].direction = HorizontalDirection::right;
    nodes[4].is_leaf = true;
    nodes[4].vertex_it = vertices.begin() + 1;
    nodes[4].lower_opp_edge = Edge::edge_from_index(vertices, 0);
    nodes[4].upper_opp_edge = Edge::edge_from_index(vertices, 1);
    nodes[4].neighbors[0] = &nodes[3];

    nodes[5].direction = HorizontalDirection::right;
    nodes[5].is_leaf = true;
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
    nodes[0].is_leaf = false;
    nodes[0].vertex_it = vertices.begin() + 4;
    nodes[0].lower_opp_edge = Edge::invalid();
    nodes[0].upper_opp_edge = Edge::invalid();
    nodes[0].neighbors[0] = nullptr;
    nodes[0].neighbors[1] = &nodes[1];
    nodes[0].neighbors[2] = &nodes[4];

    nodes[1].direction = HorizontalDirection::right;
    nodes[1].is_leaf = false;
    nodes[1].vertex_it = vertices.begin() + 6;
    nodes[1].lower_opp_edge = Edge::invalid();
    nodes[1].upper_opp_edge = Edge::edge_from_index(vertices, 4);
    nodes[1].neighbors[0] = &nodes[0];
    nodes[1].neighbors[1] = &nodes[5];
    nodes[1].neighbors[2] = &nodes[2];

    nodes[2].direction = HorizontalDirection::right;
    nodes[2].is_leaf = true;
    nodes[2].vertex_it = vertices.begin() + 5;
    nodes[2].lower_opp_edge = Edge::edge_from_index(vertices, 5);
    nodes[2].upper_opp_edge = Edge::edge_from_index(vertices, 4);
    nodes[2].neighbors[0] = &nodes[1];

    nodes[3].direction = HorizontalDirection::left;
    nodes[3].is_leaf = true;
    nodes[3].vertex_it = vertices.begin() + 2;
    nodes[3].lower_opp_edge = Edge::edge_from_index(vertices, 1);
    nodes[3].upper_opp_edge = Edge::edge_from_index(vertices, 2);
    nodes[3].neighbors[0] = &nodes[4];

    nodes[4].direction = HorizontalDirection::left;
    nodes[4].is_leaf = false;
    nodes[4].vertex_it = vertices.begin() + 3;
    nodes[4].lower_opp_edge = Edge::edge_from_index(vertices, 1);
    nodes[4].upper_opp_edge = Edge::invalid();
    nodes[4].neighbors[0] = &nodes[5];
    nodes[4].neighbors[1] = &nodes[3];
    nodes[4].neighbors[2] = &nodes[0];

    nodes[5].direction = HorizontalDirection::left;
    nodes[5].is_leaf = false;
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
    node.is_leaf = false;
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
      node.is_leaf = true;
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
      node.is_leaf = true;
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

TEST_CASE("validate_neighboring_nodes")
{
  SECTION("Branch 0 to branch 1")
  {
    Polygon2 polygon{
        {-3.12, 5.96}, {-1.92, 4.82}, {-4.46, 3.42}, {-3.00, 2.24}, {-0.18, 2.98}, {0.90, 1.86},
        {3.92, 2.42},  {6.46, 4.36},  {0.42, 8.30},  {1.92, 5.60},  {-0.18, 6.60}, {-0.96, 5.84},
    };
    VerticesView vertices(polygon);

    Node left_node, right_node;

    left_node.direction = HorizontalDirection::left;
    left_node.is_leaf = false;
    left_node.vertex_it = vertices.begin() + 1;
    left_node.lower_opp_edge = Edge::edge_from_index(vertices, 3);
    left_node.upper_opp_edge = Edge::edge_from_index(vertices, 11);
    left_node.neighbors[0] = &right_node;
    left_node.neighbors[1] = nullptr;
    left_node.neighbors[2] = nullptr;

    right_node.direction = HorizontalDirection::left;
    right_node.is_leaf = false;
    right_node.vertex_it = vertices.begin() + 9;
    right_node.lower_opp_edge = Edge::edge_from_index(vertices, 5);
    right_node.upper_opp_edge = Edge::edge_from_index(vertices, 7);
    right_node.neighbors[0] = nullptr;
    right_node.neighbors[1] = &left_node;
    right_node.neighbors[2] = nullptr;

    SECTION("Valid")
    {
      CHECK(validate_neighboring_nodes(vertices, &left_node, 0, &right_node, 1));
    }

    SECTION("Left node wrong direction")
    {
      left_node.direction = HorizontalDirection::right;
      CHECK_FALSE(validate_neighboring_nodes(vertices, &left_node, 0, &right_node, 1));
    }

    SECTION("Right node wrong direction")
    {
      right_node.direction = HorizontalDirection::right;
      CHECK_FALSE(validate_neighboring_nodes(vertices, &left_node, 0, &right_node, 1));
    }

    SECTION("Lower boundary not monotone")
    {
      std::vector<Point2>& vertices_mut = polygon.unsafe_mutable_vertices();
      std::swap(vertices_mut[4], vertices_mut[5]);
      CHECK_FALSE(validate_neighboring_nodes(vertices, &left_node, 0, &right_node, 1));
    }

    SECTION("Upper boundary not monotone")
    {
      std::vector<Point2>& vertices_mut = polygon.unsafe_mutable_vertices();
      std::swap(vertices_mut[10], vertices_mut[11]);
      CHECK_FALSE(validate_neighboring_nodes(vertices, &left_node, 0, &right_node, 1));
    }

    SECTION("No lower boundary")
    {
      left_node.lower_opp_edge = Edge::invalid();
      right_node.lower_opp_edge = Edge::invalid();
      CHECK(validate_neighboring_nodes(vertices, &left_node, 0, &right_node, 1));
    }

    SECTION("No left_node.lower_opp_edge")
    {
      left_node.lower_opp_edge = Edge::invalid();
      CHECK_FALSE(validate_neighboring_nodes(vertices, &left_node, 0, &right_node, 1));
    }

    SECTION("No left_node.upper_opp_edge")
    {
      left_node.upper_opp_edge = Edge::invalid();
      CHECK_FALSE(validate_neighboring_nodes(vertices, &left_node, 0, &right_node, 1));
    }

    SECTION("No right_node.lower_opp_edge")
    {
      right_node.lower_opp_edge = Edge::invalid();
      CHECK_FALSE(validate_neighboring_nodes(vertices, &left_node, 0, &right_node, 1));
    }
  }

  SECTION("Branch 1 to branch 2")
  {
    Polygon2 polygon{
        {-5.62, 1.92}, {-3.00, -1.16}, {1.36, 0.72},  {2.62, -0.10}, {4.60, 1.42},  {6.60, -0.46}, {4.64, -1.78},
        {2.74, -1.48}, {5.18, -3.14},  {8.00, -0.88}, {6.94, 3.78},  {2.96, 2.68},  {0.30, 4.56},  {-0.90, 2.60},
        {-2.18, 3.00}, {-4.16, 1.76},  {-3.72, 2.86}, {-2.26, 3.92}, {-4.32, 3.46},
    };
    VerticesView vertices(polygon);

    Node left_node, right_node;

    left_node.direction = HorizontalDirection::right;
    left_node.is_leaf = false;
    left_node.vertex_it = vertices.begin() + 15;
    left_node.lower_opp_edge = Edge::edge_from_index(vertices, 0);
    left_node.upper_opp_edge = Edge::edge_from_index(vertices, 17);
    left_node.neighbors[0] = nullptr;
    left_node.neighbors[1] = &right_node;
    left_node.neighbors[2] = nullptr;

    right_node.direction = HorizontalDirection::left;
    right_node.is_leaf = false;
    right_node.vertex_it = vertices.begin() + 5;
    right_node.lower_opp_edge = Edge::edge_from_index(vertices, 8);
    right_node.upper_opp_edge = Edge::edge_from_index(vertices, 10);
    right_node.neighbors[0] = nullptr;
    right_node.neighbors[1] = nullptr;
    right_node.neighbors[2] = &left_node;

    SECTION("Valid")
    {
      CHECK(validate_neighboring_nodes(vertices, &left_node, 1, &right_node, 2));
    }

    SECTION("Left node wrong direction")
    {
      left_node.direction = HorizontalDirection::left;
      CHECK_FALSE(validate_neighboring_nodes(vertices, &left_node, 1, &right_node, 2));
    }

    SECTION("Right node wrong direction")
    {
      right_node.direction = HorizontalDirection::right;
      CHECK_FALSE(validate_neighboring_nodes(vertices, &left_node, 1, &right_node, 2));
    }

    SECTION("Lower boundary not monotone")
    {
      std::vector<Point2>& vertices_mut = polygon.unsafe_mutable_vertices();
      std::swap(vertices_mut[2], vertices_mut[3]);
      CHECK_FALSE(validate_neighboring_nodes(vertices, &left_node, 1, &right_node, 2));
    }

    SECTION("Upper boundary not monotone")
    {
      std::vector<Point2>& vertices_mut = polygon.unsafe_mutable_vertices();
      std::swap(vertices_mut[11], vertices_mut[12]);
      CHECK_FALSE(validate_neighboring_nodes(vertices, &left_node, 1, &right_node, 2));
    }

    SECTION("No left_node.lower_opp_edge")
    {
      left_node.lower_opp_edge = Edge::invalid();
      CHECK_FALSE(validate_neighboring_nodes(vertices, &left_node, 1, &right_node, 2));
    }

    SECTION("No right_node.upper_opp_edge")
    {
      right_node.upper_opp_edge = Edge::invalid();
      CHECK_FALSE(validate_neighboring_nodes(vertices, &left_node, 1, &right_node, 2));
    }
  }

  SECTION("Branch 2 to branch 0")
  {
    Polygon2 polygon{
        {-1.88, 0.74}, {-3.08, 1.56}, {-3.84, 2.64}, {-2.22, 3.56}, {-0.08, 2.80},  {0.94, 3.48},   {1.82, 2.30},
        {3.06, 3.20},  {7.36, 4.96},  {2.98, 5.54},  {6.20, 6.80},  {4.08, 7.64},   {2.14, 7.90},   {0.60, 6.58},
        {-1.10, 7.74}, {-1.74, 6.80}, {-4.72, 6.60}, {-7.60, 4.34}, {-5.56, -0.06}, {-3.44, -0.44},
    };
    VerticesView vertices(polygon);

    Node left_node, right_node;

    left_node.direction = HorizontalDirection::right;
    left_node.is_leaf = false;
    left_node.vertex_it = vertices.begin() + 2;
    left_node.lower_opp_edge = Edge::edge_from_index(vertices, 18);
    left_node.upper_opp_edge = Edge::edge_from_index(vertices, 15);
    left_node.neighbors[0] = nullptr;
    left_node.neighbors[1] = nullptr;
    left_node.neighbors[2] = &right_node;

    right_node.direction = HorizontalDirection::right;
    right_node.is_leaf = false;
    right_node.vertex_it = vertices.begin() + 9;
    right_node.lower_opp_edge = Edge::edge_from_index(vertices, 6);
    right_node.upper_opp_edge = Edge::edge_from_index(vertices, 11);
    right_node.neighbors[0] = &left_node;
    right_node.neighbors[1] = nullptr;
    right_node.neighbors[2] = nullptr;

    SECTION("Valid")
    {
      CHECK(validate_neighboring_nodes(vertices, &left_node, 2, &right_node, 0));
    }

    SECTION("Left node wrong direction")
    {
      left_node.direction = HorizontalDirection::left;
      CHECK_FALSE(validate_neighboring_nodes(vertices, &left_node, 2, &right_node, 0));
    }

    SECTION("Right node wrong direction")
    {
      right_node.direction = HorizontalDirection::left;
      CHECK_FALSE(validate_neighboring_nodes(vertices, &left_node, 2, &right_node, 0));
    }

    SECTION("Lower boundary not monotone")
    {
      std::vector<Point2>& vertices_mut = polygon.unsafe_mutable_vertices();
      std::swap(vertices_mut[5], vertices_mut[6]);
      CHECK_FALSE(validate_neighboring_nodes(vertices, &left_node, 2, &right_node, 0));
    }

    SECTION("Upper boundary not monotone")
    {
      std::vector<Point2>& vertices_mut = polygon.unsafe_mutable_vertices();
      std::swap(vertices_mut[14], vertices_mut[15]);
      CHECK_FALSE(validate_neighboring_nodes(vertices, &left_node, 2, &right_node, 0));
    }

    SECTION("No upper boundary")
    {
      left_node.upper_opp_edge = Edge::invalid();
      right_node.upper_opp_edge = Edge::invalid();
      CHECK(validate_neighboring_nodes(vertices, &left_node, 2, &right_node, 0));
    }

    SECTION("No left_node.upper_opp_edge")
    {
      left_node.upper_opp_edge = Edge::invalid();
      CHECK_FALSE(validate_neighboring_nodes(vertices, &left_node, 2, &right_node, 0));
    }

    SECTION("No right_node.lower_opp_edge")
    {
      right_node.lower_opp_edge = Edge::invalid();
      CHECK_FALSE(validate_neighboring_nodes(vertices, &left_node, 2, &right_node, 0));
    }

    SECTION("No right_node.upper_opp_edge")
    {
      right_node.upper_opp_edge = Edge::invalid();
      CHECK_FALSE(validate_neighboring_nodes(vertices, &left_node, 2, &right_node, 0));
    }
  }

  SECTION("No lower and upper boundary")
  {
    Polygon2 polygon{
        {-3.22, 1.12}, {1.80, 3.42}, {4.84, 1.48},  {3.32, 4.84},
        {5.46, 7.30},  {0.32, 5.80}, {-3.38, 7.34}, {-1.04, 3.82},
    };
    VerticesView vertices(polygon);

    Node left_node, right_node;

    left_node.direction = HorizontalDirection::left;
    left_node.is_leaf = false;
    left_node.vertex_it = vertices.begin() + 7;
    left_node.lower_opp_edge = Edge::invalid();
    left_node.upper_opp_edge = Edge::invalid();
    left_node.neighbors[0] = &right_node;
    left_node.neighbors[1] = nullptr;
    left_node.neighbors[2] = nullptr;

    right_node.direction = HorizontalDirection::right;
    right_node.is_leaf = false;
    right_node.vertex_it = vertices.begin() + 3;
    right_node.lower_opp_edge = Edge::invalid();
    right_node.upper_opp_edge = Edge::invalid();
    right_node.neighbors[0] = &left_node;
    right_node.neighbors[1] = nullptr;
    right_node.neighbors[2] = nullptr;

    CHECK_FALSE(validate_neighboring_nodes(vertices, &left_node, 0, &right_node, 0));
  }
}

} // namespace dida::detail::vertical_decomposition