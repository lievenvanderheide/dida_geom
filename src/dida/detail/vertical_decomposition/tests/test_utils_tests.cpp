#include "dida/detail/vertical_decomposition/tests/test_utils.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_all.hpp>

#include "dida/detail/vertical_decomposition/sweep_line_builder.hpp"
#include "dida/polygon2.hpp"

namespace dida::detail::vertical_decomposition
{

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

TEST_CASE("node_branch_boundary_vertices")
{
  SECTION("Node towards left for CCW, towards right for CW")
  {
    std::vector<Point2> vertices_storage{
        {-4.48, 0.66}, {-2.42, 2.48}, {2.14, 1.86}, {5.92, 3.28}, {5.06, 5.14}, {1.16, 6.80},  {-1.66, 7.48},
        {-3.96, 6.54}, {-1.30, 5.66}, {1.08, 5.74}, {2.38, 4.48}, {0.90, 3.60}, {-1.04, 4.34}, {-3.04, 3.46},
    };
    VerticesView vertices(vertices_storage);

    Node node;
    node.direction = HorizontalDirection::left;
    node.type = NodeType::branch;
    node.vertex_it = vertices.begin() + 10;
    node.lower_opp_edge = Edge::edge_from_index(vertices, 2);
    node.upper_opp_edge = Edge::edge_from_index(vertices, 4);
    node.neighbors[0] = nullptr;
    node.neighbors[1] = nullptr;
    node.neighbors[2] = nullptr;

    Winding winding = GENERATE(Winding::ccw, Winding::cw);
    if (winding == Winding::cw)
    {
      flip_horizontally(vertices_storage, ArrayView<Node>(&node, 1));
    }

    SECTION("Branch 0")
    {
      NodeBranchBoundaryVertices result =
          node_branch_boundary_vertices(ChainDecomposition{nullptr, nullptr}, winding, &node, 0);
      CHECK(result.lower_boundary_vertex_it == vertices.begin() + 2);
      CHECK(result.upper_boundary_vertex_it == vertices.begin() + 5);
    }

    SECTION("Branch 1")
    {
      NodeBranchBoundaryVertices result =
          node_branch_boundary_vertices(ChainDecomposition{nullptr, nullptr}, winding, &node, 1);
      CHECK(result.lower_boundary_vertex_it == vertices.begin() + 3);
      CHECK(result.upper_boundary_vertex_it == vertices.begin() + 10);
    }

    SECTION("Branch 2")
    {
      NodeBranchBoundaryVertices result =
          node_branch_boundary_vertices(ChainDecomposition{nullptr, nullptr}, winding, &node, 2);
      CHECK(result.lower_boundary_vertex_it == vertices.begin() + 10);
      CHECK(result.upper_boundary_vertex_it == vertices.begin() + 4);
    }

    SECTION("Node is chain fist node, branch 1")
    {
      NodeBranchBoundaryVertices result =
          node_branch_boundary_vertices(ChainDecomposition{&node, nullptr}, winding, &node, 1);
      CHECK(result.lower_boundary_vertex_it == vertices.begin() + 3);
      CHECK(result.upper_boundary_vertex_it == vertices.begin() + 10);
    }

    SECTION("Node is chain first node, branch 2")
    {
      NodeBranchBoundaryVertices result =
          node_branch_boundary_vertices(ChainDecomposition{&node, nullptr}, winding, &node, 2);
      CHECK(result.lower_boundary_vertex_it == nullptr);
      CHECK(result.upper_boundary_vertex_it == vertices.begin() + 4);
    }

    SECTION("Node is chain last node, branch 1")
    {
      NodeBranchBoundaryVertices result =
          node_branch_boundary_vertices(ChainDecomposition{nullptr, &node}, winding, &node, 1);
      CHECK(result.lower_boundary_vertex_it == vertices.begin() + 3);
      CHECK(result.upper_boundary_vertex_it == nullptr);
    }

    SECTION("Node is chain last node, branch 2")
    {
      NodeBranchBoundaryVertices result =
          node_branch_boundary_vertices(ChainDecomposition{nullptr, &node}, winding, &node, 2);
      CHECK(result.lower_boundary_vertex_it == vertices.begin() + 10);
      CHECK(result.upper_boundary_vertex_it == vertices.begin() + 4);
    }

    SECTION("Node type is outer_branch, branch 1")
    {
      node.type = NodeType::outer_branch;
      NodeBranchBoundaryVertices result =
          node_branch_boundary_vertices(ChainDecomposition{nullptr, nullptr}, winding, &node, 1);
      CHECK(result.lower_boundary_vertex_it == vertices.begin() + 3);
      CHECK(result.upper_boundary_vertex_it == nullptr);
    }

    SECTION("Node type is outer_branch, branch 2")
    {
      node.type = NodeType::outer_branch;
      NodeBranchBoundaryVertices result =
          node_branch_boundary_vertices(ChainDecomposition{nullptr, nullptr}, winding, &node, 2);
      CHECK(result.lower_boundary_vertex_it == nullptr);
      CHECK(result.upper_boundary_vertex_it == vertices.begin() + 4);
    }
  } // namespace dida::detail::vertical_decomposition

  SECTION("Node towards right for CCW, towards left for CW")
  {
    std::vector<Point2> vertices_storage{
        {-4.64, 2.18}, {-0.92, 2.84}, {1.36, 1.44}, {6.34, 0.32}, {4.24, 2.96},  {0.70, 4.32},
        {3.88, 6.22},  {5.10, 6.12},  {3.48, 7.76}, {1.92, 6.46}, {-0.72, 7.66}, {-4.28, 5.32},
    };
    VerticesView vertices(vertices_storage);

    Node node;
    node.direction = HorizontalDirection::right;
    node.type = NodeType::branch;
    node.vertex_it = vertices.begin() + 5;
    node.lower_opp_edge = Edge::edge_from_index(vertices, 1);
    node.upper_opp_edge = Edge::edge_from_index(vertices, 9);
    node.neighbors[0] = nullptr;
    node.neighbors[1] = nullptr;
    node.neighbors[2] = nullptr;

    Winding winding = GENERATE(Winding::ccw, Winding::cw);
    if (winding == Winding::cw)
    {
      flip_horizontally(vertices_storage, ArrayView<Node>(&node, 1));
    }

    SECTION("Branch 0")
    {
      NodeBranchBoundaryVertices result =
          node_branch_boundary_vertices(ChainDecomposition{nullptr, nullptr}, winding, &node, 0);
      CHECK(result.lower_boundary_vertex_it == vertices.begin() + 2);
      CHECK(result.upper_boundary_vertex_it == vertices.begin() + 9);
    }

    SECTION("Branch 1")
    {
      NodeBranchBoundaryVertices result =
          node_branch_boundary_vertices(ChainDecomposition{nullptr, nullptr}, winding, &node, 1);
      CHECK(result.lower_boundary_vertex_it == vertices.begin() + 1);
      CHECK(result.upper_boundary_vertex_it == vertices.begin() + 5);
    }

    SECTION("Branch 2")
    {
      NodeBranchBoundaryVertices result =
          node_branch_boundary_vertices(ChainDecomposition{nullptr, nullptr}, winding, &node, 2);
      CHECK(result.lower_boundary_vertex_it == vertices.begin() + 5);
      CHECK(result.upper_boundary_vertex_it == vertices.begin() + 10);
    }

    SECTION("Node is chain fist node, branch 1")
    {
      NodeBranchBoundaryVertices result =
          node_branch_boundary_vertices(ChainDecomposition{&node, nullptr}, winding, &node, 1);
      CHECK(result.lower_boundary_vertex_it == vertices.begin() + 1);
      CHECK(result.upper_boundary_vertex_it == nullptr);
    }

    SECTION("Node is chain first node, branch 2")
    {
      NodeBranchBoundaryVertices result =
          node_branch_boundary_vertices(ChainDecomposition{&node, nullptr}, winding, &node, 2);
      CHECK(result.lower_boundary_vertex_it == vertices.begin() + 5);
      CHECK(result.upper_boundary_vertex_it == vertices.begin() + 10);
    }

    SECTION("Node is chain last node, branch 1")
    {
      NodeBranchBoundaryVertices result =
          node_branch_boundary_vertices(ChainDecomposition{nullptr, &node}, winding, &node, 1);
      CHECK(result.lower_boundary_vertex_it == vertices.begin() + 1);
      CHECK(result.upper_boundary_vertex_it == vertices.begin() + 5);
    }

    SECTION("Node is chain last node, branch 2")
    {
      NodeBranchBoundaryVertices result =
          node_branch_boundary_vertices(ChainDecomposition{nullptr, &node}, winding, &node, 2);
      CHECK(result.lower_boundary_vertex_it == nullptr);
      CHECK(result.upper_boundary_vertex_it == vertices.begin() + 10);
    }

    SECTION("Node type is outer_branch, branch 1")
    {
      node.type = NodeType::outer_branch;
      NodeBranchBoundaryVertices result =
          node_branch_boundary_vertices(ChainDecomposition{nullptr, nullptr}, winding, &node, 1);
      CHECK(result.lower_boundary_vertex_it == vertices.begin() + 1);
      CHECK(result.upper_boundary_vertex_it == nullptr);
    }

    SECTION("Node type is outer_branch, branch 2")
    {
      node.type = NodeType::outer_branch;
      NodeBranchBoundaryVertices result =
          node_branch_boundary_vertices(ChainDecomposition{nullptr, nullptr}, winding, &node, 2);
      CHECK(result.lower_boundary_vertex_it == nullptr);
      CHECK(result.upper_boundary_vertex_it == vertices.begin() + 10);
    }
  }

  SECTION("Leaf nodes")
  {
    std::vector<Point2> vertices_storage{
        {-5.26, 1.80}, {-3.14, 1.56}, {-0.74, 2.54}, {1.46, 1.12},
        {3.94, 2.90},  {-0.58, 5.86}, {-2.92, 3.84}, {-4.32, 4.22},
    };
    VerticesView vertices(vertices_storage);

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

      Winding winding = GENERATE(Winding::ccw, Winding::cw);
      if (winding == Winding::cw)
      {
        flip_horizontally(vertices_storage, ArrayView<Node>(&node, 1));
      }

      NodeBranchBoundaryVertices result =
          node_branch_boundary_vertices(ChainDecomposition{nullptr, nullptr}, winding, &node, 0);
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

      Winding winding = GENERATE(Winding::ccw, Winding::cw);
      if (winding == Winding::cw)
      {
        flip_horizontally(vertices_storage, ArrayView<Node>(&node, 1));
      }

      NodeBranchBoundaryVertices result =
          node_branch_boundary_vertices(ChainDecomposition{nullptr, nullptr}, winding, &node, 0);
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

TEST_CASE("node_type_to_string")
{
  CHECK(node_type_to_string(NodeType::leaf) == "NodeType::leaf");
  CHECK(node_type_to_string(NodeType::branch) == "NodeType::branch");
  CHECK(node_type_to_string(NodeType::outer_branch) == "NodeType::outer_branch");
}

TEST_CASE("flip_horizontally")
{
  std::vector<Point2> vertices_storage{{-2.76, 5.60}, {0.08, 5.42}, {-1.48, 3.20}};
  VerticesView vertices(vertices_storage);

  std::vector<Node> nodes(2);
  nodes[0].direction = HorizontalDirection::right;
  nodes[0].type = NodeType::branch;
  nodes[0].vertex_it = vertices.begin() + 0;
  nodes[0].lower_opp_edge = Edge::invalid();
  nodes[0].upper_opp_edge = Edge::invalid();
  nodes[0].neighbors[0] = nullptr;
  nodes[0].neighbors[1] = &nodes[1];
  nodes[0].neighbors[2] = &nodes[1];

  nodes[1].direction = HorizontalDirection::left;
  nodes[1].type = NodeType::branch;
  nodes[1].vertex_it = vertices.begin() + 1;
  nodes[1].lower_opp_edge = Edge::invalid();
  nodes[1].upper_opp_edge = Edge::invalid();
  nodes[1].neighbors[0] = nullptr;
  nodes[1].neighbors[1] = &nodes[0];
  nodes[1].neighbors[2] = &nodes[0];

  flip_horizontally(vertices_storage, nodes);

  CHECK(vertices[0] == Point2(2.76, 5.60));
  CHECK(vertices[1] == Point2(-0.08, 5.42));
  CHECK(vertices[2] == Point2(1.48, 3.20));

  CHECK(nodes[0].direction == HorizontalDirection::left);
  CHECK(nodes[1].direction == HorizontalDirection::right);
}

} // namespace dida::detail::vertical_decomposition