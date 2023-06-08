#include "dida/detail/vertical_decomposition.hpp"

#include <catch2/catch.hpp>

#include "dida/detail/vertical_decomposition_sweep_line_builder.hpp"

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

TEST_CASE("Edge::operator==")
{
  std::vector<Point2> vertices_storage{{1.64, 2.04}, {4.52, 1.74}, {5.92, 4.52}, {0.50, 6.34}};
  VerticesView vertices(vertices_storage);

  SECTION("Equal")
  {
    Edge a{vertices.begin(), vertices.begin() + 1};
    Edge b{vertices.begin(), vertices.begin() + 1};
    CHECK(a == b);
  }

  SECTION("start_vertex_it different")
  {
    Edge a{vertices.begin(), vertices.begin() + 1};
    Edge b{vertices.begin() + 2, vertices.begin() + 1};
    CHECK_FALSE(a == b);
  }

  SECTION("end_vertex_it different")
  {
    Edge a{vertices.begin(), vertices.begin() + 1};
    Edge b{vertices.begin(), vertices.begin() + 2};
    CHECK_FALSE(a == b);
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

  VerticalDecomposition vd;
  vd.nodes.resize(2);

  vd.nodes[0].direction = HorizontalDirection::left;
  vd.nodes[0].vertex_it = vertices.begin() + 5;
  vd.nodes[0].lower_opp_edge = Edge::edge_from_index(vertices, 0);
  vd.nodes[0].upper_opp_edge = Edge::edge_from_index(vertices, 3);
  vd.nodes[0].neighbors[0] = &vd.nodes[1];
  vd.nodes[0].neighbors[1] = nullptr;
  vd.nodes[0].neighbors[2] = nullptr;

  vd.nodes[1].direction = HorizontalDirection::right;
  vd.nodes[1].vertex_it = vertices.begin() + 2;
  vd.nodes[1].lower_opp_edge = Edge::edge_from_index(vertices, 0);
  vd.nodes[1].upper_opp_edge = Edge::edge_from_index(vertices, 3);
  vd.nodes[1].neighbors[0] = &vd.nodes[0];
  vd.nodes[1].neighbors[1] = nullptr;
  vd.nodes[1].neighbors[2] = nullptr;

  SECTION("With left and right node")
  {
    Region a{&vd.nodes[0], &vd.nodes[1], 0, 0};

    CHECK(a == a);
    CHECK_FALSE(a == Region{&vd.nodes[1], &vd.nodes[1], 0, 0});
    CHECK_FALSE(a == Region{&vd.nodes[0], &vd.nodes[0], 0, 0});
    CHECK_FALSE(a == Region{&vd.nodes[0], &vd.nodes[1], 1, 0});
    CHECK_FALSE(a == Region{&vd.nodes[0], &vd.nodes[1], 0, 1});
  }

  SECTION("No left node")
  {
    Region a{nullptr, &vd.nodes[0], 0, 0};

    CHECK(a == a);
    CHECK_FALSE(a == Region{&vd.nodes[0], &vd.nodes[0], 0, 0});
    CHECK_FALSE(a == Region{nullptr, &vd.nodes[1], 0, 0});
    CHECK(a == Region{nullptr, &vd.nodes[0], 1, 0});
    CHECK_FALSE(a == Region{nullptr, &vd.nodes[0], 0, 1});
  }

  SECTION("No right node")
  {
    Region a{&vd.nodes[1], nullptr, 0, 0};

    CHECK(a == a);
    CHECK_FALSE(a == Region{&vd.nodes[0], nullptr, 0, 0});
    CHECK_FALSE(a == Region{&vd.nodes[1], &vd.nodes[0], 0, 0});
    CHECK_FALSE(a == Region{&vd.nodes[1], nullptr, 1, 0});
    CHECK(a == Region{&vd.nodes[1], nullptr, 0, 1});
  }
}

TEST_CASE("Region::is_leaf")
{
  std::vector<Point2> vertices_storage{{-4.10, 2.96}, {5.48, 1.94},  {3.84, 4.36},
                                       {6.82, 8.06},  {-7.42, 6.68}, {-3.62, 5.78}};
  VerticesView vertices(vertices_storage);

  VerticalDecomposition vd =
      vertical_decomposition_with_sweep_line_builder(vertices, VerticalDecompositionType::interior_decomposition);

  SECTION("Left leaf")
  {
    Region region{nullptr, &vd.nodes[0], 0, 2};
    CHECK(region.is_leaf());
  }

  SECTION("Right leaf")
  {
    Region region{&vd.nodes[1], nullptr, 1, 0};
    CHECK(region.is_leaf());
  }

  SECTION("Not a leaf")
  {
    Region region{&vd.nodes[0], &vd.nodes[1], 0, 0};
    CHECK_FALSE(region.is_leaf());
  }
}

TEST_CASE("Region::boundary_edge_ranges")
{
  SECTION("Interior decomposition, non leaf nodes")
  {
    std::vector<Point2> vertices_storage{
        {-5.96, 3.04}, {-2.12, 3.26}, {-4.12, 1.02}, {2.60, 1.02},  {0.32, 3.30}, {5.48, 3.26},  {3.74, 4.68},
        {6.72, 6.10},  {2.52, 5.98},  {4.22, 7.82},  {-2.06, 7.42}, {0.86, 5.44}, {-5.36, 6.26}, {-3.44, 4.58},
    };
    VerticesView vertices(vertices_storage);

    VerticalDecomposition vd =
        vertical_decomposition_with_sweep_line_builder(vertices, VerticalDecompositionType::interior_decomposition);

    SECTION("Left neighbor 0, right neighbor 2")
    {
      Region region{&vd.nodes[0], &vd.nodes[1], 0, 2};

      Region::BoundaryEdgeRanges result =
          region.boundary_edge_ranges(vertices, VerticalDecompositionType::interior_decomposition);
      CHECK(result.lower.start_vertex_it == vertices.begin());
      CHECK(result.lower.end_vertex_it == vertices.begin() + 1);
      CHECK(result.upper.start_vertex_it == vertices.begin() + 11);
      CHECK(result.upper.end_vertex_it == vertices.begin() + 12);
    }

    SECTION("Left neighbor 1, right neighbor 0")
    {
      Region region{&vd.nodes[4], &vd.nodes[5], 1, 0};

      Region::BoundaryEdgeRanges result =
          region.boundary_edge_ranges(vertices, VerticalDecompositionType::interior_decomposition);
      CHECK(result.lower.start_vertex_it == vertices.begin() + 4);
      CHECK(result.lower.end_vertex_it == vertices.begin() + 5);
      CHECK(result.upper.start_vertex_it == vertices.begin() + 7);
      CHECK(result.upper.end_vertex_it == vertices.begin() + 8);
    }

    SECTION("Left neighbor 2, right neighbor 1")
    {
      Region region{&vd.nodes[2], &vd.nodes[3], 2, 1};

      Region::BoundaryEdgeRanges result =
          region.boundary_edge_ranges(vertices, VerticalDecompositionType::interior_decomposition);
      CHECK(result.lower.start_vertex_it == vertices.begin() + 4);
      CHECK(result.lower.end_vertex_it == vertices.begin() + 5);
      CHECK(result.upper.start_vertex_it == vertices.begin() + 11);
      CHECK(result.upper.end_vertex_it == vertices.begin() + 12);
    }
  }

  SECTION("Exterior decomposition, non leaf nodes")
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
        Region region{&vd.nodes[1], &vd.nodes[2], 0, 1};

        Region::BoundaryEdgeRanges result =
            region.boundary_edge_ranges(vertices, VerticalDecompositionType::exterior_decomposition);
        CHECK(result.lower.start_vertex_it == nullptr);
        CHECK(result.lower.end_vertex_it == nullptr);
        CHECK(result.upper.start_vertex_it == vertices.begin() + 4);
        CHECK(result.upper.end_vertex_it == vertices.begin() + 6);
      }

      SECTION("Left neighbor 1, right neighbor 0")
      {
        Region region{&vd.nodes[3], &vd.nodes[4], 1, 0};

        Region::BoundaryEdgeRanges result =
            region.boundary_edge_ranges(vertices, VerticalDecompositionType::exterior_decomposition);
        CHECK(result.lower.start_vertex_it == nullptr);
        CHECK(result.lower.end_vertex_it == nullptr);
        CHECK(result.upper.start_vertex_it == vertices.begin() + 12);
        CHECK(result.upper.end_vertex_it == vertices.begin() + 14);
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
        Region region{&vd.nodes[1], &vd.nodes[2], 0, 2};

        Region::BoundaryEdgeRanges result =
            region.boundary_edge_ranges(vertices, VerticalDecompositionType::exterior_decomposition);
        CHECK(result.lower.start_vertex_it == vertices.begin() + 12);
        CHECK(result.lower.end_vertex_it == vertices.begin() + 14);
        CHECK(result.upper.start_vertex_it == nullptr);
        CHECK(result.upper.end_vertex_it == nullptr);
      }

      SECTION("Left neighbor 2, right neighbor 0")
      {
        Region region{&vd.nodes[3], &vd.nodes[4], 2, 0};

        Region::BoundaryEdgeRanges result =
            region.boundary_edge_ranges(vertices, VerticalDecompositionType::exterior_decomposition);
        CHECK(result.lower.start_vertex_it == vertices.begin() + 4);
        CHECK(result.lower.end_vertex_it == vertices.begin() + 6);
        CHECK(result.upper.start_vertex_it == nullptr);
        CHECK(result.upper.end_vertex_it == nullptr);
      }
    }
  }

  SECTION("Interior decomposition, leaf nodes")
  {
    std::vector<Point2> vertices_storage{
        {-0.64, 4.20}, {1.56, 3.92},  {3.14, 3.92},  {1.90, 2.46},  {0.50, 2.08},  {1.30, 0.86},  {2.34, 0.18},
        {3.12, 1.08},  {4.52, 2.30},  {7.02, 2.30},  {7.66, 4.34},  {7.90, 8.18},  {7.18, 6.26},  {6.10, 7.22},
        {5.54, 6.16},  {2.68, 6.78},  {3.42, 7.98},  {4.68, 7.76},  {3.14, 9.10},  {1.24, 6.88},  {-0.24, 6.10},
        {-1.94, 6.82}, {-3.56, 8.84}, {-5.08, 9.20}, {-6.66, 8.52}, {-7.42, 7.82}, {-8.62, 7.36}, {-7.34, 6.70},
        {-6.54, 7.32}, {-5.30, 7.72}, {-3.78, 6.24}, {-2.10, 5.52}, {-4.82, 4.48}, {-6.88, 4.72}, {-7.80, 3.24},
        {-6.30, 2.40}, {-5.18, 3.12}, {-4.04, 2.60}, {-3.42, 3.50}, {-0.92, 0.78}, {2.44, -0.58}, {-0.16, 1.06},
        {-1.18, 2.74}, {-1.52, 2.50}, {-2.34, 3.68},
    };
    ArrayView<const Point2> vertices(vertices_storage);

    VerticalDecomposition vd =
        vertical_decomposition_with_sweep_line_builder(vertices, VerticalDecompositionType::interior_decomposition);

    SECTION("Leaf towards right, branch 0")
    {
      Region region{&vd.nodes[3], nullptr, 0, 0};
      Region::BoundaryEdgeRanges result =
          region.boundary_edge_ranges(vertices, VerticalDecompositionType::interior_decomposition);
      CHECK(result.lower.start_vertex_it == vertices.begin() + 7);
      CHECK(result.lower.end_vertex_it == vertices.begin() + 11);
      CHECK(result.upper.start_vertex_it == vertices.begin() + 11);
      CHECK(result.upper.end_vertex_it == vertices.begin() + 15);
    }

    SECTION("Leaf towards right, branch 1")
    {
      Region region{&vd.nodes[0], nullptr, 1, 0};
      Region::BoundaryEdgeRanges result =
          region.boundary_edge_ranges(vertices, VerticalDecompositionType::interior_decomposition);
      CHECK(result.lower.start_vertex_it == vertices.begin() + 38);
      CHECK(result.lower.end_vertex_it == vertices.begin() + 40);
      CHECK(result.upper.start_vertex_it == vertices.begin() + 40);
      CHECK(result.upper.end_vertex_it == vertices.begin() + 44);
    }

    SECTION("Leaf towards right, branch 2")
    {
      Region region{&vd.nodes[2], nullptr, 2, 0};
      Region::BoundaryEdgeRanges result =
          region.boundary_edge_ranges(vertices, VerticalDecompositionType::interior_decomposition);
      CHECK(result.lower.start_vertex_it == vertices.begin() + 15);
      CHECK(result.lower.end_vertex_it == vertices.begin() + 17);
      CHECK(result.upper.start_vertex_it == vertices.begin() + 17);
      CHECK(result.upper.end_vertex_it == vertices.begin() + 19);
    }

    SECTION("Leaf towards left, branch 0")
    {
      Region region{nullptr, &vd.nodes[0], 0, 0};
      Region::BoundaryEdgeRanges result =
          region.boundary_edge_ranges(vertices, VerticalDecompositionType::interior_decomposition);
      CHECK(result.lower.start_vertex_it == vertices.begin() + +34);
      CHECK(result.lower.end_vertex_it == vertices.begin() + 39);
      CHECK(result.upper.start_vertex_it == vertices.begin() + 31);
      CHECK(result.upper.end_vertex_it == vertices.begin() + +34);
    }

    SECTION("Leaf towards left, branch 1")
    {
      Region region{nullptr, &vd.nodes[3], 0, 1};
      Region::BoundaryEdgeRanges result =
          region.boundary_edge_ranges(vertices, VerticalDecompositionType::interior_decomposition);
      CHECK(result.lower.start_vertex_it == vertices.begin() + 4);
      CHECK(result.lower.end_vertex_it == vertices.begin() + 8);
      CHECK(result.upper.start_vertex_it == vertices.begin() + 2);
      CHECK(result.upper.end_vertex_it == vertices.begin() + 4);
    }

    SECTION("Leaf towards left, branch 2")
    {
      Region region{nullptr, &vd.nodes[1], 0, 2};
      Region::BoundaryEdgeRanges result =
          region.boundary_edge_ranges(vertices, VerticalDecompositionType::interior_decomposition);
      CHECK(result.lower.start_vertex_it == vertices.begin() + 26);
      CHECK(result.lower.end_vertex_it == vertices.begin() + 31);
      CHECK(result.upper.start_vertex_it == vertices.begin() + 21);
      CHECK(result.upper.end_vertex_it == vertices.begin() + 26);
    }
  }

  SECTION("Exterior decomposition, leaf nodes")
  {
    std::vector<Point2> vertices_storage{
        {-0.72, 6.20}, {0.78, 7.60},  {4.08, 7.08},   {2.14, 4.94},   {4.10, 3.58},   {2.28, 2.38},
        {0.56, 2.60},  {4.04, 0.88},  {5.90, 0.90},   {6.28, -0.80},  {3.50, -2.52},  {1.12, -2.54},
        {2.14, -0.76}, {0.34, -0.04}, {-1.94, -0.66}, {-0.94, -2.20}, {-3.92, -1.78}, {-4.66, 0.12},
        {-1.44, 1.32}, {-5.24, 2.56}, {-3.40, 3.50},  {-5.50, 4.72},  {-4.32, 7.46},  {-2.30, 6.34},
        {-2.66, 8.74}, {-6.32, 8.64}, {-5.60, -2.52}, {7.02, -2.80},  {6.82, 9.26},   {-0.72, 9.00},
    };

    ArrayView<const Point2> vertices(vertices_storage);

    VerticalDecomposition vd =
        vertical_decomposition_with_sweep_line_builder(vertices, VerticalDecompositionType::exterior_decomposition);

    SECTION("Leaf towards right, branch 0")
    {
      Region region{&vd.nodes[7], nullptr, 0, 0};
      Region::BoundaryEdgeRanges result =
          region.boundary_edge_ranges(vertices, VerticalDecompositionType::exterior_decomposition);
      CHECK(result.lower.start_vertex_it == vertices.begin() + 9);
      CHECK(result.lower.end_vertex_it == vertices.begin() + 11);
      CHECK(result.upper.start_vertex_it == vertices.begin() + 6);
      CHECK(result.upper.end_vertex_it == vertices.begin() + 9);
    }

    SECTION("Leaf towards right, branch 1")
    {
      Region region{&vd.nodes[8], nullptr, 1, 0};
      Region::BoundaryEdgeRanges result =
          region.boundary_edge_ranges(vertices, VerticalDecompositionType::exterior_decomposition);
      CHECK(result.lower.start_vertex_it == vertices.begin() + 4);
      CHECK(result.lower.end_vertex_it == vertices.begin() + 6);
      CHECK(result.upper.start_vertex_it == vertices.begin() + 3);
      CHECK(result.upper.end_vertex_it == vertices.begin() + 4);
    }

    SECTION("Leaf towards right, branch 2")
    {
      Region region{&vd.nodes[8], nullptr, 2, 0};
      Region::BoundaryEdgeRanges result =
          region.boundary_edge_ranges(vertices, VerticalDecompositionType::exterior_decomposition);
      CHECK(result.lower.start_vertex_it == vertices.begin() + 2);
      CHECK(result.lower.end_vertex_it == vertices.begin() + 3);
      CHECK(result.upper.start_vertex_it == vertices.begin() + 1);
      CHECK(result.upper.end_vertex_it == vertices.begin() + 2);
    }

    SECTION("Leaf towards left, branch 0")
    {
      Region region{nullptr, &vd.nodes[3], 0, 0};
      Region::BoundaryEdgeRanges result =
          region.boundary_edge_ranges(vertices, VerticalDecompositionType::exterior_decomposition);
      CHECK(result.lower.start_vertex_it == vertices.begin() + 15);
      CHECK(result.lower.end_vertex_it == vertices.begin() + 17);
      CHECK(result.upper.start_vertex_it == vertices.begin() + 17);
      CHECK(result.upper.end_vertex_it == vertices.begin() + 18);
    }

    SECTION("Leaf towards left, branch 1")
    {
      Region region{nullptr, &vd.nodes[1], 0, 1};
      Region::BoundaryEdgeRanges result =
          region.boundary_edge_ranges(vertices, VerticalDecompositionType::exterior_decomposition);
      CHECK(result.lower.start_vertex_it == vertices.begin() + 18);
      CHECK(result.lower.end_vertex_it == vertices.begin() + 19);
      CHECK(result.upper.start_vertex_it == vertices.begin() + 19);
      CHECK(result.upper.end_vertex_it == vertices.begin() + 20);
    }

    SECTION("Leaf towards left, branch 2")
    {
      Region region{nullptr, &vd.nodes[1], 0, 2};
      Region::BoundaryEdgeRanges result =
          region.boundary_edge_ranges(vertices, VerticalDecompositionType::exterior_decomposition);
      CHECK(result.lower.start_vertex_it == vertices.begin() + 20);
      CHECK(result.lower.end_vertex_it == vertices.begin() + 21);
      CHECK(result.upper.start_vertex_it == vertices.begin() + 21);
      CHECK(result.upper.end_vertex_it == vertices.begin() + 23);
    }

    SECTION("Unbounded leaf towards right")
    {
      Region region{&vd.nodes[9], nullptr, 0, 0};
      Region::BoundaryEdgeRanges result =
          region.boundary_edge_ranges(vertices, VerticalDecompositionType::exterior_decomposition);
      CHECK(result.lower.start_vertex_it == nullptr);
      CHECK(result.lower.end_vertex_it == nullptr);
      CHECK(result.upper.start_vertex_it == nullptr);
      CHECK(result.upper.end_vertex_it == nullptr);
    }

    SECTION("Unbounded leaf towards left")
    {
      Region region{nullptr, &vd.nodes[0], 0, 0};
      Region::BoundaryEdgeRanges result =
          region.boundary_edge_ranges(vertices, VerticalDecompositionType::exterior_decomposition);
      CHECK(result.lower.start_vertex_it == nullptr);
      CHECK(result.lower.end_vertex_it == nullptr);
      CHECK(result.upper.start_vertex_it == nullptr);
      CHECK(result.upper.end_vertex_it == nullptr);
    }
  }
}

