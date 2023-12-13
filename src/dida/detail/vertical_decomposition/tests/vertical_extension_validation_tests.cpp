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

/// If <tt>winding == Winding::cw</tt>, returns @c point with its x-coordinate flip, otherwise returns @c point as it
/// is.
Point2 flip_horizontally_if_necessary(Winding winding, Point2 point)
{
  return winding == Winding::ccw ? point : Point2(-point.x(), point.y());
}

/// If <tt>winding == Winding::cw</tt>, returns @c -x, otherwise returns @c x as it is.
ScalarDeg1 flip_x_if_necessary(Winding winding, ScalarDeg1 x)
{
  return winding == Winding::ccw ? x : -x;
}

} // namespace

TEST_CASE("ray_cast_up")
{
  std::vector<Point2> vertices_storage{
      {1.98, -2.24}, {7.44, 0.74}, {2.38, 2.48}, {5.06, 0.68}, {2.26, -0.92}, {0.26, 3.38}, {4.98, 4.52}, {-1.16, 4.02},
  };
  VerticesView vertices(vertices_storage);

  Winding winding = GENERATE(Winding::ccw, Winding::cw);
  if (winding == Winding::cw)
  {
    flip_horizontally(vertices_storage);
  }

  SECTION("Full polygon")
  {
    SECTION("Hits edge from inside")
    {
      Edge edge = ray_cast_up(vertices, winding, std::nullopt, flip_horizontally_if_necessary(winding, {4.06, -0.64}));
      REQUIRE(edge.is_valid());
      CHECK(edge.start_vertex_it == vertices.begin() + 3);
      CHECK(edge.end_vertex_it == vertices.begin() + 4);
    }

    SECTION("Hits edge from outside")
    {
      Edge edge = ray_cast_up(vertices, winding, std::nullopt, flip_horizontally_if_necessary(winding, {3.26, 1.16}));
      CHECK(!edge.is_valid());
    }

    SECTION("No hit")
    {
      Edge edge = ray_cast_up(vertices, winding, std::nullopt, flip_horizontally_if_necessary(winding, {6.36, 3.32}));
      CHECK(!edge.is_valid());
    }

    SECTION("Hits vertex -> return left side edge")
    {
      Edge edge = ray_cast_up(vertices, winding, std::nullopt, flip_horizontally_if_necessary(winding, {2.26, -1.52}));
      REQUIRE(edge.is_valid());

      if (winding == Winding::ccw)
      {
        CHECK(edge.start_vertex_it == vertices.begin() + 4);
        CHECK(edge.end_vertex_it == vertices.begin() + 5);
      }
      else
      {
        CHECK(edge.start_vertex_it == vertices.begin() + 3);
        CHECK(edge.end_vertex_it == vertices.begin() + 4);
      }
    }

    SECTION("ray_origin on edge -> ignore edge")
    {
      Edge edge = ray_cast_up(vertices, winding, std::nullopt, flip_horizontally_if_necessary(winding, {2.26, -0.92}));
      CHECK(!edge.is_valid());
    }
  }

  SECTION("With range")
  {
    SECTION("Hits edge in range")
    {
      PolygonRange range{
          {1, flip_x_if_necessary(winding, ScalarDeg1(5.76))},
          {5, flip_x_if_necessary(winding, ScalarDeg1(2.2))},
      };

      Edge edge = ray_cast_up(vertices, winding, range, flip_horizontally_if_necessary(winding, {3.82, -0.62}));
      REQUIRE(edge.is_valid());
      CHECK(edge.start_vertex_it == vertices.begin() + 3);
      CHECK(edge.end_vertex_it == vertices.begin() + 4);
    }

    SECTION("Doesn't hit edge outside range")
    {
      PolygonRange range{
          {1, flip_x_if_necessary(winding, ScalarDeg1(5.76))},
          {5, flip_x_if_necessary(winding, ScalarDeg1(2.2))},
      };

      Edge edge = ray_cast_up(vertices, winding, range, flip_horizontally_if_necessary(winding, {0.86, 3.9}));
      CHECK_FALSE(edge.is_valid());
    }

    SECTION("With wrapping")
    {
      PolygonRange range{
          {7, flip_x_if_necessary(winding, ScalarDeg1(0.18))},
          {3, flip_x_if_necessary(winding, ScalarDeg1(3.42))},
      };

      Edge edge = ray_cast_up(vertices, winding, range, flip_horizontally_if_necessary(winding, {5.9, 0.58}));
      REQUIRE(edge.is_valid());
      CHECK(edge.start_vertex_it == vertices.begin() + 1);
      CHECK(edge.end_vertex_it == vertices.begin() + 2);
    }

    SECTION("Partial first edge towards left")
    {
      PolygonRange range{
          {1, flip_x_if_necessary(winding, ScalarDeg1(5.76))},
          {5, flip_x_if_necessary(winding, ScalarDeg1(2.2))},
      };

      SECTION("Hit")
      {
        Edge edge = ray_cast_up(vertices, winding, range, flip_horizontally_if_necessary(winding, {3.56, 1.92}));
        REQUIRE(edge.is_valid());
        CHECK(edge.start_vertex_it == vertices.begin() + 1);
        CHECK(edge.end_vertex_it == vertices.begin() + 2);
      }

      SECTION("Miss")
      {
        Edge edge = ray_cast_up(vertices, winding, range, flip_horizontally_if_necessary(winding, {6.54, 0.8}));
        CHECK_FALSE(edge.is_valid());
      }

      SECTION("On range start point")
      {
        Edge edge = ray_cast_up(vertices, winding, range, flip_horizontally_if_necessary(winding, {5.76, 1.14}));
        REQUIRE(edge.is_valid());
        CHECK(edge.start_vertex_it == vertices.begin() + 1);
        CHECK(edge.end_vertex_it == vertices.begin() + 2);
      }
    }

    SECTION("Partial first edge towards right")
    {
      PolygonRange range{
          {5, flip_x_if_necessary(winding, ScalarDeg1(2.2))},
          {1, flip_x_if_necessary(winding, ScalarDeg1(5.76))},
      };

      SECTION("Hit")
      {
        // The range's first edge is an upward facing edge, and we're hitting it from below, so even though there's a
        // hit, the expected return value is Edge::invalid().
        Edge edge = ray_cast_up(vertices, winding, range, flip_horizontally_if_necessary(winding, {3.98, 3.93}));
        CHECK_FALSE(edge.is_valid());
      }

      SECTION("Miss")
      {
        // We're missing the range's first edge, and instead hit the edge above it.
        Edge edge = ray_cast_up(vertices, winding, range, flip_horizontally_if_necessary(winding, {1.25, 2.97}));
        REQUIRE(edge.is_valid());
        CHECK(edge.start_vertex_it == vertices.begin() + 6);
        CHECK(edge.end_vertex_it == vertices.begin() + 7);
      }

      SECTION("On range start point")
      {
        UNSCOPED_INFO("Hier " << (winding == Winding::ccw ? "ccw" : "cw"));
        Edge edge = ray_cast_up(vertices, winding, range, flip_horizontally_if_necessary(winding, {2.2, 2.93}));
        UNSCOPED_INFO("Done");
        CHECK_FALSE(edge.is_valid());
      }
    }

    SECTION("Partial last edge towards left")
    {
      PolygonRange range{
          {1, flip_x_if_necessary(winding, ScalarDeg1(5.76))},
          {4, flip_x_if_necessary(winding, ScalarDeg1(0.96))},
      };

      SECTION("Hit")
      {
        Edge edge = ray_cast_up(vertices, winding, range, flip_horizontally_if_necessary(winding, {1.8, -0.26}));
        REQUIRE(edge.is_valid());
        CHECK(edge.start_vertex_it == vertices.begin() + 4);
        CHECK(edge.end_vertex_it == vertices.begin() + 5);
      }

      SECTION("Miss")
      {
        Edge edge = ray_cast_up(vertices, winding, range, flip_horizontally_if_necessary(winding, {0.5, 2.46}));
        CHECK_FALSE(edge.is_valid());
      }

      SECTION("On range end point")
      {
        Edge edge = ray_cast_up(vertices, winding, range, flip_horizontally_if_necessary(winding, {0.96, 0.4}));
        REQUIRE(edge.is_valid());
        CHECK(edge.start_vertex_it == vertices.begin() + 4);
        CHECK(edge.end_vertex_it == vertices.begin() + 5);
      }
    }

    SECTION("Partial last edge towards right")
    {
      PolygonRange range{
          {7, flip_x_if_necessary(winding, ScalarDeg1(0.16))},
          {2, flip_x_if_necessary(winding, ScalarDeg1(3.6))},
      };

      SECTION("Hit")
      {
        // The range's last edge is an upward facing edge, and we're hitting it from below, so even though there's a
        // hit, the expected return value is Edge::invalid().
        Edge edge = ray_cast_up(vertices, winding, range, flip_horizontally_if_necessary(winding, {2.74, 1.78}));
        CHECK_FALSE(edge.is_valid());
      }

      SECTION("Miss")
      {
        // We're missing the range's last edge, and instead hit the above it.
        Edge edge = ray_cast_up(vertices, winding, range, flip_horizontally_if_necessary(winding, {4.12, 0.49}));
        REQUIRE(edge.is_valid());
        CHECK(edge.start_vertex_it == vertices.begin() + 1);
        CHECK(edge.end_vertex_it == vertices.begin() + 2);
      }

      SECTION("On range end point")
      {
        Edge edge = ray_cast_up(vertices, winding, range, flip_horizontally_if_necessary(winding, {3.6, 0.8}));
        CHECK_FALSE(edge.is_valid());
      }
    }
  }
}

