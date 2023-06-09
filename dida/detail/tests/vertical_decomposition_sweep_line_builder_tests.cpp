#include "dida/detail/vertical_decomposition_sweep_line_builder.hpp"

#include <catch2/catch.hpp>

namespace dida::detail::vertical_decomposition
{

TEST_CASE("vertical_decomposition_with_sweep_line_builder")
{
  SECTION("Interior, single left branch")
  {
    std::vector<Point2> vertices_storage{{-5.86, 2.62}, {-1.00, 4.42}, {-5.44, 6.28}, {-4.30, 4.66}};
    ArrayView<const Point2> vertices(vertices_storage);

    VerticalDecomposition vertical_decomposition =
        vertical_decomposition_with_sweep_line_builder(vertices, VerticalDecompositionType::interior_decomposition);

    const std::vector<Node>& nodes = vertical_decomposition.nodes;
    REQUIRE(nodes.size() == 4);

    CHECK(nodes[0].direction == HorizontalDirection::left);
    CHECK(nodes[0].is_leaf);
    CHECK(nodes[0].vertex_it == vertices.begin() + 0);
    CHECK(nodes[0].lower_opp_edge == Edge::edge_from_index(vertices, 0));
    CHECK(nodes[0].upper_opp_edge == Edge::edge_from_index(vertices, 3));
    CHECK(nodes[0].neighbors[0] == &nodes[2]);
    CHECK(nodes[0].neighbors[1] == nullptr);
    CHECK(nodes[0].neighbors[2] == nullptr);

    CHECK(nodes[1].direction == HorizontalDirection::left);
    CHECK(nodes[1].is_leaf);
    CHECK(nodes[1].vertex_it == vertices.begin() + 2);
    CHECK(nodes[1].lower_opp_edge == Edge::edge_from_index(vertices, 2));
    CHECK(nodes[1].upper_opp_edge == Edge::edge_from_index(vertices, 1));
    CHECK(nodes[1].neighbors[0] == &nodes[2]);
    CHECK(nodes[1].neighbors[1] == nullptr);
    CHECK(nodes[1].neighbors[2] == nullptr);

    CHECK(nodes[2].direction == HorizontalDirection::left);
    CHECK(!nodes[2].is_leaf);
    CHECK(nodes[2].vertex_it == vertices.begin() + 3);
    CHECK(nodes[2].lower_opp_edge == Edge::edge_from_index(vertices, 0));
    CHECK(nodes[2].upper_opp_edge == Edge::edge_from_index(vertices, 1));
    CHECK(nodes[2].neighbors[0] == &nodes[3]);
    CHECK(nodes[2].neighbors[1] == &nodes[0]);
    CHECK(nodes[2].neighbors[2] == &nodes[1]);

    CHECK(nodes[3].direction == HorizontalDirection::right);
    CHECK(nodes[3].is_leaf);
    CHECK(nodes[3].vertex_it == vertices.begin() + 1);
    CHECK(nodes[3].lower_opp_edge == Edge::edge_from_index(vertices, 0));
    CHECK(nodes[3].upper_opp_edge == Edge::edge_from_index(vertices, 1));
    CHECK(nodes[3].neighbors[0] == &nodes[2]);
    CHECK(nodes[3].neighbors[1] == nullptr);
    CHECK(nodes[3].neighbors[2] == nullptr);

    CHECK(vertical_decomposition.leftmost_node == &nodes.front());
    CHECK(vertical_decomposition.rightmost_node == &nodes.back());
  }

  SECTION("Interior, single right branch")
  {
    std::vector<Point2> vertices_storage{{3.88, 7.00}, {-2.34, 4.22}, {4.12, 2.14}, {1.52, 4.14}};
    ArrayView<const Point2> vertices(vertices_storage);

    VerticalDecomposition vertical_decomposition =
        vertical_decomposition_with_sweep_line_builder(vertices, VerticalDecompositionType::interior_decomposition);

    std::vector<Node>& nodes = vertical_decomposition.nodes;
    REQUIRE(nodes.size() == 4);

    CHECK(nodes[0].direction == HorizontalDirection::left);
    CHECK(nodes[0].is_leaf);
    CHECK(nodes[0].vertex_it == vertices.begin() + 1);
    CHECK(nodes[0].lower_opp_edge == Edge::edge_from_index(vertices, 1));
    CHECK(nodes[0].upper_opp_edge == Edge::edge_from_index(vertices, 0));
    CHECK(nodes[0].neighbors[0] == &nodes[1]);
    CHECK(nodes[0].neighbors[1] == nullptr);
    CHECK(nodes[0].neighbors[2] == nullptr);

    CHECK(nodes[1].direction == HorizontalDirection::right);
    CHECK(!nodes[1].is_leaf);
    CHECK(nodes[1].vertex_it == vertices.begin() + 3);
    CHECK(nodes[1].lower_opp_edge == Edge::edge_from_index(vertices, 1));
    CHECK(nodes[1].upper_opp_edge == Edge::edge_from_index(vertices, 0));
    CHECK(nodes[1].neighbors[0] == &nodes[0]);
    CHECK(nodes[1].neighbors[1] == &nodes[3]);
    CHECK(nodes[1].neighbors[2] == &nodes[2]);

    CHECK(nodes[2].direction == HorizontalDirection::right);
    CHECK(nodes[2].is_leaf);
    CHECK(nodes[2].vertex_it == vertices.begin());
    CHECK(nodes[2].lower_opp_edge == Edge::edge_from_index(vertices, 3));
    CHECK(nodes[2].upper_opp_edge == Edge::edge_from_index(vertices, 0));
    CHECK(nodes[2].neighbors[0] == &nodes[1]);
    CHECK(nodes[2].neighbors[1] == nullptr);
    CHECK(nodes[2].neighbors[2] == nullptr);

    CHECK(nodes[3].direction == HorizontalDirection::right);
    CHECK(nodes[3].is_leaf);
    CHECK(nodes[3].vertex_it == vertices.begin() + 2);
    CHECK(nodes[3].lower_opp_edge == Edge::edge_from_index(vertices, 1));
    CHECK(nodes[3].upper_opp_edge == Edge::edge_from_index(vertices, 2));
    CHECK(nodes[3].neighbors[0] == &nodes[1]);
    CHECK(nodes[3].neighbors[1] == nullptr);
    CHECK(nodes[3].neighbors[2] == nullptr);

    CHECK(vertical_decomposition.leftmost_node == &nodes.front());
    CHECK(vertical_decomposition.rightmost_node == &nodes.back());
  }

  SECTION("Internal, many nodes")
  {
    std::vector<Point2> vertices_storage{{2.42, 2.00},  {9.44, 0.98},  {7.74, 2.62}, {9.40, 3.60},  {5.42, 4.86},
                                         {9.32, 5.94},  {7.54, 6.94},  {9.36, 8.08}, {2.26, 7.16},  {-4.04, 8.18},
                                         {-2.48, 6.68}, {-4.16, 5.46}, {0.62, 4.50}, {-2.74, 3.40}, {-1.46, 2.82},
                                         {-3.30, 1.50}, {0.32, 2.24}};
    ArrayView<const Point2> vertices(vertices_storage);

    VerticalDecomposition vertical_decomposition =
        vertical_decomposition_with_sweep_line_builder(vertices, VerticalDecompositionType::interior_decomposition);

    const std::vector<Node>& nodes = vertical_decomposition.nodes;
    REQUIRE(nodes.size() == 14);

    CHECK(nodes[0].direction == HorizontalDirection::left);
    CHECK(nodes[0].is_leaf);
    CHECK(nodes[0].vertex_it == vertices.begin() + 11);
    CHECK(nodes[0].lower_opp_edge == Edge::edge_from_index(vertices, 11));
    CHECK(nodes[0].upper_opp_edge == Edge::edge_from_index(vertices, 10));
    CHECK(nodes[0].neighbors[0] == &nodes[4]);
    CHECK(nodes[0].neighbors[1] == nullptr);
    CHECK(nodes[0].neighbors[2] == nullptr);

    CHECK(nodes[1].direction == HorizontalDirection::left);
    CHECK(nodes[1].is_leaf);
    CHECK(nodes[1].vertex_it == vertices.begin() + 9);
    CHECK(nodes[1].lower_opp_edge == Edge::edge_from_index(vertices, 9));
    CHECK(nodes[1].upper_opp_edge == Edge::edge_from_index(vertices, 8));
    CHECK(nodes[1].neighbors[0] == &nodes[4]);
    CHECK(nodes[1].neighbors[1] == nullptr);
    CHECK(nodes[1].neighbors[2] == nullptr);

    CHECK(nodes[2].direction == HorizontalDirection::left);
    CHECK(nodes[2].is_leaf);
    CHECK(nodes[2].vertex_it == vertices.begin() + 15);
    CHECK(nodes[2].lower_opp_edge == Edge::edge_from_index(vertices, 15));
    CHECK(nodes[2].upper_opp_edge == Edge::edge_from_index(vertices, 14));
    CHECK(nodes[2].neighbors[0] == &nodes[5]);
    CHECK(nodes[2].neighbors[1] == nullptr);
    CHECK(nodes[2].neighbors[2] == nullptr);

    CHECK(nodes[3].direction == HorizontalDirection::left);
    CHECK(nodes[3].is_leaf);
    CHECK(nodes[3].vertex_it == vertices.begin() + 13);
    CHECK(nodes[3].lower_opp_edge == Edge::edge_from_index(vertices, 13));
    CHECK(nodes[3].upper_opp_edge == Edge::edge_from_index(vertices, 12));
    CHECK(nodes[3].neighbors[0] == &nodes[5]);
    CHECK(nodes[3].neighbors[1] == nullptr);
    CHECK(nodes[3].neighbors[2] == nullptr);

    CHECK(nodes[4].direction == HorizontalDirection::left);
    CHECK(!nodes[4].is_leaf);
    CHECK(nodes[4].vertex_it == vertices.begin() + 10);
    CHECK(nodes[4].lower_opp_edge == Edge::edge_from_index(vertices, 11));
    CHECK(nodes[4].upper_opp_edge == Edge::edge_from_index(vertices, 8));
    CHECK(nodes[4].neighbors[0] == &nodes[6]);
    CHECK(nodes[4].neighbors[1] == &nodes[0]);
    CHECK(nodes[4].neighbors[2] == &nodes[1]);

    CHECK(nodes[5].direction == HorizontalDirection::left);
    CHECK(!nodes[5].is_leaf);
    CHECK(nodes[5].vertex_it == vertices.begin() + 14);
    CHECK(nodes[5].lower_opp_edge == Edge::edge_from_index(vertices, 15));
    CHECK(nodes[5].upper_opp_edge == Edge::edge_from_index(vertices, 12));
    CHECK(nodes[5].neighbors[0] == &nodes[6]);
    CHECK(nodes[5].neighbors[1] == &nodes[2]);
    CHECK(nodes[5].neighbors[2] == &nodes[3]);

    CHECK(nodes[6].direction == HorizontalDirection::left);
    CHECK(!nodes[6].is_leaf);
    CHECK(nodes[6].vertex_it == vertices.begin() + 12);
    CHECK(nodes[6].lower_opp_edge == Edge::edge_from_index(vertices, 16));
    CHECK(nodes[6].upper_opp_edge == Edge::edge_from_index(vertices, 8));
    CHECK(nodes[6].neighbors[0] == &nodes[7]);
    CHECK(nodes[6].neighbors[1] == &nodes[5]);
    CHECK(nodes[6].neighbors[2] == &nodes[4]);

    CHECK(nodes[7].direction == HorizontalDirection::right);
    CHECK(!nodes[7].is_leaf);
    CHECK(nodes[7].vertex_it == vertices.begin() + 4);
    CHECK(nodes[7].lower_opp_edge == Edge::edge_from_index(vertices, 0));
    CHECK(nodes[7].upper_opp_edge == Edge::edge_from_index(vertices, 7));
    CHECK(nodes[7].neighbors[0] == &nodes[6]);
    CHECK(nodes[7].neighbors[1] == &nodes[9]);
    CHECK(nodes[7].neighbors[2] == &nodes[8]);

    CHECK(nodes[8].direction == HorizontalDirection::right);
    CHECK(!nodes[8].is_leaf);
    CHECK(nodes[8].vertex_it == vertices.begin() + 6);
    CHECK(nodes[8].lower_opp_edge == Edge::edge_from_index(vertices, 4));
    CHECK(nodes[8].upper_opp_edge == Edge::edge_from_index(vertices, 7));
    CHECK(nodes[8].neighbors[0] == &nodes[7]);
    CHECK(nodes[8].neighbors[1] == &nodes[10]);
    CHECK(nodes[8].neighbors[2] == &nodes[11]);

    CHECK(nodes[9].direction == HorizontalDirection::right);
    CHECK(!nodes[9].is_leaf);
    CHECK(nodes[9].vertex_it == vertices.begin() + 2);
    CHECK(nodes[9].lower_opp_edge == Edge::edge_from_index(vertices, 0));
    CHECK(nodes[9].upper_opp_edge == Edge::edge_from_index(vertices, 3));
    CHECK(nodes[9].neighbors[0] == &nodes[7]);
    CHECK(nodes[9].neighbors[1] == &nodes[13]);
    CHECK(nodes[9].neighbors[2] == &nodes[12]);

    CHECK(nodes[10].direction == HorizontalDirection::right);
    CHECK(nodes[10].is_leaf);
    CHECK(nodes[10].vertex_it == vertices.begin() + 5);
    CHECK(nodes[10].lower_opp_edge == Edge::edge_from_index(vertices, 4));
    CHECK(nodes[10].upper_opp_edge == Edge::edge_from_index(vertices, 5));
    CHECK(nodes[10].neighbors[0] == &nodes[8]);
    CHECK(nodes[10].neighbors[1] == nullptr);
    CHECK(nodes[10].neighbors[2] == nullptr);

    CHECK(nodes[11].direction == HorizontalDirection::right);
    CHECK(nodes[11].is_leaf);
    CHECK(nodes[11].vertex_it == vertices.begin() + 7);
    CHECK(nodes[11].lower_opp_edge == Edge::edge_from_index(vertices, 6));
    CHECK(nodes[11].upper_opp_edge == Edge::edge_from_index(vertices, 7));
    CHECK(nodes[11].neighbors[0] == &nodes[8]);
    CHECK(nodes[11].neighbors[1] == nullptr);
    CHECK(nodes[11].neighbors[2] == nullptr);

    CHECK(nodes[12].direction == HorizontalDirection::right);
    CHECK(nodes[12].is_leaf);
    CHECK(nodes[12].vertex_it == vertices.begin() + 3);
    CHECK(nodes[12].lower_opp_edge == Edge::edge_from_index(vertices, 2));
    CHECK(nodes[12].upper_opp_edge == Edge::edge_from_index(vertices, 3));
    CHECK(nodes[12].neighbors[0] == &nodes[9]);
    CHECK(nodes[12].neighbors[1] == nullptr);
    CHECK(nodes[12].neighbors[2] == nullptr);

    CHECK(nodes[13].direction == HorizontalDirection::right);
    CHECK(nodes[13].is_leaf);
    CHECK(nodes[13].vertex_it == vertices.begin() + 1);
    CHECK(nodes[13].lower_opp_edge == Edge::edge_from_index(vertices, 0));
    CHECK(nodes[13].upper_opp_edge == Edge::edge_from_index(vertices, 1));
    CHECK(nodes[13].neighbors[0] == &nodes[9]);
    CHECK(nodes[13].neighbors[1] == nullptr);
    CHECK(nodes[13].neighbors[2] == nullptr);

    CHECK(vertical_decomposition.leftmost_node == &nodes.front());
    CHECK(vertical_decomposition.rightmost_node == &nodes.back());
  }

  SECTION("Exterior, few nodes")
  {
    std::vector<Point2> vertices_storage{{-3.32, 1.46}, {-1.84, 1.48}, {-2.54, -0.26}, {0.64, 0.74}, {2.48, 3.02},
                                         {-1.58, 3.68}, {-0.60, 5.02}, {-2.72, 4.94},  {-4.34, 3.08}};
    ArrayView<const Point2> vertices(vertices_storage);

    VerticalDecomposition vertical_decomposition =
        vertical_decomposition_with_sweep_line_builder(vertices, VerticalDecompositionType::exterior_decomposition);

    const std::vector<Node>& nodes = vertical_decomposition.nodes;
    REQUIRE(nodes.size() == 6);

    CHECK(nodes[0].direction == HorizontalDirection::right);
    CHECK(!nodes[0].is_leaf);
    CHECK(nodes[0].vertex_it == vertices.begin() + 8);
    CHECK(nodes[0].lower_opp_edge == Edge::invalid());
    CHECK(nodes[0].upper_opp_edge == Edge::invalid());
    CHECK(nodes[0].neighbors[0] == nullptr);
    CHECK(nodes[0].neighbors[1] == &nodes[1]);
    CHECK(nodes[0].neighbors[2] == &nodes[4]);

    CHECK(nodes[1].direction == HorizontalDirection::right);
    CHECK(!nodes[1].is_leaf);
    CHECK(nodes[1].vertex_it == vertices.begin() + 2);
    CHECK(nodes[1].lower_opp_edge == Edge::invalid());
    CHECK(nodes[1].upper_opp_edge == Edge::edge_from_index(vertices, 0));
    CHECK(nodes[1].neighbors[0] == &nodes[0]);
    CHECK(nodes[1].neighbors[1] == &nodes[5]);
    CHECK(nodes[1].neighbors[2] == &nodes[2]);

    CHECK(nodes[2].direction == HorizontalDirection::right);
    CHECK(nodes[2].is_leaf);
    CHECK(nodes[2].vertex_it == vertices.begin() + 1);
    CHECK(nodes[2].lower_opp_edge == Edge::edge_from_index(vertices, 1));
    CHECK(nodes[2].upper_opp_edge == Edge::edge_from_index(vertices, 0));
    CHECK(nodes[2].neighbors[0] == &nodes[1]);
    CHECK(nodes[2].neighbors[1] == nullptr);
    CHECK(nodes[2].neighbors[2] == nullptr);

    CHECK(nodes[3].direction == HorizontalDirection::left);
    CHECK(nodes[3].is_leaf);
    CHECK(nodes[3].vertex_it == vertices.begin() + 5);
    CHECK(nodes[3].lower_opp_edge == Edge::edge_from_index(vertices, 4));
    CHECK(nodes[3].upper_opp_edge == Edge::edge_from_index(vertices, 5));
    CHECK(nodes[3].neighbors[0] == &nodes[4]);
    CHECK(nodes[3].neighbors[1] == nullptr);
    CHECK(nodes[3].neighbors[2] == nullptr);

    CHECK(nodes[4].direction == HorizontalDirection::left);
    CHECK(!nodes[4].is_leaf);
    CHECK(nodes[4].vertex_it == vertices.begin() + 6);
    CHECK(nodes[4].lower_opp_edge == Edge::edge_from_index(vertices, 4));
    CHECK(nodes[4].upper_opp_edge == Edge::invalid());
    CHECK(nodes[4].neighbors[0] == &nodes[5]);
    CHECK(nodes[4].neighbors[1] == &nodes[3]);
    CHECK(nodes[4].neighbors[2] == &nodes[0]);

    CHECK(nodes[5].direction == HorizontalDirection::left);
    CHECK(!nodes[5].is_leaf);
    CHECK(nodes[5].vertex_it == vertices.begin() + 4);
    CHECK(nodes[5].lower_opp_edge == Edge::invalid());
    CHECK(nodes[5].upper_opp_edge == Edge::invalid());
    CHECK(nodes[5].neighbors[0] == nullptr);
    CHECK(nodes[5].neighbors[1] == &nodes[1]);
    CHECK(nodes[5].neighbors[2] == &nodes[4]);

    CHECK(vertical_decomposition.leftmost_node == &nodes.front());
    CHECK(vertical_decomposition.rightmost_node == &nodes.back());
  }

  SECTION("Vertices on same vertical line")
  {
    std::vector<Point2> vertices_storage = {{5, 1}, {10, 2}, {7, 3}, {10, 5}, {7, 6},
                                            {9, 7}, {2, 7},  {7, 5}, {3, 3},  {7, 2}};
    ArrayView<const Point2> vertices(vertices_storage);

    VerticalDecomposition vertical_decomposition =
        vertical_decomposition_with_sweep_line_builder(vertices, VerticalDecompositionType::interior_decomposition);

    const std::vector<Node>& nodes = vertical_decomposition.nodes;
    REQUIRE(nodes.size() == 10);

    CHECK(nodes[0].direction == HorizontalDirection::left);
    CHECK(nodes[0].is_leaf);
    CHECK(nodes[0].vertex_it == vertices.begin() + 6);
    CHECK(nodes[0].lower_opp_edge == Edge::edge_from_index(vertices, 6));
    CHECK(nodes[0].upper_opp_edge == Edge::edge_from_index(vertices, 5));
    CHECK(nodes[0].neighbors[0] == &nodes[5]);
    CHECK(nodes[0].neighbors[1] == nullptr);
    CHECK(nodes[0].neighbors[2] == nullptr);

    CHECK(nodes[1].direction == HorizontalDirection::left);
    CHECK(nodes[1].is_leaf);
    CHECK(nodes[1].vertex_it == vertices.begin() + 8);
    CHECK(nodes[1].lower_opp_edge == Edge::edge_from_index(vertices, 8));
    CHECK(nodes[1].upper_opp_edge == Edge::edge_from_index(vertices, 7));
    CHECK(nodes[1].neighbors[0] == &nodes[3]);
    CHECK(nodes[1].neighbors[1] == nullptr);
    CHECK(nodes[1].neighbors[2] == nullptr);

    CHECK(nodes[2].direction == HorizontalDirection::left);
    CHECK(nodes[2].is_leaf);
    CHECK(nodes[2].vertex_it == vertices.begin() + 0);
    CHECK(nodes[2].lower_opp_edge == Edge::edge_from_index(vertices, 0));
    CHECK(nodes[2].upper_opp_edge == Edge::edge_from_index(vertices, 9));
    CHECK(nodes[2].neighbors[0] == &nodes[3]);
    CHECK(nodes[2].neighbors[1] == nullptr);
    CHECK(nodes[2].neighbors[2] == nullptr);

    CHECK(nodes[3].direction == HorizontalDirection::left);
    CHECK(!nodes[3].is_leaf);
    CHECK(nodes[3].vertex_it == vertices.begin() + 9);
    CHECK(nodes[3].lower_opp_edge == Edge::edge_from_index(vertices, 0));
    CHECK(nodes[3].upper_opp_edge == Edge::edge_from_index(vertices, 7));
    CHECK(nodes[3].neighbors[0] == &nodes[4]);
    CHECK(nodes[3].neighbors[1] == &nodes[2]);
    CHECK(nodes[3].neighbors[2] == &nodes[1]);

    CHECK(nodes[4].direction == HorizontalDirection::right);
    CHECK(!nodes[4].is_leaf);
    CHECK(nodes[4].vertex_it == vertices.begin() + 2);
    CHECK(nodes[4].lower_opp_edge == Edge::edge_from_index(vertices, 0));
    CHECK(nodes[4].upper_opp_edge == Edge::edge_from_index(vertices, 7));
    CHECK(nodes[4].neighbors[0] == &nodes[3]);
    CHECK(nodes[4].neighbors[1] == &nodes[8]);
    CHECK(nodes[4].neighbors[2] == &nodes[5]);

    CHECK(nodes[5].direction == HorizontalDirection::left);
    CHECK(!nodes[5].is_leaf);
    CHECK(nodes[5].vertex_it == vertices.begin() + 7);
    CHECK(nodes[5].lower_opp_edge == Edge::edge_from_index(vertices, 2));
    CHECK(nodes[5].upper_opp_edge == Edge::edge_from_index(vertices, 5));
    CHECK(nodes[5].neighbors[0] == &nodes[6]);
    CHECK(nodes[5].neighbors[1] == &nodes[4]);
    CHECK(nodes[5].neighbors[2] == &nodes[0]);

    CHECK(nodes[6].direction == HorizontalDirection::right);
    CHECK(!nodes[6].is_leaf);
    CHECK(nodes[6].vertex_it == vertices.begin() + 4);
    CHECK(nodes[6].lower_opp_edge == Edge::edge_from_index(vertices, 2));
    CHECK(nodes[6].upper_opp_edge == Edge::edge_from_index(vertices, 5));
    CHECK(nodes[6].neighbors[0] == &nodes[5]);
    CHECK(nodes[6].neighbors[1] == &nodes[9]);
    CHECK(nodes[6].neighbors[2] == &nodes[7]);

    CHECK(nodes[7].direction == HorizontalDirection::right);
    CHECK(nodes[7].is_leaf);
    CHECK(nodes[7].vertex_it == vertices.begin() + 5);
    CHECK(nodes[7].lower_opp_edge == Edge::edge_from_index(vertices, 4));
    CHECK(nodes[7].upper_opp_edge == Edge::edge_from_index(vertices, 5));
    CHECK(nodes[7].neighbors[0] == &nodes[6]);
    CHECK(nodes[7].neighbors[1] == nullptr);
    CHECK(nodes[7].neighbors[2] == nullptr);

    CHECK(nodes[8].direction == HorizontalDirection::right);
    CHECK(nodes[8].is_leaf);
    CHECK(nodes[8].vertex_it == vertices.begin() + 1);
    CHECK(nodes[8].lower_opp_edge == Edge::edge_from_index(vertices, 0));
    CHECK(nodes[8].upper_opp_edge == Edge::edge_from_index(vertices, 1));
    CHECK(nodes[8].neighbors[0] == &nodes[4]);
    CHECK(nodes[8].neighbors[1] == nullptr);
    CHECK(nodes[8].neighbors[2] == nullptr);

    CHECK(nodes[9].direction == HorizontalDirection::right);
    CHECK(nodes[9].is_leaf);
    CHECK(nodes[9].vertex_it == vertices.begin() + 3);
    CHECK(nodes[9].lower_opp_edge == Edge::edge_from_index(vertices, 2));
    CHECK(nodes[9].upper_opp_edge == Edge::edge_from_index(vertices, 3));
    CHECK(nodes[9].neighbors[0] == &nodes[6]);
    CHECK(nodes[9].neighbors[1] == nullptr);
    CHECK(nodes[9].neighbors[2] == nullptr);

    CHECK(vertical_decomposition.leftmost_node == &nodes.front());
    CHECK(vertical_decomposition.rightmost_node == &nodes.back());
  }
}

} // namespace dida::detail::vertical_decomposition