namespace
{

/// Validates that the set of regions produced with a @c RegionIterator is the same, up to a rotation of the region
/// list, regardless of the node passed to the @c RegionIterator constructor.
///
/// @param vd The vertical decomposition to validate.
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

    VerticalDecomposition vd;
    vd.nodes.resize(6);

    vd.nodes[0].direction = HorizontalDirection::right;
    vd.nodes[0].vertex_it = vertices.begin() + 10;
    vd.nodes[0].lower_opp_edge = Edge::edge_from_index(vertices, 8);
    vd.nodes[0].upper_opp_edge = Edge::edge_from_index(vertices, 7);
    vd.nodes[0].neighbors[0] = nullptr;
    vd.nodes[0].neighbors[1] = nullptr;
    vd.nodes[0].neighbors[2] = &vd.nodes[2];

    vd.nodes[1].direction = HorizontalDirection::left;
    vd.nodes[1].vertex_it = vertices.begin() + 13;
    vd.nodes[1].lower_opp_edge = Edge::edge_from_index(vertices, 0);
    vd.nodes[1].upper_opp_edge = Edge::edge_from_index(vertices, 11);
    vd.nodes[1].neighbors[0] = &vd.nodes[2];
    vd.nodes[1].neighbors[1] = nullptr;
    vd.nodes[1].neighbors[2] = nullptr;

