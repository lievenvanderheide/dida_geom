#include "dida/detail/vertical_decomposition.hpp"

#include <catch2/catch.hpp>

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

TEST_CASE("Edge::edge_from_index")
{
  std::vector<Point2> vertices_storage{{1.64, 2.04}, {4.52, 1.74}, {5.92, 4.52}, {0.50, 6.34}};
  ArrayView<const Point2> vertices(vertices_storage);

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
    ArrayView<const Point2> vertices(vertices_storage);

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
  ArrayView<const Point2> vertices(vertices_storage);
  Edge edge = Edge::edge_from_index(vertices, 0);
  CHECK(edge.segment() == Segment2(vertices[0], vertices[1]));
}

TEST_CASE("Edge::operator==")
{
  std::vector<Point2> vertices_storage{{1.64, 2.04}, {4.52, 1.74}, {5.92, 4.52}, {0.50, 6.34}};
  ArrayView<const Point2> vertices(vertices_storage);

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

namespace
{

/// Validates that the set of regions produced with a @c VerticalDecompositionRegionsIterator is the same, up to a
/// rotation of the region list, regardless of the node passed to the @c VerticalDecompositionRegionsIterator
/// constructor.
///
/// @param vd The vertical decomposition to validate.
void validate_region_iterator_with_each_start_node(const VerticalDecomposition& vd)
{
  std::vector<VerticalDecompositionRegionsIterator::Region> expected_regions;

  VerticalDecompositionRegionsIterator ref_iterator(&vd.nodes[0]);
  do
  {
    expected_regions.push_back(ref_iterator.region());
  } while (ref_iterator.move_next());

  for (size_t i = 1; i < vd.nodes.size(); i++)
  {
    VerticalDecompositionRegionsIterator iterator(&vd.nodes[i]);

    std::vector<VerticalDecompositionRegionsIterator::Region>::iterator expected_regions_it =
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

TEST_CASE("VerticalDecompositionRegionsIterator")
{
  SECTION("Interior")
  {
    std::vector<Point2> vertices_storage{{2.16, 1.54},  {15.28, 1.80}, {13.70, 3.04}, {15.14, 3.96}, {10.14, 5.00},
                                         {13.02, 7.12}, {12.84, 5.96}, {15.02, 8.46}, {-0.62, 8.72}, {2.02, 6.58},
                                         {1.26, 7.88},  {5.86, 5.18},  {2.02, 3.86},  {4.90, 3.02}};

    ArrayView<const Point2> vertices(vertices_storage);

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
      VerticalDecompositionRegionsIterator iterator(&vd.nodes[0]);

      CHECK(iterator.region().left_node == &vd.nodes[0]);
      CHECK(iterator.region().right_node == &vd.nodes[2]);

      REQUIRE(iterator.move_next());
      CHECK(iterator.region().left_node == nullptr);
      CHECK(iterator.region().right_node == &vd.nodes[1]);
      CHECK(iterator.region().leaf_region_branch_index == 2);

      REQUIRE(iterator.move_next());
      CHECK(iterator.region().left_node == nullptr);
      CHECK(iterator.region().right_node == &vd.nodes[1]);
      CHECK(iterator.region().leaf_region_branch_index == 1);

      REQUIRE(iterator.move_next());
      CHECK(iterator.region().left_node == &vd.nodes[1]);
      CHECK(iterator.region().right_node == &vd.nodes[2]);

      REQUIRE(iterator.move_next());
      CHECK(iterator.region().left_node == &vd.nodes[2]);
      CHECK(iterator.region().right_node == &vd.nodes[3]);

      REQUIRE(iterator.move_next());
      CHECK(iterator.region().left_node == &vd.nodes[3]);
      CHECK(iterator.region().right_node == &vd.nodes[5]);

      REQUIRE(iterator.move_next());
      CHECK(iterator.region().left_node == &vd.nodes[5]);
      CHECK(iterator.region().right_node == nullptr);
      CHECK(iterator.region().leaf_region_branch_index == 1);

      REQUIRE(iterator.move_next());
      CHECK(iterator.region().left_node == &vd.nodes[5]);
      CHECK(iterator.region().right_node == nullptr);
      CHECK(iterator.region().leaf_region_branch_index == 2);

      REQUIRE(iterator.move_next());
      CHECK(iterator.region().left_node == &vd.nodes[3]);
      CHECK(iterator.region().right_node == &vd.nodes[4]);

      REQUIRE(iterator.move_next());
      CHECK(iterator.region().left_node == nullptr);
      CHECK(iterator.region().right_node == &vd.nodes[4]);
      CHECK(iterator.region().leaf_region_branch_index == 1);

      REQUIRE(iterator.move_next());
      CHECK(iterator.region().left_node == &vd.nodes[4]);
      CHECK(iterator.region().right_node == nullptr);
      CHECK(iterator.region().leaf_region_branch_index == 0);

      REQUIRE(iterator.move_next());
      CHECK(iterator.region().left_node == nullptr);
      CHECK(iterator.region().right_node == &vd.nodes[0]);
      CHECK(iterator.region().leaf_region_branch_index == 0);

      REQUIRE(iterator.move_next());
      CHECK(iterator.region().left_node == &vd.nodes[0]);
      CHECK(iterator.region().right_node == nullptr);
      CHECK(iterator.region().leaf_region_branch_index == 1);

      CHECK(!iterator.move_next());
    }

    SECTION("Starts with rightward node, first region a leaf")
    {
      VerticalDecompositionRegionsIterator iterator(&vd.nodes[5]);

      CHECK(iterator.region().left_node == &vd.nodes[5]);
      CHECK(iterator.region().right_node == nullptr);
      CHECK(iterator.region().leaf_region_branch_index == 2);

      REQUIRE(iterator.move_next());
      CHECK(iterator.region().left_node == &vd.nodes[3]);
      CHECK(iterator.region().right_node == &vd.nodes[4]);

      // The rest belongs to the general case, so no need to test it again.
    }

    SECTION("Starts with leftward node, first skipped region not a leaf")
    {
      VerticalDecompositionRegionsIterator iterator(&vd.nodes[2]);

      CHECK(iterator.region().left_node == nullptr);
      CHECK(iterator.region().right_node == &vd.nodes[1]);
      CHECK(iterator.region().leaf_region_branch_index == 2);

      REQUIRE(iterator.move_next());
      CHECK(iterator.region().left_node == nullptr);
      CHECK(iterator.region().right_node == &vd.nodes[1]);
      CHECK(iterator.region().leaf_region_branch_index == 1);

      // The rest belongs to the general case, so no need to test it again.
    }

    SECTION("Starts with lefward node, first skipped region a leaf")
    {
      VerticalDecompositionRegionsIterator iterator(&vd.nodes[1]);

      CHECK(iterator.region().left_node == nullptr);
      CHECK(iterator.region().right_node == &vd.nodes[1]);
      CHECK(iterator.region().leaf_region_branch_index == 1);

      REQUIRE(iterator.move_next());
      CHECK(iterator.region().left_node == &vd.nodes[1]);
      CHECK(iterator.region().right_node == &vd.nodes[2]);

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

    ArrayView<const Point2> vertices(vertices_storage);

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
      VerticalDecompositionRegionsIterator iterator(&vd.nodes[0]);

      CHECK(iterator.region().left_node == &vd.nodes[0]);
      CHECK(iterator.region().right_node == &vd.nodes[1]);

      REQUIRE(iterator.move_next());
      CHECK(iterator.region().left_node == nullptr);
      CHECK(iterator.region().right_node == &vd.nodes[1]);
      CHECK(iterator.region().leaf_region_branch_index == 1);

      REQUIRE(iterator.move_next());
      CHECK(iterator.region().left_node == &vd.nodes[1]);
      CHECK(iterator.region().right_node == &vd.nodes[3]);

      REQUIRE(iterator.move_next());
      CHECK(iterator.region().left_node == &vd.nodes[3]);
      CHECK(iterator.region().right_node == nullptr);
      CHECK(iterator.region().leaf_region_branch_index == 1);

      REQUIRE(iterator.move_next());
      CHECK(iterator.region().left_node == &vd.nodes[3]);
      CHECK(iterator.region().right_node == &vd.nodes[5]);

      REQUIRE(iterator.move_next());
      CHECK(iterator.region().left_node == &vd.nodes[4]);
      CHECK(iterator.region().right_node == &vd.nodes[5]);

      REQUIRE(iterator.move_next());
      CHECK(iterator.region().left_node == &vd.nodes[4]);
      CHECK(iterator.region().right_node == nullptr);
      CHECK(iterator.region().leaf_region_branch_index == 2);

      REQUIRE(iterator.move_next());
      CHECK(iterator.region().left_node == &vd.nodes[2]);
      CHECK(iterator.region().right_node == &vd.nodes[4]);

      REQUIRE(iterator.move_next());
      CHECK(iterator.region().left_node == nullptr);
      CHECK(iterator.region().right_node == &vd.nodes[2]);
      CHECK(iterator.region().leaf_region_branch_index == 2);

      REQUIRE(iterator.move_next());
      CHECK(iterator.region().left_node == &vd.nodes[0]);
      CHECK(iterator.region().right_node == &vd.nodes[2]);

      CHECK(!iterator.move_next());
    }

    SECTION("Each start node produces the same regions")
    {
      validate_region_iterator_with_each_start_node(vd);
    }
  }
}

TEST_CASE("VerticalDecompositionRegionsIterator::Region::operator==")
{
  std::vector<Point2> vertices_storage{
      {-5.42, 2.82}, {3.92, 3.62}, {2.26, 5.74}, {3.74, 7.82}, {-4.52, 6.74}, {-1.10, 4.48},
  };

  ArrayView<const Point2> vertices(vertices_storage);

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

  VerticalDecompositionRegionsIterator::Region lower_left{nullptr, &vd.nodes[0], 1};
  VerticalDecompositionRegionsIterator::Region upper_left{nullptr, &vd.nodes[0], 2};

  VerticalDecompositionRegionsIterator::Region mid_0{&vd.nodes[0], &vd.nodes[1], 1};
  VerticalDecompositionRegionsIterator::Region mid_1{&vd.nodes[0], &vd.nodes[1], 2};

  VerticalDecompositionRegionsIterator::Region lower_right{&vd.nodes[1], nullptr, 1};
  VerticalDecompositionRegionsIterator::Region upper_right{&vd.nodes[1], nullptr, 2};

  CHECK(lower_left == lower_left);
  CHECK_FALSE(lower_left == upper_left);
  CHECK_FALSE(lower_left == mid_0);
  CHECK(mid_0 == mid_0);
  CHECK(mid_0 == mid_1);
  CHECK_FALSE(mid_0 == lower_right);
  CHECK(lower_right == lower_right);
  CHECK_FALSE(lower_right == upper_right);
}

} // namespace dida::detail::vertical_decomposition