#include "dida/detail/vertical_decomposition/zigzag_phase.hpp"

#include <catch2/catch_test_macros.hpp>

#include "dida/detail/vertical_decomposition/tests/test_utils.hpp"
#include "dida/polygon2.hpp"

namespace dida::detail::vertical_decomposition
{

TEST_CASE("vertical_decomposition_zigzag_phase")
{
  SECTION("Zigzag simple")
  {
    Polygon2 polygon{
        {-2.76, 5.04}, {-4.98, 3.88}, {-1.24, 3.48}, {1.74, 2.22}, {-0.98, 0.92}, {-2.96, 0.96},
        {0.68, -0.70}, {6.42, 2.46},  {3.06, 4.12},  {5.92, 5.56}, {0.24, 7.14},
    };
    VerticesView vertices(polygon);

    NodePool node_pool;
    std::vector<ChainDecomposition> chain_decompositions = vertical_decomposition_zigzag_phase(vertices, node_pool);

    REQUIRE(chain_decompositions.size() == 2);

    CHECK(chain_decompositions[0].first_node->vertex_it == vertices.begin() + 1);
    CHECK(chain_decompositions[0].last_node->vertex_it == vertices.begin() + 7);
    CHECK(validate_chain_decomposition(vertices, chain_decompositions[0]));

    CHECK(chain_decompositions[1].first_node->vertex_it == vertices.begin() + 7);
    CHECK(chain_decompositions[1].last_node->vertex_it == vertices.begin() + 1);
    CHECK(validate_chain_decomposition(vertices, chain_decompositions[1]));
  }

  SECTION("Zigzag complex")
  {
    Polygon2 polygon{
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
    VerticesView vertices(polygon);

    NodePool node_pool;
    std::vector<ChainDecomposition> chain_decompositions = vertical_decomposition_zigzag_phase(vertices, node_pool);

    REQUIRE(chain_decompositions.size() == 2);

    CHECK(chain_decompositions[0].first_node->vertex_it == vertices.begin() + 1);
    CHECK(chain_decompositions[0].last_node->vertex_it == vertices.begin() + 28);
    CHECK(validate_chain_decomposition(vertices, chain_decompositions[0]));

    CHECK(chain_decompositions[1].first_node->vertex_it == vertices.begin() + 28);
    CHECK(chain_decompositions[1].last_node->vertex_it == vertices.begin() + 1);
    CHECK(validate_chain_decomposition(vertices, chain_decompositions[1]));
  }

  SECTION("Greater sinuosity")
  {
    Polygon2 polygon{
        {-2.58, 6.06}, {-3.74, 5.10}, {-0.68, 3.44}, {-3.68, 1.86}, {-6.74, 2.70}, {-2.84, 3.44},
        {-5.40, 4.80}, {-3.64, 6.12}, {-8.64, 6.96}, {-4.92, 3.72}, {-8.00, 3.08}, {-5.48, 0.40},
        {-2.64, 1.00}, {1.62, 2.80},  {2.94, 2.42},  {1.48, 1.42},  {5.98, 2.14},  {4.38, 3.96},
        {7.00, 4.04},  {7.74, 2.42},  {5.12, 1.02},  {8.32, 2.54},  {7.44, 4.94},  {2.64, 4.50},
    };

    VerticesView vertices(polygon);

    NodePool node_pool;
    std::vector<ChainDecomposition> chain_decompositions = vertical_decomposition_zigzag_phase(vertices, node_pool);

    REQUIRE(chain_decompositions.size() == 4);

    CHECK(chain_decompositions[0].first_node->vertex_it == vertices.begin() + 1);
    CHECK(chain_decompositions[0].last_node->vertex_it == vertices.begin() + 8);
    CHECK(validate_chain_decomposition(vertices, chain_decompositions[0]));

    CHECK(chain_decompositions[1].first_node->vertex_it == vertices.begin() + 8);
    CHECK(chain_decompositions[1].last_node->vertex_it == vertices.begin() + 16);
    CHECK(validate_chain_decomposition(vertices, chain_decompositions[1]));

    CHECK(chain_decompositions[2].first_node->vertex_it == vertices.begin() + 16);
    CHECK(chain_decompositions[2].last_node->vertex_it == vertices.begin() + 21);
    CHECK(validate_chain_decomposition(vertices, chain_decompositions[2]));

    CHECK(chain_decompositions[3].first_node->vertex_it == vertices.begin() + 21);
    CHECK(chain_decompositions[3].last_node->vertex_it == vertices.begin() + 1);
    CHECK(validate_chain_decomposition(vertices, chain_decompositions[3]));
  }

  SECTION("Opposites for chain last nodes")
  {
    Polygon2 polygon{
        {-3.68, 5.88}, {-5.18, 4.80}, {0.66, 3.04}, {-3.34, 0.56}, {-0.68, -0.58}, {-1.62, 0.60},
        {5.30, 2.64},  {0.40, 5.50},  {4.24, 6.98}, {0.86, 8.10},  {1.60, 6.82},   {-1.52, 6.70},
    };

    VerticesView vertices(polygon);

    NodePool node_pool;
    std::vector<ChainDecomposition> chain_decompositions = vertical_decomposition_zigzag_phase(vertices, node_pool);

    REQUIRE(chain_decompositions.size() == 3);

    CHECK(chain_decompositions[0].first_node->vertex_it == vertices.begin() + 1);
    CHECK(chain_decompositions[0].last_node->vertex_it == vertices.begin() + 4);
    CHECK(validate_chain_decomposition(vertices, chain_decompositions[0]));

    CHECK(chain_decompositions[1].first_node->vertex_it == vertices.begin() + 4);
    CHECK(chain_decompositions[1].last_node->vertex_it == vertices.begin() + 9);
    CHECK(validate_chain_decomposition(vertices, chain_decompositions[1]));

    CHECK(chain_decompositions[2].first_node->vertex_it == vertices.begin() + 9);
    CHECK(chain_decompositions[2].last_node->vertex_it == vertices.begin() + 1);
    CHECK(validate_chain_decomposition(vertices, chain_decompositions[2]));
  }

  SECTION("Spiral")
  {
    Polygon2 polygon{
        {-0.02, -1.36}, {-5.40, 2.06}, {0.32, -2.64}, {7.34, 2.94},  {0.12, 9.06}, {-4.80, 4.02},
        {0.18, 0.04},   {4.56, 3.06},  {0.14, 6.30},  {-1.64, 4.06}, {0.14, 2.52}, {2.08, 3.48},
        {-0.14, 1.30},  {-2.50, 3.82}, {-0.02, 7.80}, {5.84, 3.02},
    };

    VerticesView vertices(polygon);

    NodePool node_pool;
    std::vector<ChainDecomposition> chain_decompositions = vertical_decomposition_zigzag_phase(vertices, node_pool);

    REQUIRE(chain_decompositions.size() == 3);

    CHECK(chain_decompositions[0].first_node->vertex_it == vertices.begin() + 1);
    CHECK(chain_decompositions[0].last_node->vertex_it == vertices.begin() + 5);
    CHECK(validate_chain_decomposition(vertices, chain_decompositions[0]));

    CHECK(chain_decompositions[1].first_node->vertex_it == vertices.begin() + 5);
    CHECK(chain_decompositions[1].last_node->vertex_it == vertices.begin() + 9);
    CHECK(validate_chain_decomposition(vertices, chain_decompositions[1]));

    CHECK(chain_decompositions[2].first_node->vertex_it == vertices.begin() + 9);
    CHECK(chain_decompositions[2].last_node->vertex_it == vertices.begin() + 1);
    CHECK(validate_chain_decomposition(vertices, chain_decompositions[2]));
  }

  SECTION("Monotone polygon")
  {
    SECTION("First node vertex on left side")
    {
      Polygon2 polygon{{-2.08, 6.00}, {-4.26, 4.70}, {-6.92, 5.36}, {-5.84, 2.54}, {-1.86, 3.64}, {0.70, 2.26}};

      VerticesView vertices(polygon);

      NodePool node_pool;
      std::vector<ChainDecomposition> chain_decompositions = vertical_decomposition_zigzag_phase(vertices, node_pool);

      REQUIRE(chain_decompositions.size() == 1);
      CHECK(chain_decompositions[0].first_node->vertex_it == vertices.begin() + 2);
      CHECK(validate_polygon_decomposition(vertices, chain_decompositions[0].first_node));
    }

    SECTION("First node vertex on right side")
    {
      Polygon2 polygon{
          {-3.48, 3.04}, {-0.68, 3.68}, {2.90, 2.52}, {4.92, 4.50}, {1.50, 7.52}, {-4.84, 5.36}, {-7.22, 5.40},
      };

      VerticesView vertices(polygon);

      NodePool node_pool;
      std::vector<ChainDecomposition> chain_decompositions = vertical_decomposition_zigzag_phase(vertices, node_pool);

      REQUIRE(chain_decompositions.size() == 1);
      CHECK(chain_decompositions[0].first_node->vertex_it == vertices.begin() + 3);
      CHECK(validate_polygon_decomposition(vertices, chain_decompositions[0].first_node));
    }
  }
}

} // namespace dida::detail::vertical_decomposition