    vd.nodes[2].direction = HorizontalDirection::left;
    vd.nodes[2].vertex_it = vertices.begin() + 11;
    vd.nodes[2].lower_opp_edge = Edge::edge_from_index(vertices, 0);
    vd.nodes[2].upper_opp_edge = Edge::edge_from_index(vertices, 7);
    vd.nodes[2].neighbors[0] = &vd.nodes[3];
    vd.nodes[2].neighbors[1] = &vd.nodes[1];
    vd.nodes[2].neighbors[2] = &vd.nodes[0];

    vd.nodes[3].direction = HorizontalDirection::right;
    vd.nodes[3].vertex_it = vertices.begin() + 4;
    vd.nodes[3].lower_opp_edge = Edge::edge_from_index(vertices, 0);
    vd.nodes[3].upper_opp_edge = Edge::edge_from_index(vertices, 7);
    vd.nodes[3].neighbors[0] = &vd.nodes[2];
    vd.nodes[3].neighbors[1] = &vd.nodes[5];
    vd.nodes[3].neighbors[2] = &vd.nodes[4];

    vd.nodes[4].direction = HorizontalDirection::left;
    vd.nodes[4].vertex_it = vertices.begin() + 5;
    vd.nodes[4].lower_opp_edge = Edge::edge_from_index(vertices, 6);
    vd.nodes[4].upper_opp_edge = Edge::edge_from_index(vertices, 7);
    vd.nodes[4].neighbors[0] = nullptr;
    vd.nodes[4].neighbors[1] = nullptr;
    vd.nodes[4].neighbors[2] = &vd.nodes[3];

