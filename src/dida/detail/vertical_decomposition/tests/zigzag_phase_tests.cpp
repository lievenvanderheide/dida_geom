#include "dida/detail/vertical_decomposition/zigzag_phase.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_all.hpp>

#include "dida/detail/vertical_decomposition/tests/test_utils.hpp"
#include "dida/polygon2.hpp"

namespace dida::detail::vertical_decomposition
{

TEST_CASE("interior_zigzag_phase")
{
  SECTION("Zigzag simple")
  {
    std::vector<Point2> vertices_storage{
        {-2.76, 5.04}, {-4.98, 3.88}, {-1.24, 3.48}, {1.74, 2.22}, {-0.98, 0.92}, {-2.96, 0.96},
        {0.68, -0.70}, {6.42, 2.46},  {3.06, 4.12},  {5.92, 5.56}, {0.24, 7.14},
    };
    VerticesView vertices(vertices_storage);

    Winding winding = GENERATE(Winding::ccw, Winding::cw);
    if (winding == Winding::cw)
    {
      flip_horizontally(vertices_storage);
    }

    NodePool node_pool;
    std::vector<ChainDecomposition> chain_decompositions = interior_zigzag_phase(vertices, winding, node_pool);

    REQUIRE(chain_decompositions.size() == 2);

    CHECK(chain_decompositions[0].first_node->vertex_it == vertices.begin() + 1);
    CHECK(chain_decompositions[0].last_node->vertex_it == vertices.begin() + 7);
    CHECK(validate_chain_decomposition(vertices, winding, chain_decompositions[0]));

    CHECK(chain_decompositions[1].first_node->vertex_it == vertices.begin() + 7);
    CHECK(chain_decompositions[1].last_node->vertex_it == vertices.begin() + 1);
    CHECK(validate_chain_decomposition(vertices, winding, chain_decompositions[1]));
  }

  SECTION("Zigzag complex")
  {
    std::vector<Point2> vertices_storage{
        {-5.30, 6.28},  {-6.24, 4.86}, {-5.24, 4.88},  {-4.82, 4.34},  {-4.12, 4.26},  {-3.44, 4.42},  {-2.80, 4.12},
        {-1.46, 3.94},  {-0.50, 3.40}, {-1.60, 3.20},  {-2.54, 3.26},  {-3.56, 2.70},  {-4.78, 2.72},  {-4.18, 2.16},
        {-3.32, 2.18},  {-2.82, 1.88}, {-3.52, 1.18},  {-4.26, 1.14},  {-5.36, 1.40},  {-4.30, -0.18}, {-3.48, 0.12},
        {-3.00, -0.08}, {-1.50, 0.74}, {-1.88, -0.26}, {-3.14, -0.48}, {-1.62, -0.98}, {0.82, -0.24},  {2.74, -1.12},
        {5.12, -0.66},  {4.44, -0.02}, {3.60, -0.06},  {2.84, 0.74},   {2.04, 0.80},   {1.18, 1.32},   {2.42, 1.76},
        {3.40, 1.58},   {4.24, 1.70},  {5.22, 2.40},   {4.18, 2.58},   {3.46, 2.40},   {2.56, 2.90},   {3.86, 3.62},
        {5.04, 3.54},   {4.40, 4.16},  {3.08, 3.74},   {2.42, 4.12},   {1.38, 3.94},   {2.50, 5.14},   {3.82, 5.16},
        {4.58, 5.64},   {5.44, 5.54},  {6.86, 6.50},   {5.12, 7.46},   {3.88, 8.22},   {2.86, 7.44},   {1.56, 7.86},
        {0.16, 6.52},   {-2.58, 7.28}, {-3.88, 5.54},
    };
    VerticesView vertices(vertices_storage);

    Winding winding = GENERATE(Winding::ccw, Winding::cw);
    if (winding == Winding::cw)
    {
      flip_horizontally(vertices_storage);
    }

    NodePool node_pool;
    std::vector<ChainDecomposition> chain_decompositions = interior_zigzag_phase(vertices, winding, node_pool);

    REQUIRE(chain_decompositions.size() == 2);

    CHECK(chain_decompositions[0].first_node->vertex_it == vertices.begin() + 1);
    CHECK(chain_decompositions[0].last_node->vertex_it == vertices.begin() + 28);
    CHECK(validate_chain_decomposition(vertices, winding, chain_decompositions[0]));

    CHECK(chain_decompositions[1].first_node->vertex_it == vertices.begin() + 28);
    CHECK(chain_decompositions[1].last_node->vertex_it == vertices.begin() + 1);
    CHECK(validate_chain_decomposition(vertices, winding, chain_decompositions[1]));
  }

  SECTION("Greater sinuosity")
  {
    std::vector<Point2> vertices_storage{
        {-2.58, 6.06}, {-3.74, 5.10}, {-0.68, 3.44}, {-3.68, 1.86}, {-6.74, 2.70}, {-2.84, 3.44},
        {-5.40, 4.80}, {-3.64, 6.12}, {-8.64, 6.96}, {-4.92, 3.72}, {-8.00, 3.08}, {-5.48, 0.40},
        {-2.64, 1.00}, {1.62, 2.80},  {2.94, 2.42},  {1.48, 1.42},  {5.98, 2.14},  {4.38, 3.96},
        {7.00, 4.04},  {7.74, 2.42},  {5.12, 1.02},  {8.32, 2.54},  {7.44, 4.94},  {2.64, 4.50},
    };

    VerticesView vertices(vertices_storage);

    Winding winding = GENERATE(Winding::ccw, Winding::cw);
    if (winding == Winding::cw)
    {
      flip_horizontally(vertices_storage);
    }

    NodePool node_pool;
    std::vector<ChainDecomposition> chain_decompositions = interior_zigzag_phase(vertices, winding, node_pool);

    REQUIRE(chain_decompositions.size() == 4);

    CHECK(chain_decompositions[0].first_node->vertex_it == vertices.begin() + 1);
    CHECK(chain_decompositions[0].last_node->vertex_it == vertices.begin() + 8);
    CHECK(validate_chain_decomposition(vertices, winding, chain_decompositions[0]));

    CHECK(chain_decompositions[1].first_node->vertex_it == vertices.begin() + 8);
    CHECK(chain_decompositions[1].last_node->vertex_it == vertices.begin() + 16);
    CHECK(validate_chain_decomposition(vertices, winding, chain_decompositions[1]));

    CHECK(chain_decompositions[2].first_node->vertex_it == vertices.begin() + 16);
    CHECK(chain_decompositions[2].last_node->vertex_it == vertices.begin() + 21);
    CHECK(validate_chain_decomposition(vertices, winding, chain_decompositions[2]));

    CHECK(chain_decompositions[3].first_node->vertex_it == vertices.begin() + 21);
    CHECK(chain_decompositions[3].last_node->vertex_it == vertices.begin() + 1);
    CHECK(validate_chain_decomposition(vertices, winding, chain_decompositions[3]));
  }

  SECTION("Opposites for chain last nodes")
  {
    std::vector<Point2> vertices_storage{
        {-3.68, 5.88}, {-5.18, 4.80}, {0.66, 3.04}, {-3.34, 0.56}, {-0.68, -0.58}, {-1.62, 0.60},
        {5.30, 2.64},  {0.40, 5.50},  {4.24, 6.98}, {0.86, 8.10},  {1.60, 6.82},   {-1.52, 6.70},
    };

    VerticesView vertices(vertices_storage);

    Winding winding = GENERATE(Winding::ccw, Winding::cw);
    if (winding == Winding::cw)
    {
      flip_horizontally(vertices_storage);
    }

    NodePool node_pool;
    std::vector<ChainDecomposition> chain_decompositions = interior_zigzag_phase(vertices, winding, node_pool);

    REQUIRE(chain_decompositions.size() == 3);

    CHECK(chain_decompositions[0].first_node->vertex_it == vertices.begin() + 1);
    CHECK(chain_decompositions[0].last_node->vertex_it == vertices.begin() + 4);
    CHECK(validate_chain_decomposition(vertices, winding, chain_decompositions[0]));

    CHECK(chain_decompositions[1].first_node->vertex_it == vertices.begin() + 4);
    CHECK(chain_decompositions[1].last_node->vertex_it == vertices.begin() + 9);
    CHECK(validate_chain_decomposition(vertices, winding, chain_decompositions[1]));

    CHECK(chain_decompositions[2].first_node->vertex_it == vertices.begin() + 9);
    CHECK(chain_decompositions[2].last_node->vertex_it == vertices.begin() + 1);
    CHECK(validate_chain_decomposition(vertices, winding, chain_decompositions[2]));
  }

  SECTION("Spiral")
  {
    std::vector<Point2> vertices_storage{
        {-0.02, -1.36}, {-5.40, 2.06}, {0.32, -2.64}, {7.34, 2.94},  {0.12, 9.06}, {-4.80, 4.02},
        {0.18, 0.04},   {4.56, 3.06},  {0.14, 6.30},  {-1.64, 4.06}, {0.14, 2.52}, {2.08, 3.48},
        {-0.14, 1.30},  {-2.50, 3.82}, {-0.02, 7.80}, {5.84, 3.02},
    };

    VerticesView vertices(vertices_storage);

    Winding winding = GENERATE(Winding::ccw, Winding::cw);
    if (winding == Winding::cw)
    {
      flip_horizontally(vertices_storage);
    }

    NodePool node_pool;
    std::vector<ChainDecomposition> chain_decompositions = interior_zigzag_phase(vertices, winding, node_pool);

    REQUIRE(chain_decompositions.size() == 3);

    CHECK(chain_decompositions[0].first_node->vertex_it == vertices.begin() + 1);
    CHECK(chain_decompositions[0].last_node->vertex_it == vertices.begin() + 5);
    CHECK(validate_chain_decomposition(vertices, winding, chain_decompositions[0]));

    CHECK(chain_decompositions[1].first_node->vertex_it == vertices.begin() + 5);
    CHECK(chain_decompositions[1].last_node->vertex_it == vertices.begin() + 9);
    CHECK(validate_chain_decomposition(vertices, winding, chain_decompositions[1]));

    CHECK(chain_decompositions[2].first_node->vertex_it == vertices.begin() + 9);
    CHECK(chain_decompositions[2].last_node->vertex_it == vertices.begin() + 1);
    CHECK(validate_chain_decomposition(vertices, winding, chain_decompositions[2]));
  }

  SECTION("Monotone polygon")
  {
    SECTION("First node vertex on left side")
    {
      std::vector<Point2> vertices_storage{
          {-2.08, 6.00}, {-4.26, 4.70}, {-6.92, 5.36}, {-5.84, 2.54}, {-1.86, 3.64}, {0.70, 2.26},
      };
      VerticesView vertices(vertices_storage);

      Winding winding = GENERATE(Winding::ccw, Winding::cw);
      if (winding == Winding::cw)
      {
        flip_horizontally(vertices_storage);
      }

      NodePool node_pool;
      std::vector<ChainDecomposition> chain_decompositions = interior_zigzag_phase(vertices, winding, node_pool);

      REQUIRE(chain_decompositions.size() == 1);
      CHECK(chain_decompositions[0].first_node->vertex_it == vertices.begin() + 2);
      CHECK(validate_polygon_decomposition(vertices, winding, chain_decompositions[0].first_node));
    }

    SECTION("First node vertex on right side")
    {
      std::vector<Point2> vertices_storage{
          {-3.48, 3.04}, {-0.68, 3.68}, {2.90, 2.52}, {4.92, 4.50}, {1.50, 7.52}, {-4.84, 5.36}, {-7.22, 5.40},
      };

      VerticesView vertices(vertices_storage);

      Winding winding = GENERATE(Winding::ccw, Winding::cw);
      if (winding == Winding::cw)
      {
        flip_horizontally(vertices_storage);
      }

      NodePool node_pool;
      std::vector<ChainDecomposition> chain_decompositions = interior_zigzag_phase(vertices, winding, node_pool);

      REQUIRE(chain_decompositions.size() == 1);
      CHECK(chain_decompositions[0].first_node->vertex_it == vertices.begin() + 3);
      CHECK(validate_polygon_decomposition(vertices, winding, chain_decompositions[0].first_node));
    }
  }
}

namespace
{

void validate_exterior_zigzag_phase_result(ExteriorChainDecompositions& result, VerticesView vertices, Winding winding,
                                           size_t expected_leftmost_vertex_index,
                                           size_t expected_rightmost_vertex_index,
                                           size_t expected_num_lower_chain_decompositions,
                                           size_t expected_num_upper_chain_decompositions)
{
  CHECK(result.leftmost_node->vertex_it == vertices.begin() + expected_leftmost_vertex_index);
  CHECK(result.rightmost_node->vertex_it == vertices.begin() + expected_rightmost_vertex_index);

  REQUIRE(result.lower_chain_decompositions.size() == expected_num_lower_chain_decompositions);
  REQUIRE(result.upper_chain_decompositions.size() == expected_num_upper_chain_decompositions);

  if (winding == Winding::ccw)
  {
    CHECK(result.lower_chain_decompositions.front().first_node == result.rightmost_node);
    CHECK(result.lower_chain_decompositions.back().last_node == result.leftmost_node);

    CHECK(result.upper_chain_decompositions.front().first_node == result.leftmost_node);
    CHECK(result.upper_chain_decompositions.back().last_node == result.rightmost_node);
  }
  else
  {
    CHECK(result.lower_chain_decompositions.front().first_node == result.leftmost_node);
    CHECK(result.lower_chain_decompositions.back().last_node == result.rightmost_node);

    CHECK(result.upper_chain_decompositions.front().first_node == result.rightmost_node);
    CHECK(result.upper_chain_decompositions.back().last_node == result.leftmost_node);
  }

  {
    Node* leftmost_node_upper_neighbor_backup = result.leftmost_node->neighbors[2];
    result.leftmost_node->neighbors[2] = nullptr;

    Node* rightmost_node_upper_neighbor_backup = result.rightmost_node->neighbors[2];
    result.rightmost_node->neighbors[2] = nullptr;

    for (const ChainDecomposition& chain_decomposition : result.lower_chain_decompositions)
    {
      CHECK(validate_chain_decomposition(vertices, winding, chain_decomposition));
    }

    result.leftmost_node->neighbors[2] = leftmost_node_upper_neighbor_backup;
    result.rightmost_node->neighbors[2] = rightmost_node_upper_neighbor_backup;
  }

  {
    Node* leftmost_node_lower_neighbor_backup = result.leftmost_node->neighbors[1];
    result.leftmost_node->neighbors[1] = nullptr;

    Node* rightmost_node_lower_neighbor_backup = result.rightmost_node->neighbors[1];
    result.rightmost_node->neighbors[1] = nullptr;

    for (const ChainDecomposition& chain_decomposition : result.upper_chain_decompositions)
    {
      CHECK(validate_chain_decomposition(vertices, winding, chain_decomposition));
    }

    result.leftmost_node->neighbors[1] = leftmost_node_lower_neighbor_backup;
    result.rightmost_node->neighbors[1] = rightmost_node_lower_neighbor_backup;
  }
}

} // namespace

TEST_CASE("exterior_zigzag_phase")
{
  SECTION("Monotone polygon")
  {
    std::vector<Point2> vertices_storage{
        {-4.24, 4.70}, {-6.56, 3.62}, {-5.24, 0.74}, {-0.62, 0.14}, {1.82, 1.76},
        {4.30, 0.30},  {6.08, 2.06},  {4.54, 5.20},  {-1.98, 6.96},
    };

    size_t expected_leftmost_vertex_index = 1;
    size_t expected_rightmost_vertex_index = 6;

    Winding winding = GENERATE(Winding::cw, Winding::ccw);
    if (winding == Winding::ccw)
    {
      flip_horizontally(vertices_storage);
      std::swap(expected_leftmost_vertex_index, expected_rightmost_vertex_index);
    }

    VerticesView vertices(vertices_storage);

    NodePool node_pool;
    ExteriorChainDecompositions chain_decompositions = exterior_zigzag_phase(vertices, winding, node_pool);

    validate_exterior_zigzag_phase_result(chain_decompositions, vertices, winding, expected_leftmost_vertex_index,
                                          expected_rightmost_vertex_index, 1, 1);
  }

  SECTION("Multiply chains")
  {
    std::vector<Point2> vertices_storage{
        {-3.86, 8.50}, {-3.82, 6.90}, {-5.34, 6.00}, {-8.60, 5.76}, {-5.12, 5.16},  {-2.20, 3.22}, {-4.62, 1.48},
        {-5.96, 1.90}, {-4.84, 3.24}, {-6.90, 2.16}, {-5.26, 0.46}, {-3.12, -0.02}, {-0.94, 1.50}, {2.08, 1.24},
        {3.76, 0.56},  {5.80, 2.06},  {5.02, 3.96},  {3.16, 4.26},  {4.62, 2.86},   {4.54, 1.92},  {2.58, 1.88},
        {1.46, 3.80},  {2.38, 5.50},  {7.30, 5.66},  {3.20, 6.54},  {2.46, 7.76},   {3.82, 9.28},  {5.60, 8.10},
        {4.44, 7.46},  {6.34, 7.66},  {6.40, 9.28},  {3.68, 9.84},  {1.22, 7.78},   {-2.26, 7.64}, {-4.04, 9.50},
        {-6.48, 9.50}, {-8.00, 8.58}, {-8.12, 7.02}, {-6.68, 6.68}, {-5.50, 7.54},  {-6.96, 7.38}, {-7.48, 8.28},
        {-6.26, 8.94},
    };

    size_t expected_leftmost_vertex_index = 3;
    size_t expected_rightmost_vertex_index = 23;

    Winding winding = GENERATE(Winding::cw, Winding::ccw);
    if (winding == Winding::ccw)
    {
      flip_horizontally(vertices_storage);
      std::swap(expected_leftmost_vertex_index, expected_rightmost_vertex_index);
    }

    VerticesView vertices(vertices_storage);

    NodePool node_pool;
    ExteriorChainDecompositions chain_decompositions = exterior_zigzag_phase(vertices, winding, node_pool);

    validate_exterior_zigzag_phase_result(chain_decompositions, vertices, winding, expected_leftmost_vertex_index,
                                          expected_rightmost_vertex_index, 3, 3);
  }
}

} // namespace dida::detail::vertical_decomposition