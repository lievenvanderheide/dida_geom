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

} // namespace dida::detail::vertical_decomposition