    vd.nodes[5].direction = HorizontalDirection::right;
    vd.nodes[5].vertex_it = vertices.begin() + 2;
    vd.nodes[5].lower_opp_edge = Edge::edge_from_index(vertices, 0);
    vd.nodes[5].upper_opp_edge = Edge::edge_from_index(vertices, 3);
    vd.nodes[5].neighbors[0] = &vd.nodes[3];
    vd.nodes[5].neighbors[1] = nullptr;
    vd.nodes[5].neighbors[2] = nullptr;

    SECTION("Start with rightward node, first region not a leaf")
    {
      RegionIterator iterator(&vd.nodes[0]);

      CHECK(iterator.region().left_node == &vd.nodes[0]);
      CHECK(iterator.region().right_node == &vd.nodes[2]);
      CHECK(iterator.region().left_node_branch_index == 2);
      CHECK(iterator.region().right_node_branch_index == 2);

      REQUIRE(iterator.move_next());
      CHECK(iterator.region().left_node == nullptr);
      CHECK(iterator.region().right_node == &vd.nodes[1]);
      CHECK(iterator.region().right_node_branch_index == 2);

      REQUIRE(iterator.move_next());
      CHECK(iterator.region().left_node == nullptr);
      CHECK(iterator.region().right_node == &vd.nodes[1]);
      CHECK(iterator.region().right_node_branch_index == 1);

      REQUIRE(iterator.move_next());
      CHECK(iterator.region().left_node == &vd.nodes[1]);
      CHECK(iterator.region().right_node == &vd.nodes[2]);
      CHECK(iterator.region().left_node_branch_index == 0);
      CHECK(iterator.region().right_node_branch_index == 1);

      REQUIRE(iterator.move_next());
      CHECK(iterator.region().left_node == &vd.nodes[2]);
      CHECK(iterator.region().right_node == &vd.nodes[3]);
      CHECK(iterator.region().left_node_branch_index == 0);
      CHECK(iterator.region().right_node_branch_index == 0);

      REQUIRE(iterator.move_next());
      CHECK(iterator.region().left_node == &vd.nodes[3]);
      CHECK(iterator.region().right_node == &vd.nodes[5]);
      CHECK(iterator.region().left_node_branch_index == 1);
      CHECK(iterator.region().right_node_branch_index == 0);

      REQUIRE(iterator.move_next());
      CHECK(iterator.region().left_node == &vd.nodes[5]);
      CHECK(iterator.region().right_node == nullptr);
      CHECK(iterator.region().left_node_branch_index == 1);

      REQUIRE(iterator.move_next());
      CHECK(iterator.region().left_node == &vd.nodes[5]);
      CHECK(iterator.region().right_node == nullptr);
      CHECK(iterator.region().left_node_branch_index == 2);

      REQUIRE(iterator.move_next());
      CHECK(iterator.region().left_node == &vd.nodes[3]);
      CHECK(iterator.region().right_node == &vd.nodes[4]);
      CHECK(iterator.region().left_node_branch_index == 2);
      CHECK(iterator.region().right_node_branch_index == 2);

      REQUIRE(iterator.move_next());
      CHECK(iterator.region().left_node == nullptr);
      CHECK(iterator.region().right_node == &vd.nodes[4]);
      CHECK(iterator.region().right_node_branch_index == 1);

      REQUIRE(iterator.move_next());
      CHECK(iterator.region().left_node == &vd.nodes[4]);
      CHECK(iterator.region().right_node == nullptr);
      CHECK(iterator.region().left_node_branch_index == 0);

      REQUIRE(iterator.move_next());
      CHECK(iterator.region().left_node == nullptr);
      CHECK(iterator.region().right_node == &vd.nodes[0]);
      CHECK(iterator.region().right_node_branch_index == 0);

      REQUIRE(iterator.move_next());
      CHECK(iterator.region().left_node == &vd.nodes[0]);
      CHECK(iterator.region().right_node == nullptr);
      CHECK(iterator.region().left_node_branch_index == 1);

      CHECK(!iterator.move_next());
    }