TEST_CASE("ray_cast_down")
{
  std::vector<Point2> vertices_storage{
      {-2.41, 3.78}, {-4.93, 1.68}, {1.45, -0.08}, {-2.47, -0.96},
      {1.81, -2.16}, {5.21, -1.82}, {2.65, 2.3},   {-3.69, 1.76},
  };
  VerticesView vertices(vertices_storage);

  Winding winding = GENERATE(Winding::ccw, Winding::cw);
  if (winding == Winding::cw)
  {
    flip_horizontally(vertices_storage);
  }

  SECTION("Full polygon")
  {
    SECTION("Hits edge from inside")
    {
      Edge edge = ray_cast_down(vertices, winding, std::nullopt, flip_horizontally_if_necessary(winding, {0.47, 1.12}));
      REQUIRE(edge.is_valid());
      CHECK(edge.start_vertex_it == vertices.begin() + 1);
      CHECK(edge.end_vertex_it == vertices.begin() + 2);
    }

    SECTION("Hits edge from outside")
    {
      Edge edge =
          ray_cast_down(vertices, winding, std::nullopt, flip_horizontally_if_necessary(winding, {-1.29, 2.38}));
      CHECK(!edge.is_valid());
    }

    SECTION("No hit")
    {
      Edge edge =
          ray_cast_down(vertices, winding, std::nullopt, flip_horizontally_if_necessary(winding, {-3.63, -0.68}));
    }

    SECTION("Hits vertex -> return right side edge")
    {
      Edge edge =
          ray_cast_down(vertices, winding, std::nullopt, flip_horizontally_if_necessary(winding, {1.81, -0.98}));
      REQUIRE(edge.is_valid());

      if (winding == Winding::ccw)
      {
        CHECK(edge.start_vertex_it == vertices.begin() + 4);
        CHECK(edge.end_vertex_it == vertices.begin() + 5);
      }
      else
      {
        CHECK(edge.start_vertex_it == vertices.begin() + 3);
        CHECK(edge.end_vertex_it == vertices.begin() + 4);
      }
    }

    SECTION("ray_origin on edge -> ignore edge")
    {
      Edge edge = ray_cast_down(vertices, winding, std::nullopt, flip_horizontally_if_necessary(winding, {2.65, 2.3}));
      REQUIRE(edge.is_valid());
      CHECK(edge.start_vertex_it == vertices.begin() + 4);
      CHECK(edge.end_vertex_it == vertices.begin() + 5);
    }
  }

  SECTION("With range")
  {
    SECTION("Hits edge in range")
    {
      PolygonRange range{
          {1, flip_x_if_necessary(winding, ScalarDeg1(-4))},
          {6, flip_x_if_necessary(winding, ScalarDeg1(1.8))},
      };

      Edge edge = ray_cast_down(vertices, winding, range, flip_horizontally_if_necessary(winding, {-0.72, 1.04}));
      REQUIRE(edge.is_valid());
      CHECK(edge.start_vertex_it == vertices.begin() + 1);
      CHECK(edge.end_vertex_it == vertices.begin() + 2);
    }

    SECTION("Doesn't hit edge outside range")
    {
      PolygonRange range{
          {2, flip_x_if_necessary(winding, ScalarDeg1(-1.42))},
          {6, flip_x_if_necessary(winding, ScalarDeg1(-2.72))},
      };

      Edge edge = ray_cast_down(vertices, winding, range, flip_horizontally_if_necessary(winding, {-0.72, 1.04}));
      REQUIRE(edge.is_valid());
      CHECK(edge.start_vertex_it == vertices.begin() + 3);
      CHECK(edge.end_vertex_it == vertices.begin() + 4);
    }

    SECTION("With wrapping")
    {
      PolygonRange range{
          {6, flip_x_if_necessary(winding, ScalarDeg1(-2.27))},
          {2, flip_x_if_necessary(winding, ScalarDeg1(-1.42))},
      };

      Edge edge = ray_cast_down(vertices, winding, range, flip_horizontally_if_necessary(winding, {-0.72, 1.04}));
      REQUIRE(edge.is_valid());
      CHECK(edge.start_vertex_it == vertices.begin() + 1);
      CHECK(edge.end_vertex_it == vertices.begin() + 2);
    }

    SECTION("Partial first edge towards left")
    {
      PolygonRange range{
          {2, flip_x_if_necessary(winding, ScalarDeg1(-0.32))},
          {6, flip_x_if_necessary(winding, ScalarDeg1(-1.96))},
      };

      SECTION("Hit")
      {
        // The range's first edge is a downward facing edge, and we're hitting it from above, so even though there's a
        // hit, the expected return value is Edge::invalid().
        Edge edge = ray_cast_down(vertices, winding, range, flip_horizontally_if_necessary(winding, {-1.94, 0.36}));
        CHECK_FALSE(edge.is_valid());
      }

      SECTION("Miss")
      {
        // We're missing the range's first edge, and instead hit the edge below it.
        Edge edge = ray_cast_down(vertices, winding, range, flip_horizontally_if_necessary(winding, {0.36, 0.04}));
        REQUIRE(edge.is_valid());
        CHECK(edge.start_vertex_it == vertices.begin() + 3);
        CHECK(edge.end_vertex_it == vertices.begin() + 4);
      }

      SECTION("On range start point")
      {
        Edge edge = ray_cast_down(vertices, winding, range, flip_horizontally_if_necessary(winding, {-0.32, -0.08}));
        CHECK_FALSE(edge.is_valid());
      }
    }

    SECTION("Partial first edge towards right")
    {
      PolygonRange range{
          {1, flip_x_if_necessary(winding, ScalarDeg1(-3.71))},
          {5, flip_x_if_necessary(winding, ScalarDeg1(3.43))},
      };

      SECTION("Hit")
      {
        Edge edge = ray_cast_down(vertices, winding, range, flip_horizontally_if_necessary(winding, {-1.13, 1.11}));
        REQUIRE(edge.is_valid());
        CHECK(edge.start_vertex_it == vertices.begin() + 1);
        CHECK(edge.end_vertex_it == vertices.begin() + 2);
      }

      SECTION("Miss")
      {
        Edge edge = ray_cast_down(vertices, winding, range, flip_horizontally_if_necessary(winding, {-4.37, 1.74}));
        CHECK_FALSE(edge.is_valid());
      }

      SECTION("On range start point")
      {
        Edge edge = ray_cast_down(vertices, winding, range, flip_horizontally_if_necessary(winding, {-3.71, 1.61}));
        REQUIRE(edge.is_valid());
        CHECK(edge.start_vertex_it == vertices.begin() + 1);
        CHECK(edge.end_vertex_it == vertices.begin() + 2);
      }
    }

    SECTION("Partial last edge towards left")
    {
      PolygonRange range{
          {1, flip_x_if_necessary(winding, ScalarDeg1(-3.3))},
          {6, flip_x_if_necessary(winding, ScalarDeg1(-0.74))},
      };

      SECTION("Hit")
      {
        // The range's last edge is a downward facing edge, and we're hitting it from above, so even though there's a
        // hit, the expected return value is Edge::invalid().
        Edge edge = ray_cast_down(vertices, winding, range, flip_horizontally_if_necessary(winding, {0.86, 3.04}));
        CHECK_FALSE(edge.is_valid());
      }

      SECTION("Miss")
      {
        // We're missing the range's last edge, and instead hit the edge below it.
        Edge edge = ray_cast_down(vertices, winding, range, flip_horizontally_if_necessary(winding, {-1.72, 2.56}));
        REQUIRE(edge.is_valid());
        CHECK(edge.start_vertex_it == vertices.begin() + 1);
        CHECK(edge.end_vertex_it == vertices.begin() + 2);
      }

      SECTION("On range start point")
      {
        Edge edge = ray_cast_down(vertices, winding, range, flip_horizontally_if_necessary(winding, {-0.74, 2.23}));
        CHECK_FALSE(edge.is_valid());
      }
    }

    SECTION("Partial last edge towards right")
    {
      PolygonRange range{
          {3, flip_x_if_necessary(winding, ScalarDeg1(-1.64))},
          {1, flip_x_if_necessary(winding, ScalarDeg1(-0.86))},
      };

      SECTION("Hit")
      {
        Edge edge = ray_cast_down(vertices, winding, range, flip_horizontally_if_necessary(winding, {-1.86, 1.06}));
        REQUIRE(edge.is_valid());
        CHECK(edge.start_vertex_it == vertices.begin() + 1);
        CHECK(edge.end_vertex_it == vertices.begin() + 2);
      }

      SECTION("Miss")
      {
        Edge edge = ray_cast_down(vertices, winding, range, flip_horizontally_if_necessary(winding, {-0.1, 0.66}));
        REQUIRE(edge.is_valid());
        CHECK(edge.start_vertex_it == vertices.begin() + 3);
        CHECK(edge.end_vertex_it == vertices.begin() + 4);
      }

      SECTION("On range end point")
      {
        Edge edge = ray_cast_down(vertices, winding, range, flip_horizontally_if_necessary(winding, {-0.86, 1.06}));
        REQUIRE(edge.is_valid());
        CHECK(edge.start_vertex_it == vertices.begin() + 1);
        CHECK(edge.end_vertex_it == vertices.begin() + 2);
      }
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
      flip_horizontally(vertices_storage);
      flip_horizontally(nodes);
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
      flip_horizontally(vertices_storage);
      flip_horizontally(nodes);
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
      flip_horizontally(vertices_storage);
      flip_horizontally(nodes);
    }

    std::vector<VerticalExtensionContactPoint> contact_points =
        vertical_extension_contact_points(chain_decomposition, winding);
    REQUIRE(contact_points.size() == 10);

    std::vector<ChainDecompositionIsland> islands =
        split_chain_decomposition_into_islands(vertices, winding, chain_decomposition, contact_points);

    REQUIRE(islands.size() == 2);

    CHECK(islands[0].contact_points.begin() == contact_points.data());
    CHECK(islands[0].contact_points.end() == contact_points.data() + 6);
    CHECK(islands[0].range.begin.edge_index == 0);
    CHECK(islands[0].range.begin.x == flip_x_if_necessary(winding, ScalarDeg1(2.40)));
    CHECK(islands[0].range.end.edge_index == 3);
    CHECK(islands[0].range.end.x == flip_x_if_necessary(winding, ScalarDeg1(-0.06)));

    CHECK(islands[1].contact_points.begin() == contact_points.data() + 7);
    CHECK(islands[1].contact_points.end() == contact_points.data() + 10);
    CHECK(islands[1].range.begin.edge_index == 3);
    CHECK(islands[1].range.begin.x == flip_x_if_necessary(winding, ScalarDeg1(-0.06)));
    CHECK(islands[1].range.end.edge_index == 5);
    CHECK(islands[1].range.end.x == flip_x_if_necessary(winding, ScalarDeg1(-2.34)));
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
      flip_horizontally(vertices_storage);
      flip_horizontally(nodes);
    }

    std::vector<VerticalExtensionContactPoint> contact_points =
        vertical_extension_contact_points(chain_decomposition, winding);
    REQUIRE(contact_points.size() == 10);

    std::vector<ChainDecompositionIsland> islands =
        split_chain_decomposition_into_islands(vertices, winding, chain_decomposition, contact_points);

    REQUIRE(islands.size() == 2);

    CHECK(islands[0].contact_points.begin() == contact_points.data());
    CHECK(islands[0].contact_points.end() == contact_points.data() + 3);
    CHECK(islands[0].range.begin.edge_index == 0);
    CHECK(islands[0].range.begin.x == flip_x_if_necessary(winding, ScalarDeg1(-1.20)));
    CHECK(islands[0].range.end.edge_index == 2);
    CHECK(islands[0].range.end.x == flip_x_if_necessary(winding, ScalarDeg1(0.56)));

    CHECK(islands[1].contact_points.begin() == contact_points.data() + 4);
    CHECK(islands[1].contact_points.end() == contact_points.data() + 10);
    CHECK(islands[1].range.begin.edge_index == 2);
    CHECK(islands[1].range.begin.x == flip_x_if_necessary(winding, ScalarDeg1(0.56)));
    CHECK(islands[1].range.end.edge_index == 5);
    CHECK(islands[1].range.end.x == flip_x_if_necessary(winding, ScalarDeg1(2.04)));
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
      flip_horizontally(vertices_storage);
      flip_horizontally(nodes);
    }

    std::vector<VerticalExtensionContactPoint> contact_points =
        vertical_extension_contact_points(chain_decomposition, winding);
    REQUIRE(contact_points.size() == 8);

    std::vector<ChainDecompositionIsland> islands =
        split_chain_decomposition_into_islands(vertices, winding, chain_decomposition, contact_points);

    REQUIRE(islands.size() == 2);

    CHECK(islands[0].contact_points.begin() == contact_points.data());
    CHECK(islands[0].contact_points.end() == contact_points.data() + 4);
    CHECK(islands[0].range.begin.edge_index == 0);
    CHECK(islands[0].range.begin.x == flip_x_if_necessary(winding, ScalarDeg1(-5.10)));
    CHECK(islands[0].range.end.edge_index == 4);
    CHECK(islands[0].range.end.x == flip_x_if_necessary(winding, ScalarDeg1(-5.10)));

    CHECK(islands[1].contact_points.begin() == contact_points.data() + 5);
    CHECK(islands[1].contact_points.end() == contact_points.data() + 8);
    CHECK(islands[1].range.begin.edge_index == 4);
    CHECK(islands[1].range.begin.x == flip_x_if_necessary(winding, ScalarDeg1(-5.10)));
    CHECK(islands[1].range.end.edge_index == 7);
    CHECK(islands[1].range.end.x == flip_x_if_necessary(winding, ScalarDeg1(-1.62)));
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
      flip_horizontally(vertices_storage);
      flip_horizontally(nodes);
    }

    std::vector<VerticalExtensionContactPoint> contact_points =
        vertical_extension_contact_points(chain_decomposition, winding);
    REQUIRE(contact_points.size() == 10);

    std::vector<ChainDecompositionIsland> islands =
        split_chain_decomposition_into_islands(vertices, winding, chain_decomposition, contact_points);

    REQUIRE(islands.size() == 2);

    CHECK(islands[0].contact_points.begin() == contact_points.data());
    CHECK(islands[0].contact_points.end() == contact_points.data() + 4);
    CHECK(islands[0].range.begin.edge_index == 0);
    CHECK(islands[0].range.begin.x == flip_x_if_necessary(winding, ScalarDeg1(4.28)));
    CHECK(islands[0].range.end.edge_index == 3);
    CHECK(islands[0].range.end.x == flip_x_if_necessary(winding, ScalarDeg1(4.28)));

    CHECK(islands[1].contact_points.begin() == contact_points.data() + 5);
    CHECK(islands[1].contact_points.end() == contact_points.data() + 8);
    CHECK(islands[1].range.begin.edge_index == 3);
    CHECK(islands[1].range.begin.x == flip_x_if_necessary(winding, ScalarDeg1(4.28)));
    CHECK(islands[1].range.end.edge_index == 6);
    CHECK(islands[1].range.end.x == flip_x_if_necessary(winding, ScalarDeg1(0.56)));
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
        flip_horizontally(vertices_storage);
        flip_horizontally(nodes);
      }

      std::vector<VerticalExtensionContactPoint> contact_points =
          vertical_extension_contact_points(chain_decomposition, winding);
      REQUIRE(contact_points.size() == 11);

      std::vector<ChainDecompositionIsland> islands =
          split_chain_decomposition_into_islands(vertices, winding, chain_decomposition, contact_points);

      REQUIRE(islands.size() == 1);
      CHECK(islands[0].contact_points.begin() == contact_points.data());
      CHECK(islands[0].contact_points.end() == contact_points.data() + 10);
      CHECK(islands[0].range.begin.edge_index == 0);
      CHECK(islands[0].range.begin.x == flip_x_if_necessary(winding, ScalarDeg1(-.06)));
      CHECK(islands[0].range.end.edge_index == 7);
      CHECK(islands[0].range.end.x == flip_x_if_necessary(winding, ScalarDeg1(.8)));
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
        flip_horizontally(vertices_storage);
        flip_horizontally(nodes);
      }

      std::vector<VerticalExtensionContactPoint> contact_points =
          vertical_extension_contact_points(chain_decomposition, winding);
      REQUIRE(contact_points.size() == 11);

      std::vector<ChainDecompositionIsland> islands =
          split_chain_decomposition_into_islands(vertices, winding, chain_decomposition, contact_points);

      REQUIRE(islands.size() == 1);
      CHECK(islands[0].contact_points.begin() == contact_points.data());
      CHECK(islands[0].contact_points.end() == contact_points.data() + 10);
      CHECK(islands[0].range.begin.edge_index == 0);
      CHECK(islands[0].range.begin.x == flip_x_if_necessary(winding, ScalarDeg1(4.88)));
      CHECK(islands[0].range.end.edge_index == 6);
      CHECK(islands[0].range.end.x == flip_x_if_necessary(winding, ScalarDeg1(4.26)));
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
      if (winding == Winding::cw)
      {
        flip_horizontally(vertices_storage);
        flip_horizontally(nodes);
      }

      std::vector<VerticalExtensionContactPoint> contact_points =
          vertical_extension_contact_points(chain_decomposition, winding);
      REQUIRE(contact_points.size() == 14);

      std::vector<ChainDecompositionIsland> islands =
          split_chain_decomposition_into_islands(vertices, winding, chain_decomposition, contact_points);

      REQUIRE(islands.size() == 1);
      CHECK(islands[0].contact_points.begin() == contact_points.data());
      CHECK(islands[0].contact_points.end() == contact_points.data() + 12);
      CHECK(islands[0].range.begin.edge_index == 0);
      CHECK(islands[0].range.begin.x == flip_x_if_necessary(winding, ScalarDeg1(-2.50)));
      CHECK(islands[0].range.end.edge_index == 7);
      CHECK(islands[0].range.end.x == flip_x_if_necessary(winding, ScalarDeg1(-2.50)));
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
      flip_horizontally(vertices_storage);
      flip_horizontally(nodes);
    }

    std::vector<VerticalExtensionContactPoint> contact_points =
        vertical_extension_contact_points(chain_decomposition, winding);
    REQUIRE(contact_points.size() == 14);

    std::vector<ChainDecompositionIsland> islands =
        split_chain_decomposition_into_islands(vertices, winding, chain_decomposition, contact_points);

    REQUIRE(islands.size() == 3);

    CHECK(islands[0].contact_points.begin() == contact_points.data());
    CHECK(islands[0].contact_points.end() == contact_points.data() + 3);
    CHECK(islands[0].range.begin.edge_index == 0);
    CHECK(islands[0].range.begin.x == flip_x_if_necessary(winding, ScalarDeg1(-3.60)));
    CHECK(islands[0].range.end.edge_index == 2);
    CHECK(islands[0].range.end.x == flip_x_if_necessary(winding, ScalarDeg1(-1.28)));

    CHECK(islands[1].contact_points.begin() == contact_points.data() + 4);
    CHECK(islands[1].contact_points.end() == contact_points.data() + 10);
    CHECK(islands[1].range.begin.edge_index == 2);
    CHECK(islands[1].range.begin.x == flip_x_if_necessary(winding, ScalarDeg1(-1.28)));
    CHECK(islands[1].range.end.edge_index == 5);
    CHECK(islands[1].range.end.x == flip_x_if_necessary(winding, ScalarDeg1(1.24)));

    CHECK(islands[2].contact_points.begin() == contact_points.data() + 11);
    CHECK(islands[2].contact_points.end() == contact_points.data() + 14);
    CHECK(islands[2].range.begin.edge_index == 5);
    CHECK(islands[2].range.begin.x == flip_x_if_necessary(winding, ScalarDeg1(1.24)));
    CHECK(islands[2].range.end.edge_index == 7);
    CHECK(islands[2].range.end.x == flip_x_if_necessary(winding, ScalarDeg1(2.64)));
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
      flip_horizontally(vertices_storage);
      flip_horizontally(nodes);
    }

    std::vector<VerticalExtensionContactPoint> contact_points =
        vertical_extension_contact_points(chain_decomposition, winding);
    REQUIRE(contact_points.size() == 12);

    std::vector<ChainDecompositionIsland> islands =
        split_chain_decomposition_into_islands(vertices, winding, chain_decomposition, contact_points);

    REQUIRE(islands.size() == 3);

    CHECK(islands[0].contact_points.begin() == contact_points.data() + 2);
    CHECK(islands[0].contact_points.end() == contact_points.data() + 3);
    CHECK(islands[0].range.begin.edge_index == 1);
    CHECK(islands[0].range.begin.x == flip_x_if_necessary(winding, ScalarDeg1(6.24)));
    CHECK(islands[0].range.end.edge_index == 3);
    CHECK(islands[0].range.end.x == flip_x_if_necessary(winding, ScalarDeg1(6.24)));

    CHECK(islands[1].contact_points.begin() == contact_points.data() + 5);
    CHECK(islands[1].contact_points.end() == contact_points.data() + 6);
    CHECK(islands[1].range.begin.edge_index == 3);
    CHECK(islands[1].range.begin.x == flip_x_if_necessary(winding, ScalarDeg1(6.24)));
    CHECK(islands[1].range.end.edge_index == 5);
    CHECK(islands[1].range.end.x == flip_x_if_necessary(winding, ScalarDeg1(6.24)));

    CHECK(islands[2].contact_points.begin() == contact_points.data() + 8);
    CHECK(islands[2].contact_points.end() == contact_points.data() + 12);
    CHECK(islands[2].range.begin.edge_index == 5);
    CHECK(islands[2].range.begin.x == flip_x_if_necessary(winding, ScalarDeg1(6.24)));
    CHECK(islands[2].range.end.edge_index == 9);
    CHECK(islands[2].range.end.x == flip_x_if_necessary(winding, ScalarDeg1(6.24)));
  }
}

} // namespace dida::detail::vertical_decomposition