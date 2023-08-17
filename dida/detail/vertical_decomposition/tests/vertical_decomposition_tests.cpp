#include "dida/detail/vertical_decomposition/vertical_decomposition.hpp"

#include <catch2/catch.hpp>

#include "dida/detail/vertical_decomposition/sweep_line_builder.hpp"

namespace dida::detail::vertical_decomposition
{

TEST_CASE("y_on_edge_for_x")
{
  SECTION("Edge start left of edge end")
  {
    YOnEdge y = y_on_edge_for_x(Segment2({-3, 4}, {13, 0}), ScalarDeg1(9));
    CHECK(y == ScalarDeg1(1));
  }

  SECTION("Edge start right of edge end")
  {
    YOnEdge y = y_on_edge_for_x(Segment2({8, 5}, {2, 2}), ScalarDeg1(4));
    CHECK(y == ScalarDeg1(3));
  }
}

TEST_CASE("other_direction")
{
  CHECK(other_direction(HorizontalDirection::left) == HorizontalDirection::right);
  CHECK(other_direction(HorizontalDirection::right) == HorizontalDirection::left);
}

TEST_CASE("lex_less_than_with_direction")
{
  SECTION("x different")
  {
    Point2 a(1, 4);
    Point2 b(2, 3);

    CHECK(lex_less_than_with_direction<HorizontalDirection::right>(a, b));
    CHECK_FALSE(lex_less_than_with_direction<HorizontalDirection::left>(a, b));

    CHECK_FALSE(lex_less_than_with_direction<HorizontalDirection::right>(b, a));
    CHECK(lex_less_than_with_direction<HorizontalDirection::left>(b, a));
  }

  SECTION("Y different")
  {
    Point2 a(4, 1);
    Point2 b(4, 2);

    CHECK(lex_less_than_with_direction<HorizontalDirection::right>(a, b));
    CHECK_FALSE(lex_less_than_with_direction<HorizontalDirection::left>(a, b));

    CHECK_FALSE(lex_less_than_with_direction<HorizontalDirection::right>(b, a));
    CHECK(lex_less_than_with_direction<HorizontalDirection::left>(b, a));
  }

  SECTION("Equal")
  {
    Point2 a(4, 1);
    CHECK_FALSE(lex_less_than_with_direction<HorizontalDirection::right>(a, a));
    CHECK_FALSE(lex_less_than_with_direction<HorizontalDirection::left>(a, a));
  }
}

TEST_CASE("Edge::edge_from_index")
{
  std::vector<Point2> vertices_storage{{1.64, 2.04}, {4.52, 1.74}, {5.92, 4.52}, {0.50, 6.34}};
  VerticesView vertices(vertices_storage);

  SECTION("General")
  {
    Edge edge = Edge::edge_from_index(vertices, 1);
    CHECK(edge.start_vertex_it == vertices.begin() + 1);
    CHECK(edge.end_vertex_it == vertices.begin() + 2);
  }

  SECTION("With wrap")
  {
    Edge edge = Edge::edge_from_index(vertices, 3);
    CHECK(edge.start_vertex_it == vertices.begin() + 3);
    CHECK(edge.end_vertex_it == vertices.begin() + 0);
  }
}

TEST_CASE("Edge::invalid")
{
  Edge invalid_edge = Edge::invalid();
  CHECK(invalid_edge.start_vertex_it == nullptr);
  CHECK(invalid_edge.end_vertex_it == nullptr);
}

TEST_CASE("Edge::is_valid")
{
  SECTION("Valid")
  {
    std::vector<Point2> vertices_storage{{1.64, 2.04}, {4.52, 1.74}, {5.92, 4.52}, {0.50, 6.34}};
    VerticesView vertices(vertices_storage);

    Edge edge = Edge::edge_from_index(vertices, 1);
    CHECK(edge.is_valid());
  }

  SECTION("Invalid")
  {
    Edge edge = Edge::invalid();
    CHECK(!edge.is_valid());
  }
}

TEST_CASE("Edge::segment")
{
  std::vector<Point2> vertices_storage{{1.64, 2.04}, {4.52, 1.74}, {5.92, 4.52}, {0.50, 6.34}};
  VerticesView vertices(vertices_storage);
  Edge edge = Edge::edge_from_index(vertices, 0);
  CHECK(edge.segment() == Segment2(vertices[0], vertices[1]));
}

TEST_CASE("Edge::on_interior_side")
{
  std::vector<Point2> vertices_storage{{2, 2}, {8, 5}, {-2, 3}};
  VerticesView vertices(vertices_storage);

  Edge edge_0 = Edge::edge_from_index(vertices, 0);
  CHECK_FALSE(edge_0.on_interior_side({5, 2}));
  CHECK_FALSE(edge_0.on_interior_side({4, 3}));
  CHECK(edge_0.on_interior_side({2, 3}));

  Edge edge_1 = Edge::edge_from_index(vertices, 1);
  CHECK_FALSE(edge_1.on_interior_side({5, 6}));
  CHECK_FALSE(edge_1.on_interior_side({3, 4}));
  CHECK(edge_1.on_interior_side({2, 3}));
}

TEST_CASE("Edge::on_exterior_side")
{
  std::vector<Point2> vertices_storage{{2, 2}, {8, 5}, {-2, 3}};
  VerticesView vertices(vertices_storage);

  Edge edge_0 = Edge::edge_from_index(vertices, 0);
  CHECK(edge_0.on_exterior_side({5, 2}));
  CHECK_FALSE(edge_0.on_exterior_side({4, 3}));
  CHECK_FALSE(edge_0.on_exterior_side({2, 3}));

  Edge edge_1 = Edge::edge_from_index(vertices, 1);
  CHECK(edge_1.on_exterior_side({5, 6}));
  CHECK_FALSE(edge_1.on_exterior_side({3, 4}));
  CHECK_FALSE(edge_1.on_exterior_side({2, 3}));
}

TEST_CASE("Edge::operator==/!=")
{
  std::vector<Point2> vertices_storage{{1.64, 2.04}, {4.52, 1.74}, {5.92, 4.52}, {0.50, 6.34}};
  VerticesView vertices(vertices_storage);

  SECTION("Equal")
  {
    Edge a{vertices.begin(), vertices.begin() + 1};
    Edge b{vertices.begin(), vertices.begin() + 1};
    CHECK(a == b);
    CHECK_FALSE(a != b);
  }

  SECTION("start_vertex_it different")
  {
    Edge a{vertices.begin(), vertices.begin() + 1};
    Edge b{vertices.begin() + 2, vertices.begin() + 1};
    CHECK_FALSE(a == b);
    CHECK(a != b);
  }

  SECTION("end_vertex_it different")
  {
    Edge a{vertices.begin(), vertices.begin() + 1};
    Edge b{vertices.begin(), vertices.begin() + 2};
    CHECK_FALSE(a == b);
    CHECK(a != b);
  }
}

TEST_CASE("Node::neighbor_branch_index")
{
  Node nodes[4];

  nodes[0].neighbors[0] = &nodes[1];
  nodes[0].neighbors[1] = &nodes[2];
  nodes[0].neighbors[2] = &nodes[3];

  CHECK(nodes[0].neighbor_branch_index(&nodes[1]) == 0);
  CHECK(nodes[0].neighbor_branch_index(&nodes[2]) == 1);
  CHECK(nodes[0].neighbor_branch_index(&nodes[3]) == 2);
}

TEST_CASE("Node::replace_neighbor")
{
  Node nodes[5];
  nodes[0].neighbors[0] = &nodes[1];
  nodes[0].neighbors[1] = &nodes[2];
  nodes[0].neighbors[2] = &nodes[3];

  SECTION("Replace neighbor 0")
  {
    nodes[0].replace_neighbor(&nodes[1], &nodes[4]);
    CHECK(nodes[0].neighbors[0] == &nodes[4]);
    CHECK(nodes[0].neighbors[1] == &nodes[2]);
    CHECK(nodes[0].neighbors[2] == &nodes[3]);
  }

  SECTION("Replace neighbor 1")
  {
    nodes[0].replace_neighbor(&nodes[2], &nodes[4]);
    CHECK(nodes[0].neighbors[0] == &nodes[1]);
    CHECK(nodes[0].neighbors[1] == &nodes[4]);
    CHECK(nodes[0].neighbors[2] == &nodes[3]);
  }

  SECTION("Replace neighbor 2")
  {
    nodes[0].replace_neighbor(&nodes[3], &nodes[4]);
    CHECK(nodes[0].neighbors[0] == &nodes[1]);
    CHECK(nodes[0].neighbors[1] == &nodes[2]);
    CHECK(nodes[0].neighbors[2] == &nodes[4]);
  }
}

TEST_CASE("EdgeRange::invalid")
{
  EdgeRange range = EdgeRange::invalid();
  CHECK(range.start_vertex_it == nullptr);
  CHECK(range.end_vertex_it == nullptr);
}

TEST_CASE("EdgeRange::is_valid")
{
  SECTION("Valid")
  {
    std::vector<Point2> vertices_storage{{3.46, 4.52}, {8.48, 3.62}, {7.16, 5.80}};
    VerticesView vertices(vertices_storage);
    EdgeRange range{vertices.begin(), vertices.begin() + 2};
    CHECK(range.is_valid());
  }

  SECTION("Invalid")
  {
    EdgeRange range = EdgeRange::invalid();
    CHECK(!range.is_valid());
  }
}

TEST_CASE("edge_for_point_with_monotone_edge_range")
{
  std::vector<Point2> vertices_storage{
      {-3.62, 2.84}, {-0.14, 1.78}, {2.18, 3.26},  {5.32, 2.84},  {7.84, 3.86},  {6.56, 5.00},
      {1.36, 7.20},  {-1.88, 6.06}, {-5.90, 4.88}, {-7.98, 5.74}, {-9.48, 3.98}, {-7.28, 2.50},
  };
  VerticesView vertices(vertices_storage);

  VertexIt leftmost_vertex_it = vertices.begin() + 10;
  VertexIt rightmost_vertex_it = vertices.begin() + 4;

  SECTION("Towards right")
  {
    EdgeRange edge_range{leftmost_vertex_it, rightmost_vertex_it};

    SECTION("General")
    {
      CHECK(edge_for_point_with_monotone_edge_range<HorizontalDirection::right>(vertices, edge_range, {-5.24, 1.42}) ==
            Edge::edge_from_index(vertices, 11));
      CHECK(edge_for_point_with_monotone_edge_range<HorizontalDirection::right>(vertices, edge_range, {3.38, 2.18}) ==
            Edge::edge_from_index(vertices, 2));
    }

    SECTION("x on vertex, y different")
    {
      CHECK(edge_for_point_with_monotone_edge_range<HorizontalDirection::right>(vertices, edge_range, {-7.28, 1.5}) ==
            Edge::edge_from_index(vertices, 10));
      CHECK(edge_for_point_with_monotone_edge_range<HorizontalDirection::right>(vertices, edge_range, {-7.28, 3.5}) ==
            Edge::edge_from_index(vertices, 11));
    }

    SECTION("On vertex")
    {
      CHECK(edge_for_point_with_monotone_edge_range<HorizontalDirection::right>(vertices, edge_range, {-7.28, 2.5}) ==
            Edge::edge_from_index(vertices, 11));
    }
  }

  SECTION("Towards left")
  {
    EdgeRange edge_range{rightmost_vertex_it, leftmost_vertex_it};

    SECTION("General")
    {
      CHECK(edge_for_point_with_monotone_edge_range<HorizontalDirection::left>(vertices, edge_range, {-0.04, 8.08}) ==
            Edge::edge_from_index(vertices, 6));
      CHECK(edge_for_point_with_monotone_edge_range<HorizontalDirection::left>(vertices, edge_range, {-7.01, 6.89}) ==
            Edge::edge_from_index(vertices, 8));
    }

    SECTION("x on vertex, y different")
    {
      CHECK(edge_for_point_with_monotone_edge_range<HorizontalDirection::left>(vertices, edge_range, {-7.98, 6.74}) ==
            Edge::edge_from_index(vertices, 8));
      CHECK(edge_for_point_with_monotone_edge_range<HorizontalDirection::left>(vertices, edge_range, {-7.98, 4.74}) ==
            Edge::edge_from_index(vertices, 9));
    }

    SECTION("On vertex")
    {
      CHECK(edge_for_point_with_monotone_edge_range<HorizontalDirection::left>(vertices, edge_range, {-7.98, 5.74}) ==
            Edge::edge_from_index(vertices, 9));
    }
  }
}

TEST_CASE("Region::operator==")
{
  std::vector<Point2> vertices_storage{
      {-5.42, 2.82}, {3.92, 3.62}, {2.26, 5.74}, {3.74, 7.82}, {-4.52, 6.74}, {-1.10, 4.48},
  };
  VerticesView vertices(vertices_storage);

  VerticalDecomposition vd =
      vertical_decomposition_with_sweep_line_builder(vertices, VerticalDecompositionType::interior_decomposition);

  Region a{&vd.nodes[2], &vd.nodes[3], 0, 0};

  CHECK(a == a);
  CHECK_FALSE(a == Region{&vd.nodes[1], &vd.nodes[3], 0, 0});
  CHECK_FALSE(a == Region{&vd.nodes[2], &vd.nodes[4], 0, 0});
  CHECK_FALSE(a == Region{&vd.nodes[2], &vd.nodes[3], 1, 0});
  CHECK_FALSE(a == Region{&vd.nodes[2], &vd.nodes[3], 0, 1});
}

TEST_CASE("Region::lower_boundary")
{
  SECTION("Interior decomposition")
  {
    std::vector<Point2> vertices_storage{
        {-5.96, 3.04}, {-2.12, 3.26}, {-4.12, 1.02}, {2.60, 1.02},  {0.32, 3.30}, {5.48, 3.26},  {3.74, 4.68},
        {6.72, 6.10},  {2.52, 5.98},  {4.22, 7.82},  {-2.06, 7.42}, {0.86, 5.44}, {-5.36, 6.26}, {-3.44, 4.58},
    };
    VerticesView vertices(vertices_storage);

    VerticalDecomposition vd =
        vertical_decomposition_with_sweep_line_builder(vertices, VerticalDecompositionType::interior_decomposition);

    SECTION("Left branch 0, right branch 2")
    {
      Region region{&vd.nodes[3], &vd.nodes[4], 0, 2};

      EdgeRange lower_boundary = region.lower_boundary(VerticalDecompositionType::interior_decomposition);
      CHECK(lower_boundary.start_vertex_it == vertices.begin());
      CHECK(lower_boundary.end_vertex_it == vertices.begin() + 1);

      EdgeRange upper_boundary = region.upper_boundary(VerticalDecompositionType::interior_decomposition);
      CHECK(upper_boundary.start_vertex_it == vertices.begin() + 11);
      CHECK(upper_boundary.end_vertex_it == vertices.begin() + 12);
    }

    SECTION("Left branch 1, right branch 0")
    {
      Region region{&vd.nodes[8], &vd.nodes[10], 1, 0};

      EdgeRange lower_boundary = region.lower_boundary(VerticalDecompositionType::interior_decomposition);
      CHECK(lower_boundary.start_vertex_it == vertices.begin() + 4);
      CHECK(lower_boundary.end_vertex_it == vertices.begin() + 5);

      EdgeRange upper_boundary = region.upper_boundary(VerticalDecompositionType::interior_decomposition);
      CHECK(upper_boundary.start_vertex_it == vertices.begin() + 7);
      CHECK(upper_boundary.end_vertex_it == vertices.begin() + 8);
    }

    SECTION("Left branch 2, right branch 1")
    {
      Region region{&vd.nodes[6], &vd.nodes[7], 2, 1};

      EdgeRange lower_boundary = region.lower_boundary(VerticalDecompositionType::interior_decomposition);
      CHECK(lower_boundary.start_vertex_it == vertices.begin() + 4);
      CHECK(lower_boundary.end_vertex_it == vertices.begin() + 5);

      EdgeRange upper_boundary = region.upper_boundary(VerticalDecompositionType::interior_decomposition);
      CHECK(upper_boundary.start_vertex_it == vertices.begin() + 11);
      CHECK(upper_boundary.end_vertex_it == vertices.begin() + 12);
    }

    SECTION("Left leaf, right branch 1")
    {
      Region region{&vd.nodes[2], &vd.nodes[4], 0, 1};

      EdgeRange lower_boundary = region.lower_boundary(VerticalDecompositionType::interior_decomposition);
      CHECK(lower_boundary.start_vertex_it == vertices.begin() + 2);
      CHECK(lower_boundary.end_vertex_it == vertices.begin() + 3);

      EdgeRange upper_boundary = region.upper_boundary(VerticalDecompositionType::interior_decomposition);
      CHECK(upper_boundary.start_vertex_it == vertices.begin() + 1);
      CHECK(upper_boundary.end_vertex_it == vertices.begin() + 2);
    }

    SECTION("Left branch 2, right leaf")
    {
      Region region{&vd.nodes[10], &vd.nodes[13], 2, 0};

      EdgeRange lower_boundary = region.lower_boundary(VerticalDecompositionType::interior_decomposition);
      CHECK(lower_boundary.start_vertex_it == vertices.begin() + 6);
      CHECK(lower_boundary.end_vertex_it == vertices.begin() + 7);

      EdgeRange upper_boundary = region.upper_boundary(VerticalDecompositionType::interior_decomposition);
      CHECK(upper_boundary.start_vertex_it == vertices.begin() + 7);
      CHECK(upper_boundary.end_vertex_it == vertices.begin() + 8);
    }
  }

  SECTION("Exterior decomposition")
  {
    SECTION("No lower boundary")
    {
      std::vector<Point2> vertices_storage{
          {0.32, -0.88}, {1.80, -1.18}, {3.96, -1.90},  {3.26, -1.00}, {2.26, -0.30}, {4.24, -0.26},
          {5.16, -0.52}, {4.48, 0.26},  {3.04, 1.00},   {6.34, 1.64},  {10.56, 0.86}, {9.18, 0.46},
          {7.90, -0.58}, {9.02, -0.12}, {10.12, -0.04}, {9.44, -0.68}, {9.16, -2.26}, {10.08, -1.42},
          {12.84, 0.64}, {10.28, 2.06}, {6.60, 2.94},   {1.96, 1.56},
      };
      VerticesView vertices(vertices_storage);

      VerticalDecomposition vd =
          vertical_decomposition_with_sweep_line_builder(vertices, VerticalDecompositionType::exterior_decomposition);

      SECTION("Left neighbor 0, right neighbor 1")
      {
        Region region{&vd.nodes[3], &vd.nodes[4], 0, 1};

        EdgeRange lower_boundary = region.lower_boundary(VerticalDecompositionType::exterior_decomposition);
        CHECK(lower_boundary.start_vertex_it == nullptr);
        CHECK(lower_boundary.end_vertex_it == nullptr);

        EdgeRange upper_boundary = region.upper_boundary(VerticalDecompositionType::exterior_decomposition);
        CHECK(upper_boundary.start_vertex_it == vertices.begin() + 4);
        CHECK(upper_boundary.end_vertex_it == vertices.begin() + 6);
      }

      SECTION("Left neighbor 1, right neighbor 0")
      {
        Region region{&vd.nodes[5], &vd.nodes[6], 1, 0};

        EdgeRange lower_boundary = region.lower_boundary(VerticalDecompositionType::exterior_decomposition);
        CHECK(lower_boundary.start_vertex_it == nullptr);
        CHECK(lower_boundary.end_vertex_it == nullptr);

        EdgeRange upper_boundary = region.upper_boundary(VerticalDecompositionType::exterior_decomposition);
        CHECK(upper_boundary.start_vertex_it == vertices.begin() + 12);
        CHECK(upper_boundary.end_vertex_it == vertices.begin() + 14);
      }
    }

    SECTION("No upper boundary")
    {
      std::vector<Point2> vertices_storage{
          {-0.32, 0.88},   {-1.80, 1.18},   {-3.96, 1.90},  {-3.26, 1.00},  {-2.26, 0.30},   {-4.24, 0.26},
          {-5.16, 0.52},   {-4.48, -0.26},  {-3.04, -1.00}, {-6.34, -1.64}, {-10.56, -0.86}, {-9.18, -0.46},
          {-7.90, 0.58},   {-9.02, 0.12},   {-10.12, 0.04}, {-9.44, 0.68},  {-9.16, 2.26},   {-10.08, 1.42},
          {-12.84, -0.64}, {-10.28, -2.06}, {-6.60, -2.94}, {-1.96, -1.56},
      };
      VerticesView vertices(vertices_storage);

      VerticalDecomposition vd =
          vertical_decomposition_with_sweep_line_builder(vertices, VerticalDecompositionType::exterior_decomposition);

      SECTION("Left neighbor 0, right neighbor 2")
      {
        Region region{&vd.nodes[3], &vd.nodes[4], 0, 2};

        EdgeRange lower_boundary = region.lower_boundary(VerticalDecompositionType::exterior_decomposition);
        CHECK(lower_boundary.start_vertex_it == vertices.begin() + 12);
        CHECK(lower_boundary.end_vertex_it == vertices.begin() + 14);

        EdgeRange upper_boundary = region.upper_boundary(VerticalDecompositionType::exterior_decomposition);
        CHECK(upper_boundary.start_vertex_it == nullptr);
        CHECK(upper_boundary.end_vertex_it == nullptr);
      }

      SECTION("Left neighbor 2, right neighbor 0")
      {
        Region region{&vd.nodes[5], &vd.nodes[6], 2, 0};

        EdgeRange lower_boundary = region.lower_boundary(VerticalDecompositionType::exterior_decomposition);
        CHECK(lower_boundary.start_vertex_it == vertices.begin() + 4);
        CHECK(lower_boundary.end_vertex_it == vertices.begin() + 6);

        EdgeRange upper_boundary = region.upper_boundary(VerticalDecompositionType::exterior_decomposition);
        CHECK(upper_boundary.start_vertex_it == nullptr);
        CHECK(upper_boundary.end_vertex_it == nullptr);
      }
    }
  }
}

namespace
{

/// Validates that the set of regions produced with a @c RegionIterator is the same, up to a rotation of the region
/// list, regardless of the node passed to the @c RegionIterator constructor.
///
/// @param vd The vertical decomposition to use to validate the @c RegionIterator.
void validate_region_iterator_with_each_start_node(const VerticalDecomposition& vd)
{
  std::vector<Region> expected_regions;

  RegionIterator ref_iterator(&vd.nodes[0]);
  do
  {
    expected_regions.push_back(ref_iterator.region());
  } while (ref_iterator.move_next());

  for (size_t i = 1; i < vd.nodes.size(); i++)
  {
    RegionIterator iterator(&vd.nodes[i]);

    std::vector<Region>::iterator expected_regions_it =
        std::find(expected_regions.begin(), expected_regions.end(), iterator.region());

    for (size_t j = 1; j < expected_regions.size(); j++)
    {
      REQUIRE(iterator.move_next());
      expected_regions_it = next_cyclic(expected_regions, expected_regions_it);

      CHECK(iterator.region() == *expected_regions_it);
    }

    CHECK(!iterator.move_next());
  }
}

} // namespace

TEST_CASE("RegionIterator")
{
  SECTION("Interior")
  {
    std::vector<Point2> vertices_storage{{2.16, 1.54},  {15.28, 1.80}, {13.70, 3.04}, {15.14, 3.96}, {10.14, 5.00},
                                         {13.02, 7.12}, {12.84, 5.96}, {15.02, 8.46}, {-0.62, 8.72}, {2.02, 6.58},
                                         {1.26, 7.88},  {5.86, 5.18},  {2.02, 3.86},  {4.90, 3.02}};

    VerticesView vertices(vertices_storage);
    VerticalDecomposition vd =
        vertical_decomposition_with_sweep_line_builder(vertices, VerticalDecompositionType::interior_decomposition);

    SECTION("Start with rightward non leaf node")
    {
      RegionIterator iterator(&vd.nodes[1]);

      CHECK(iterator.region().left_node == &vd.nodes[1]);
      CHECK(iterator.region().right_node == &vd.nodes[6]);
      CHECK(iterator.region().left_node_branch_index == 2);
      CHECK(iterator.region().right_node_branch_index == 2);

      REQUIRE(iterator.move_next());
      CHECK(iterator.region().left_node == &vd.nodes[2]);
      CHECK(iterator.region().right_node == &vd.nodes[5]);
      CHECK(iterator.region().left_node_branch_index == 0);
      CHECK(iterator.region().right_node_branch_index == 2);

      REQUIRE(iterator.move_next());
      CHECK(iterator.region().left_node == &vd.nodes[4]);
      CHECK(iterator.region().right_node == &vd.nodes[5]);
      CHECK(iterator.region().left_node_branch_index == 0);
      CHECK(iterator.region().right_node_branch_index == 1);

      REQUIRE(iterator.move_next());
      CHECK(iterator.region().left_node == &vd.nodes[5]);
      CHECK(iterator.region().right_node == &vd.nodes[6]);
      CHECK(iterator.region().left_node_branch_index == 0);
      CHECK(iterator.region().right_node_branch_index == 1);

      REQUIRE(iterator.move_next());
      CHECK(iterator.region().left_node == &vd.nodes[6]);
      CHECK(iterator.region().right_node == &vd.nodes[7]);
      CHECK(iterator.region().left_node_branch_index == 0);
      CHECK(iterator.region().right_node_branch_index == 0);

      REQUIRE(iterator.move_next());
      CHECK(iterator.region().left_node == &vd.nodes[7]);
      CHECK(iterator.region().right_node == &vd.nodes[10]);
      CHECK(iterator.region().left_node_branch_index == 1);
      CHECK(iterator.region().right_node_branch_index == 0);

      REQUIRE(iterator.move_next());
      CHECK(iterator.region().left_node == &vd.nodes[10]);
      CHECK(iterator.region().right_node == &vd.nodes[13]);
      CHECK(iterator.region().left_node_branch_index == 1);
      CHECK(iterator.region().right_node_branch_index == 0);

      REQUIRE(iterator.move_next());
      CHECK(iterator.region().left_node == &vd.nodes[10]);
      CHECK(iterator.region().right_node == &vd.nodes[12]);
      CHECK(iterator.region().left_node_branch_index == 2);
      CHECK(iterator.region().right_node_branch_index == 0);

      REQUIRE(iterator.move_next());
      CHECK(iterator.region().left_node == &vd.nodes[7]);
      CHECK(iterator.region().right_node == &vd.nodes[9]);
      CHECK(iterator.region().left_node_branch_index == 2);
      CHECK(iterator.region().right_node_branch_index == 2);

      REQUIRE(iterator.move_next());
      CHECK(iterator.region().left_node == &vd.nodes[8]);
      CHECK(iterator.region().right_node == &vd.nodes[9]);
      CHECK(iterator.region().left_node_branch_index == 0);
      CHECK(iterator.region().right_node_branch_index == 1);

      REQUIRE(iterator.move_next());
      CHECK(iterator.region().left_node == &vd.nodes[9]);
      CHECK(iterator.region().right_node == &vd.nodes[11]);
      CHECK(iterator.region().left_node_branch_index == 0);
      CHECK(iterator.region().right_node_branch_index == 0);

      REQUIRE(iterator.move_next());
      CHECK(iterator.region().left_node == &vd.nodes[0]);
      CHECK(iterator.region().right_node == &vd.nodes[1]);
      CHECK(iterator.region().left_node_branch_index == 0);
      CHECK(iterator.region().right_node_branch_index == 0);

      REQUIRE(iterator.move_next());
      CHECK(iterator.region().left_node == &vd.nodes[1]);
      CHECK(iterator.region().right_node == &vd.nodes[3]);
      CHECK(iterator.region().left_node_branch_index == 1);
      CHECK(iterator.region().right_node_branch_index == 0);

      CHECK(!iterator.move_next());
    }

    SECTION("Starts with rightward leaf node")
    {
      RegionIterator iterator(&vd.nodes[3]);

      CHECK(iterator.region().left_node == &vd.nodes[1]);
      CHECK(iterator.region().right_node == &vd.nodes[6]);
      CHECK(iterator.region().left_node_branch_index == 2);
      CHECK(iterator.region().right_node_branch_index == 2);

      REQUIRE(iterator.move_next());
      CHECK(iterator.region().left_node == &vd.nodes[2]);
      CHECK(iterator.region().right_node == &vd.nodes[5]);
      CHECK(iterator.region().left_node_branch_index == 0);
      CHECK(iterator.region().right_node_branch_index == 2);

      // The rest is the same as in "Start with rightward non leaf node" section, so no need to test it again.
    }

    SECTION("Starts with leftward non leaf node")
    {
      RegionIterator iterator(&vd.nodes[6]);

      CHECK(iterator.region().left_node == &vd.nodes[2]);
      CHECK(iterator.region().right_node == &vd.nodes[5]);
      CHECK(iterator.region().left_node_branch_index == 0);
      CHECK(iterator.region().right_node_branch_index == 2);

      REQUIRE(iterator.move_next());
      CHECK(iterator.region().left_node == &vd.nodes[4]);
      CHECK(iterator.region().right_node == &vd.nodes[5]);
      CHECK(iterator.region().left_node_branch_index == 0);
      CHECK(iterator.region().right_node_branch_index == 1);

      // The rest belongs to the general case, so no need to test it again.
    }

    SECTION("Starts with lefward leaf node")
    {
      RegionIterator iterator(&vd.nodes[5]);

      CHECK(iterator.region().left_node == &vd.nodes[4]);
      CHECK(iterator.region().right_node == &vd.nodes[5]);
      CHECK(iterator.region().left_node_branch_index == 0);
      CHECK(iterator.region().right_node_branch_index == 1);

      REQUIRE(iterator.move_next());
      CHECK(iterator.region().left_node == &vd.nodes[5]);
      CHECK(iterator.region().right_node == &vd.nodes[6]);
      CHECK(iterator.region().left_node_branch_index == 0);
      CHECK(iterator.region().right_node_branch_index == 1);

      // The rest belongs to the general case, so no need to test it again.
    }

    SECTION("Each start node produces the same regions")
    {
      validate_region_iterator_with_each_start_node(vd);
    }
  }

  SECTION("Exterior")
  {
    std::vector<Point2> vertices_storage{
        {-2.72, 2.30}, {2.84, -2.38}, {2.04, 1.30}, {8.40, 1.34}, {7.36, -0.96},
        {12.62, 2.52}, {6.08, 6.02},  {8.28, 2.96}, {0.42, 3.28}, {1.88, 7.18},
    };

    VerticesView vertices(vertices_storage);
    VerticalDecomposition vd =
        vertical_decomposition_with_sweep_line_builder(vertices, VerticalDecompositionType::exterior_decomposition);

    SECTION("Start at node[0]")
    {
      RegionIterator iterator(&vd.nodes[0]);

      CHECK(iterator.region().left_node == &vd.nodes[0]);
      CHECK(iterator.region().right_node == &vd.nodes[2]);
      CHECK(iterator.region().left_node_branch_index == 2);
      CHECK(iterator.region().right_node_branch_index == 2);

      REQUIRE(iterator.move_next());
      CHECK(iterator.region().left_node == &vd.nodes[1]);
      CHECK(iterator.region().right_node == &vd.nodes[2]);
      CHECK(iterator.region().left_node_branch_index == 0);
      CHECK(iterator.region().right_node_branch_index == 1);

      REQUIRE(iterator.move_next());
      CHECK(iterator.region().left_node == &vd.nodes[2]);
      CHECK(iterator.region().right_node == &vd.nodes[5]);
      CHECK(iterator.region().left_node_branch_index == 0);
      CHECK(iterator.region().right_node_branch_index == 0);

      REQUIRE(iterator.move_next());
      CHECK(iterator.region().left_node == &vd.nodes[5]);
      CHECK(iterator.region().right_node == &vd.nodes[7]);
      CHECK(iterator.region().left_node_branch_index == 1);
      CHECK(iterator.region().right_node_branch_index == 0);

      REQUIRE(iterator.move_next());
      CHECK(iterator.region().left_node == &vd.nodes[5]);
      CHECK(iterator.region().right_node == &vd.nodes[9]);
      CHECK(iterator.region().left_node_branch_index == 2);
      CHECK(iterator.region().right_node_branch_index == 2);

      REQUIRE(iterator.move_next());
      CHECK(iterator.region().left_node == &vd.nodes[6]);
      CHECK(iterator.region().right_node == &vd.nodes[9]);
      CHECK(iterator.region().left_node_branch_index == 1);
      CHECK(iterator.region().right_node_branch_index == 1);

      REQUIRE(iterator.move_next());
      CHECK(iterator.region().left_node == &vd.nodes[6]);
      CHECK(iterator.region().right_node == &vd.nodes[8]);
      CHECK(iterator.region().left_node_branch_index == 2);
      CHECK(iterator.region().right_node_branch_index == 0);

      REQUIRE(iterator.move_next());
      CHECK(iterator.region().left_node == &vd.nodes[4]);
      CHECK(iterator.region().right_node == &vd.nodes[6]);
      CHECK(iterator.region().left_node_branch_index == 0);
      CHECK(iterator.region().right_node_branch_index == 0);

      REQUIRE(iterator.move_next());
      CHECK(iterator.region().left_node == &vd.nodes[3]);
      CHECK(iterator.region().right_node == &vd.nodes[4]);
      CHECK(iterator.region().left_node_branch_index == 0);
      CHECK(iterator.region().right_node_branch_index == 2);

      REQUIRE(iterator.move_next());
      CHECK(iterator.region().left_node == &vd.nodes[0]);
      CHECK(iterator.region().right_node == &vd.nodes[4]);
      CHECK(iterator.region().left_node_branch_index == 1);
      CHECK(iterator.region().right_node_branch_index == 1);

      CHECK(!iterator.move_next());
    }

    SECTION("Each start node produces the same regions")
    {
      validate_region_iterator_with_each_start_node(vd);
    }
  }
}

} // namespace dida::detail::vertical_decomposition