    SECTION("Starts with rightward node, first region a leaf")
    {
      RegionIterator iterator(&vd.nodes[5]);

      CHECK(iterator.region().left_node == &vd.nodes[5]);
      CHECK(iterator.region().right_node == nullptr);
      CHECK(iterator.region().left_node_branch_index == 2);

      REQUIRE(iterator.move_next());
      CHECK(iterator.region().left_node == &vd.nodes[3]);
      CHECK(iterator.region().right_node == &vd.nodes[4]);
      CHECK(iterator.region().left_node_branch_index == 2);
      CHECK(iterator.region().right_node_branch_index == 2);

      // The rest belongs to the general case, so no need to test it again.
    }

    SECTION("Starts with leftward node, first skipped region not a leaf")
    {
      RegionIterator iterator(&vd.nodes[2]);

      CHECK(iterator.region().left_node == nullptr);
      CHECK(iterator.region().right_node == &vd.nodes[1]);
      CHECK(iterator.region().right_node_branch_index == 2);

      REQUIRE(iterator.move_next());
      CHECK(iterator.region().left_node == nullptr);
      CHECK(iterator.region().right_node == &vd.nodes[1]);
      CHECK(iterator.region().right_node_branch_index == 1);

      // The rest belongs to the general case, so no need to test it again.
    }

