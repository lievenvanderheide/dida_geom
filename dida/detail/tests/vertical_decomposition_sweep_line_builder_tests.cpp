#include "dida/detail/vertical_decomposition_sweep_line_builder.hpp"

#include <catch2/catch.hpp>

namespace dida::detail::vertical_decomposition
{

TEST_CASE("vertical_decomposition_with_sweep_line_builder")
{
  SECTION("Interior, single left_branches")
  {
    std::vector<Point2> vertices_storage{{-5.86, 2.62}, {-1.00, 4.42}, {-5.44, 6.28}, {-4.30, 4.66}};
    ArrayView<const Point2> vertices(vertices_storage);

    VerticalDecomposition vertical_decomposition =
        vertical_decomposition_with_sweep_line_builder(vertices, VerticalDecompositionType::interior_decomposition);

    REQUIRE(vertical_decomposition.nodes.size() == 1);

    const Node& node = vertical_decomposition.nodes[0];
    CHECK(node.direction == HorizontalDirection::left);
    CHECK(node.vertex_it == vertices.begin() + 3);
    CHECK(node.lower_opp_edge == Edge::edge_from_index(vertices, 0));
    CHECK(node.upper_opp_edge == Edge::edge_from_index(vertices, 1));
    CHECK(node.neighbors[0] == nullptr);
    CHECK(node.neighbors[1] == nullptr);
    CHECK(node.neighbors[2] == nullptr);
  }

  SECTION("Interior, single right_branches")
  {
    std::vector<Point2> vertices_storage{{3.88, 7.00}, {-2.34, 4.22}, {4.12, 2.14}, {1.52, 4.14}};
    ArrayView<const Point2> vertices(vertices_storage);

    VerticalDecomposition vertical_decomposition =
        vertical_decomposition_with_sweep_line_builder(vertices, VerticalDecompositionType::interior_decomposition);

    REQUIRE(vertical_decomposition.nodes.size() == 1);

    const Node& node = vertical_decomposition.nodes[0];
    CHECK(node.direction == HorizontalDirection::right);
    CHECK(node.vertex_it == vertices.begin() + 3);
    CHECK(node.lower_opp_edge == Edge::edge_from_index(vertices, 1));
    CHECK(node.upper_opp_edge == Edge::edge_from_index(vertices, 0));
    CHECK(node.neighbors[0] == nullptr);
    CHECK(node.neighbors[1] == nullptr);
    CHECK(node.neighbors[2] == nullptr);
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

    REQUIRE(vertical_decomposition.nodes.size() == 6);

    const std::vector<Node>& nodes = vertical_decomposition.nodes;

    CHECK(nodes[0].direction == HorizontalDirection::left);
    CHECK(nodes[0].vertex_it == vertices.begin() + 10);
    CHECK(nodes[0].lower_opp_edge == Edge::edge_from_index(vertices, 11));
    CHECK(nodes[0].upper_opp_edge == Edge::edge_from_index(vertices, 8));
    CHECK(nodes[0].neighbors[0] == &nodes[2]);
    CHECK(nodes[0].neighbors[1] == nullptr);
    CHECK(nodes[0].neighbors[2] == nullptr);

    CHECK(nodes[1].direction == HorizontalDirection::left);
    CHECK(nodes[1].vertex_it == vertices.begin() + 14);
    CHECK(nodes[1].lower_opp_edge == Edge::edge_from_index(vertices, 15));
    CHECK(nodes[1].upper_opp_edge == Edge::edge_from_index(vertices, 12));
    CHECK(nodes[1].neighbors[0] == &nodes[2]);
    CHECK(nodes[1].neighbors[1] == nullptr);
    CHECK(nodes[1].neighbors[2] == nullptr);

    CHECK(nodes[2].direction == HorizontalDirection::left);
    CHECK(nodes[2].vertex_it == vertices.begin() + 12);
    CHECK(nodes[2].lower_opp_edge == Edge::edge_from_index(vertices, 16));
    CHECK(nodes[2].upper_opp_edge == Edge::edge_from_index(vertices, 8));
    CHECK(nodes[2].neighbors[0] == &nodes[3]);
    CHECK(nodes[2].neighbors[1] == &nodes[1]);
    CHECK(nodes[2].neighbors[2] == &nodes[0]);

    CHECK(nodes[3].direction == HorizontalDirection::right);
    CHECK(nodes[3].vertex_it == vertices.begin() + 4);
    CHECK(nodes[3].lower_opp_edge == Edge::edge_from_index(vertices, 0));
    CHECK(nodes[3].upper_opp_edge == Edge::edge_from_index(vertices, 7));
    CHECK(nodes[3].neighbors[0] == &nodes[2]);
    CHECK(nodes[3].neighbors[1] == &nodes[5]);
    CHECK(nodes[3].neighbors[2] == &nodes[4]);

    CHECK(nodes[4].direction == HorizontalDirection::right);
    CHECK(nodes[4].vertex_it == vertices.begin() + 6);
    CHECK(nodes[4].lower_opp_edge == Edge::edge_from_index(vertices, 4));
    CHECK(nodes[4].upper_opp_edge == Edge::edge_from_index(vertices, 7));
    CHECK(nodes[4].neighbors[0] == &nodes[3]);
    CHECK(nodes[4].neighbors[1] == nullptr);
    CHECK(nodes[4].neighbors[2] == nullptr);

    CHECK(nodes[5].direction == HorizontalDirection::right);
    CHECK(nodes[5].vertex_it == vertices.begin() + 2);
    CHECK(nodes[5].lower_opp_edge == Edge::edge_from_index(vertices, 0));
    CHECK(nodes[5].upper_opp_edge == Edge::edge_from_index(vertices, 3));
    CHECK(nodes[5].neighbors[0] == &nodes[3]);
    CHECK(nodes[5].neighbors[1] == nullptr);
    CHECK(nodes[5].neighbors[2] == nullptr);
  }

  SECTION("Exterior, few nodes")
  {
    std::vector<Point2> vertices_storage{{-3.32, 1.46}, {-1.84, 1.48}, {-2.54, -0.26}, {0.64, 0.74}, {2.48, 3.02},
                                         {-1.58, 3.68}, {-0.60, 5.02}, {-2.72, 4.94},  {-4.34, 3.08}};
    ArrayView<const Point2> vertices(vertices_storage);

    VerticalDecomposition vertical_decomposition =
        vertical_decomposition_with_sweep_line_builder(vertices, VerticalDecompositionType::exterior_decomposition);

    REQUIRE(vertical_decomposition.nodes.size() == 4);
    const std::vector<Node>& nodes = vertical_decomposition.nodes;

    CHECK(nodes[0].direction == HorizontalDirection::right);
    CHECK(nodes[0].vertex_it == vertices.begin() + 8);
    CHECK(nodes[0].lower_opp_edge == Edge::invalid());
    CHECK(nodes[0].upper_opp_edge == Edge::invalid());
    CHECK(nodes[0].neighbors[0] == nullptr);
    CHECK(nodes[0].neighbors[1] == &nodes[1]);
    CHECK(nodes[0].neighbors[2] == &nodes[2]);

    CHECK(nodes[1].direction == HorizontalDirection::right);
    CHECK(nodes[1].vertex_it == vertices.begin() + 2);
    CHECK(nodes[1].lower_opp_edge == Edge::invalid());
    CHECK(nodes[1].upper_opp_edge == Edge::edge_from_index(vertices, 0));
    CHECK(nodes[1].neighbors[0] == &nodes[0]);
    CHECK(nodes[1].neighbors[1] == &nodes[3]);
    CHECK(nodes[1].neighbors[2] == nullptr);

    CHECK(nodes[2].direction == HorizontalDirection::left);
    CHECK(nodes[2].vertex_it == vertices.begin() + 6);
    CHECK(nodes[2].lower_opp_edge == Edge::edge_from_index(vertices, 4));
    CHECK(nodes[2].upper_opp_edge == Edge::invalid());
    CHECK(nodes[2].neighbors[0] == &nodes[3]);
    CHECK(nodes[2].neighbors[1] == nullptr);
    CHECK(nodes[2].neighbors[2] == &nodes[0]);

    CHECK(nodes[3].direction == HorizontalDirection::left);
    CHECK(nodes[3].vertex_it == vertices.begin() + 4);
    CHECK(nodes[3].lower_opp_edge == Edge::invalid());
    CHECK(nodes[3].upper_opp_edge == Edge::invalid());
    CHECK(nodes[3].neighbors[0] == nullptr);
    CHECK(nodes[3].neighbors[1] == &nodes[1]);
    CHECK(nodes[3].neighbors[2] == &nodes[2]);
  }

  SECTION("Vertices on same vertical line")
  {
    std::vector<Point2> vertices_storage = {{5, 1}, {10, 2}, {7, 3}, {10, 5}, {7, 6},
                                            {9, 7}, {2, 7},  {7, 5}, {3, 3},  {7, 2}};
    ArrayView<const Point2> vertices(vertices_storage);

    VerticalDecomposition vertical_decomposition =
        vertical_decomposition_with_sweep_line_builder(vertices, VerticalDecompositionType::interior_decomposition);

    REQUIRE(vertical_decomposition.nodes.size() == 4);
    const std::vector<Node>& nodes = vertical_decomposition.nodes;

    CHECK(nodes[0].direction == HorizontalDirection::left);
    CHECK(nodes[0].vertex_it == vertices.begin() + 9);
    CHECK(nodes[0].lower_opp_edge == Edge::edge_from_index(vertices, 0));
    CHECK(nodes[0].upper_opp_edge == Edge::edge_from_index(vertices, 7));
    CHECK(nodes[0].neighbors[0] == &nodes[1]);
    CHECK(nodes[0].neighbors[1] == nullptr);
    CHECK(nodes[0].neighbors[2] == nullptr);

    CHECK(nodes[1].direction == HorizontalDirection::right);
    CHECK(nodes[1].vertex_it == vertices.begin() + 2);
    CHECK(nodes[1].lower_opp_edge == Edge::edge_from_index(vertices, 0));
    CHECK(nodes[1].upper_opp_edge == Edge::edge_from_index(vertices, 7));
    CHECK(nodes[1].neighbors[0] == &nodes[0]);
    CHECK(nodes[1].neighbors[1] == nullptr);
    CHECK(nodes[1].neighbors[2] == &nodes[2]);

    CHECK(nodes[2].direction == HorizontalDirection::left);
    CHECK(nodes[2].vertex_it == vertices.begin() + 7);
    CHECK(nodes[2].lower_opp_edge == Edge::edge_from_index(vertices, 2));
    CHECK(nodes[2].upper_opp_edge == Edge::edge_from_index(vertices, 5));
    CHECK(nodes[2].neighbors[0] == &nodes[3]);
    CHECK(nodes[2].neighbors[1] == &nodes[1]);
    CHECK(nodes[2].neighbors[2] == nullptr);

    CHECK(nodes[3].direction == HorizontalDirection::right);
    CHECK(nodes[3].vertex_it == vertices.begin() + 4);
    CHECK(nodes[3].lower_opp_edge == Edge::edge_from_index(vertices, 2));
    CHECK(nodes[3].upper_opp_edge == Edge::edge_from_index(vertices, 5));
    CHECK(nodes[3].neighbors[0] == &nodes[2]);
    CHECK(nodes[3].neighbors[1] == nullptr);
    CHECK(nodes[3].neighbors[2] == nullptr);
  }
}

} // namespace dida::detail::vertical_decomposition