    SECTION("Starts with lefward node, first skipped region a leaf")
    {
      RegionIterator iterator(&vd.nodes[1]);

      CHECK(iterator.region().left_node == nullptr);
      CHECK(iterator.region().right_node == &vd.nodes[1]);
      CHECK(iterator.region().right_node_branch_index == 1);

      REQUIRE(iterator.move_next());
      CHECK(iterator.region().left_node == &vd.nodes[1]);
      CHECK(iterator.region().right_node == &vd.nodes[2]);
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
    std::vector<Point2> vertices_storage = {
        {-2.72, 2.30}, {2.84, -2.38}, {2.04, 1.30}, {8.40, 1.34}, {7.36, -0.96},
        {12.62, 2.52}, {6.08, 6.02},  {8.28, 2.96}, {0.42, 3.28}, {1.88, 7.18},
    };

    VerticesView vertices(vertices_storage);

    VerticalDecomposition vd;
    vd.nodes.resize(6);

    vd.nodes[0].direction = HorizontalDirection::right;
    vd.nodes[0].vertex_it = vertices.begin() + 0;
    vd.nodes[0].lower_opp_edge = Edge::invalid();
    vd.nodes[0].upper_opp_edge = Edge::invalid();
    vd.nodes[0].neighbors[0] = nullptr;
    vd.nodes[0].neighbors[1] = &vd.nodes[2];
    vd.nodes[0].neighbors[2] = &vd.nodes[1];

    vd.nodes[1].direction = HorizontalDirection::left;
    vd.nodes[1].vertex_it = vertices.begin() + 9;
    vd.nodes[1].lower_opp_edge = Edge::edge_from_index(vertices, 7);
    vd.nodes[1].upper_opp_edge = Edge::invalid();
    vd.nodes[1].neighbors[0] = &vd.nodes[3];
    vd.nodes[1].neighbors[1] = nullptr;
    vd.nodes[1].neighbors[2] = &vd.nodes[0];

    vd.nodes[2].direction = HorizontalDirection::left;
    vd.nodes[2].vertex_it = vertices.begin() + 1;
    vd.nodes[2].lower_opp_edge = Edge::invalid();
    vd.nodes[2].upper_opp_edge = Edge::edge_from_index(vertices, 2);
    vd.nodes[2].neighbors[0] = &vd.nodes[4];
    vd.nodes[2].neighbors[1] = &vd.nodes[0];
    vd.nodes[2].neighbors[2] = nullptr;

    vd.nodes[3].direction = HorizontalDirection::right;
    vd.nodes[3].vertex_it = vertices.begin() + 6;
    vd.nodes[3].lower_opp_edge = Edge::edge_from_index(vertices, 7);
    vd.nodes[3].upper_opp_edge = Edge::invalid();
    vd.nodes[3].neighbors[0] = &vd.nodes[1];
    vd.nodes[3].neighbors[1] = nullptr;
    vd.nodes[3].neighbors[2] = &vd.nodes[5];

    vd.nodes[4].direction = HorizontalDirection::right;
    vd.nodes[4].vertex_it = vertices.begin() + 4;
    vd.nodes[4].lower_opp_edge = Edge::invalid();
    vd.nodes[4].upper_opp_edge = Edge::edge_from_index(vertices, 2);
    vd.nodes[4].neighbors[0] = &vd.nodes[2];
    vd.nodes[4].neighbors[1] = &vd.nodes[5];
    vd.nodes[4].neighbors[2] = nullptr;

    vd.nodes[5].direction = HorizontalDirection::left;
    vd.nodes[5].vertex_it = vertices.begin() + 5;
    vd.nodes[5].lower_opp_edge = Edge::invalid();
    vd.nodes[5].upper_opp_edge = Edge::invalid();
    vd.nodes[5].neighbors[0] = nullptr;
    vd.nodes[5].neighbors[1] = &vd.nodes[4];
    vd.nodes[5].neighbors[2] = &vd.nodes[3];

    SECTION("Start at node[0]")
    {
      RegionIterator iterator(&vd.nodes[0]);

      CHECK(iterator.region().left_node == &vd.nodes[0]);
      CHECK(iterator.region().right_node == &vd.nodes[1]);
      CHECK(iterator.region().left_node_branch_index == 2);
      CHECK(iterator.region().right_node_branch_index == 2);

      REQUIRE(iterator.move_next());
      CHECK(iterator.region().left_node == nullptr);
      CHECK(iterator.region().right_node == &vd.nodes[1]);
      CHECK(iterator.region().right_node_branch_index == 1);

      REQUIRE(iterator.move_next());
      CHECK(iterator.region().left_node == &vd.nodes[1]);
      CHECK(iterator.region().right_node == &vd.nodes[3]);
      CHECK(iterator.region().left_node_branch_index == 0);
      CHECK(iterator.region().right_node_branch_index == 0);

      REQUIRE(iterator.move_next());
      CHECK(iterator.region().left_node == &vd.nodes[3]);
      CHECK(iterator.region().right_node == nullptr);
      CHECK(iterator.region().left_node_branch_index == 1);

      REQUIRE(iterator.move_next());
      CHECK(iterator.region().left_node == &vd.nodes[3]);
      CHECK(iterator.region().right_node == &vd.nodes[5]);
      CHECK(iterator.region().left_node_branch_index == 2);
      CHECK(iterator.region().right_node_branch_index == 2);

      REQUIRE(iterator.move_next());
      CHECK(iterator.region().left_node == &vd.nodes[4]);
      CHECK(iterator.region().right_node == &vd.nodes[5]);
      CHECK(iterator.region().left_node_branch_index == 1);
      CHECK(iterator.region().right_node_branch_index == 1);

      REQUIRE(iterator.move_next());
      CHECK(iterator.region().left_node == &vd.nodes[4]);
      CHECK(iterator.region().right_node == nullptr);
      CHECK(iterator.region().left_node_branch_index == 2);

      REQUIRE(iterator.move_next());
      CHECK(iterator.region().left_node == &vd.nodes[2]);
      CHECK(iterator.region().right_node == &vd.nodes[4]);
      CHECK(iterator.region().left_node_branch_index == 0);
      CHECK(iterator.region().right_node_branch_index == 0);

      REQUIRE(iterator.move_next());
      CHECK(iterator.region().left_node == nullptr);
      CHECK(iterator.region().right_node == &vd.nodes[2]);
      CHECK(iterator.region().right_node_branch_index == 2);

      REQUIRE(iterator.move_next());
      CHECK(iterator.region().left_node == &vd.nodes[0]);
      CHECK(iterator.region().right_node == &vd.nodes[2]);
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