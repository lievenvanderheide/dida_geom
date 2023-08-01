#include "dida/detail/vertical_decomposition_merge.hpp"

#include <catch2/catch.hpp>
#include <deque>
#include <iostream>

#include "dida/detail/tests/vertical_decomposition_test_utils.hpp"
#include "dida/polygon2.hpp"

namespace dida::detail::vertical_decomposition
{

TEST_CASE("vertical_decomposition_merge")
{
  SECTION("Clams")
  {
    Polygon2 polygon{
        {0.66, 2.30}, {0.74, 1.12}, {2.54, 0.18}, {6.84, -0.48}, {9.94, 0.92}, {8.22, 2.08}, {6.76, 2.10},
        {7.16, 1.58}, {5.84, 2.44}, {7.22, 3.34}, {6.84, 2.80},  {7.78, 3.02}, {8.66, 3.78}, {7.20, 4.62},
        {4.82, 3.80}, {3.58, 4.30}, {1.56, 4.78}, {0.00, 3.98},  {1.82, 2.62}, {3.44, 2.44}, {3.06, 3.16},
        {4.36, 2.46}, {2.80, 1.18}, {3.28, 1.98}, {2.20, 1.30},
    };

    VerticesView vertices(polygon);
    NodePool node_pool;

    std::vector<ChainDecomposition> chain_decompositions = initial_chain_decompositions(vertices, node_pool);

    SECTION("Right facing, lower chain longer")
    {
      ChainDecomposition merged =
          merge_chain_decompositions(vertices, node_pool, chain_decompositions[7], chain_decompositions[0]);
      CHECK(validate_chain_decomposition(vertices, merged));
    }

    SECTION("Right facing, upper chain longer")
    {
      ChainDecomposition merged =
          merge_chain_decompositions(vertices, node_pool, chain_decompositions[4], chain_decompositions[5]);
      CHECK(validate_chain_decomposition(vertices, merged));
    }

    SECTION("Left facing, lower chain longer")
    {
      ChainDecomposition merged =
          merge_chain_decompositions(vertices, node_pool, chain_decompositions[0], chain_decompositions[1]);
      CHECK(validate_chain_decomposition(vertices, merged));
    }

    SECTION("Left facing, upper chain longer")
    {
      ChainDecomposition merged =
          merge_chain_decompositions(vertices, node_pool, chain_decompositions[3], chain_decompositions[4]);
      CHECK(validate_chain_decomposition(vertices, merged));
    }
  }

  SECTION("Enter forward branch")
  {
    SECTION("Towards right, enter lower branch")
    {
      Polygon2 polygon{
          {-5.16, 2.56}, {-3.14, 2.32}, {1.12, 3.28},  {3.70, 3.06},  {2.04, 5.02},  {-0.28, 5.14}, {1.40, 6.20},
          {3.92, 6.40},  {0.80, 7.74},  {-2.66, 5.58}, {-2.14, 4.52}, {0.88, 4.34},  {1.38, 4.30},  {1.20, 4.54},
          {2.04, 4.18},  {1.26, 3.98},  {1.40, 4.16},  {-1.58, 3.32}, {-3.26, 3.62},
      };

      VerticesView vertices(polygon);
      NodePool node_pool;

      std::vector<ChainDecomposition> chain_decompositions = initial_chain_decompositions(vertices, node_pool);
      REQUIRE(chain_decompositions.size() == 6);

      ChainDecomposition branches =
          merge_chain_decompositions(vertices, node_pool, chain_decompositions[0], chain_decompositions[1]);
      REQUIRE(validate_chain_decomposition(vertices, branches));

      branches = merge_chain_decompositions(vertices, node_pool, branches, chain_decompositions[2]);
      REQUIRE(validate_chain_decomposition(vertices, branches));

      SECTION("Chain A enters branch")
      {
        ChainDecomposition merged = merge_chain_decompositions(vertices, node_pool, chain_decompositions[5], branches);
        CHECK(validate_chain_decomposition(vertices, merged));
      }

      SECTION("Chain B enters branch")
      {
        ChainDecomposition merged = merge_chain_decompositions(vertices, node_pool, branches, chain_decompositions[3]);
        CHECK(validate_chain_decomposition(vertices, merged));
      }
    }

    SECTION("Towards right, enter upper branch")
    {
      Polygon2 polygon{
          {-3.58, 2.24}, {-2.22, 1.34}, {0.84, 0.98},  {3.46, 2.26},  {1.16, 3.64},  {-0.32, 2.94},
          {-1.46, 3.66}, {-0.50, 4.64}, {1.12, 4.20},  {3.14, 5.74},  {-0.20, 7.54}, {-2.94, 8.16},
          {-4.82, 8.12}, {-6.16, 7.16}, {-5.14, 5.98}, {-2.98, 5.48}, {-1.14, 6.14}, {-0.14, 5.98},
          {-0.42, 6.48}, {0.46, 5.56},  {-0.56, 5.30}, {-0.24, 5.62}, {-1.64, 5.44}, {-3.28, 4.20},
      };

      VerticesView vertices(polygon);
      NodePool node_pool;

      std::vector<ChainDecomposition> chain_decompositions = initial_chain_decompositions(vertices, node_pool);
      REQUIRE(chain_decompositions.size() == 6);

      ChainDecomposition branches =
          merge_chain_decompositions(vertices, node_pool, chain_decompositions[0], chain_decompositions[1]);
      REQUIRE(validate_chain_decomposition(vertices, branches));

      branches = merge_chain_decompositions(vertices, node_pool, branches, chain_decompositions[2]);
      REQUIRE(validate_chain_decomposition(vertices, branches));

      SECTION("Chain A enters branch")
      {
        ChainDecomposition merged = merge_chain_decompositions(vertices, node_pool, chain_decompositions[5], branches);
        CHECK(validate_chain_decomposition(vertices, merged));
      }

      SECTION("Chain B enters branch")
      {
        ChainDecomposition merged = merge_chain_decompositions(vertices, node_pool, branches, chain_decompositions[3]);
        CHECK(validate_chain_decomposition(vertices, merged));
      }
    }

    SECTION("Towards left, enter lower branch")
    {
      Polygon2 polygon{
          {-5.48, 3.04}, {-3.72, 0.38}, {-0.92, 2.02}, {2.60, 0.24},  {0.82, 3.48},  {-1.78, 2.60},
          {-3.14, 2.74}, {-2.96, 2.34}, {-3.90, 3.02}, {-2.86, 3.54}, {-3.20, 3.14}, {-1.76, 3.06},
          {0.70, 4.38},  {-1.30, 5.96}, {-4.36, 5.22}, {-2.00, 4.48},
      };

      VerticesView vertices(polygon);
      NodePool node_pool;

      std::vector<ChainDecomposition> chain_decompositions = initial_chain_decompositions(vertices, node_pool);
      REQUIRE(chain_decompositions.size() == 6);

      ChainDecomposition branches =
          merge_chain_decompositions(vertices, node_pool, chain_decompositions[4], chain_decompositions[5]);
      REQUIRE(validate_chain_decomposition(vertices, branches));

      branches = merge_chain_decompositions(vertices, node_pool, branches, chain_decompositions[0]);
      REQUIRE(validate_chain_decomposition(vertices, branches));

      SECTION("Chain A enters branch")
      {
        ChainDecomposition merged = merge_chain_decompositions(vertices, node_pool, chain_decompositions[3], branches);
        CHECK(validate_chain_decomposition(vertices, merged));
      }

      SECTION("Chain B enters branch")
      {
        ChainDecomposition merged = merge_chain_decompositions(vertices, node_pool, branches, chain_decompositions[1]);
        CHECK(validate_chain_decomposition(vertices, merged));
      }
    }

    SECTION("Towards left, enter upper branch")
    {
      Polygon2 polygon{
          {-4.94, 2.26}, {-3.36, 0.94}, {-0.78, 1.00}, {4.54, 2.32},  {4.24, 3.86},  {2.44, 5.32}, {0.08, 6.10},
          {-1.02, 6.04}, {-0.36, 5.36}, {-2.16, 6.22}, {-0.52, 7.58}, {-0.94, 6.98}, {0.86, 7.06}, {3.08, 7.34},
          {4.00, 8.20},  {-0.44, 9.14}, {-3.68, 8.34}, {-5.36, 6.34}, {-3.36, 4.78}, {0.40, 3.58}, {-1.64, 1.96},
      };

      VerticesView vertices(polygon);
      NodePool node_pool;

      std::vector<ChainDecomposition> chain_decompositions = initial_chain_decompositions(vertices, node_pool);
      REQUIRE(chain_decompositions.size() == 6);

      ChainDecomposition branches =
          merge_chain_decompositions(vertices, node_pool, chain_decompositions[4], chain_decompositions[5]);
      REQUIRE(validate_chain_decomposition(vertices, branches));

      branches = merge_chain_decompositions(vertices, node_pool, branches, chain_decompositions[0]);
      REQUIRE(validate_chain_decomposition(vertices, branches));

      SECTION("Chain A enters branch")
      {
        ChainDecomposition merged = merge_chain_decompositions(vertices, node_pool, chain_decompositions[3], branches);
        CHECK(validate_chain_decomposition(vertices, merged));
      }

      SECTION("Chain B enters branch")
      {
        ChainDecomposition merged = merge_chain_decompositions(vertices, node_pool, branches, chain_decompositions[1]);
        CHECK(validate_chain_decomposition(vertices, merged));
      }
    }
  }

  SECTION("Move past reverse branch")
  {
    SECTION("Towards right, from lower branch")
    {
      Polygon2 polygon{
          {-4.60, 3.22}, {-2.78, 2.12}, {0.82, 2.30},  {3.72, 4.04},  {3.50, 4.30},  {4.38, 4.30},  {3.68, 3.48},
          {3.78, 3.88},  {2.00, 2.24},  {0.46, 1.58},  {-1.38, 1.70}, {-4.16, 0.82}, {-0.96, 0.04}, {3.64, 2.28},
          {5.38, 4.62},  {1.88, 6.94},  {-0.80, 5.50}, {1.40, 5.94},  {2.70, 4.12},  {0.20, 3.38},  {-1.52, 4.30},
      };

      VerticesView vertices(polygon);
      NodePool node_pool;

      std::vector<ChainDecomposition> chain_decompositions = initial_chain_decompositions(vertices, node_pool);
      REQUIRE(chain_decompositions.size() == 6);

      ChainDecomposition branches =
          merge_chain_decompositions(vertices, node_pool, chain_decompositions[4], chain_decompositions[5]);
      REQUIRE(validate_chain_decomposition(vertices, branches));

      branches = merge_chain_decompositions(vertices, node_pool, chain_decompositions[3], branches);
      REQUIRE(validate_chain_decomposition(vertices, branches));

      SECTION("Branch vertex visible from other chain")
      {
        ChainDecomposition merged = merge_chain_decompositions(vertices, node_pool, branches, chain_decompositions[0]);
        CHECK(validate_chain_decomposition(vertices, merged));
      }

      SECTION("Branch vertex not visible from other chain")
      {
        ChainDecomposition merged = merge_chain_decompositions(vertices, node_pool, chain_decompositions[2], branches);
        CHECK(validate_chain_decomposition(vertices, merged));
      }
    }

    SECTION("Towards right, from upper branch")
    {
      Polygon2 polygon{
          {-5.40, 3.58}, {-3.64, 2.44}, {-1.92, 3.60}, {-0.08, 3.92}, {1.40, 2.32},  {-0.68, 0.54}, {-2.24, 0.70},
          {1.12, -0.18}, {4.10, 1.60},  {2.64, 4.84},  {0.98, 7.60},  {-1.62, 8.38}, {-4.38, 6.86}, {-2.06, 7.10},
          {0.64, 5.20},  {3.00, 2.10},  {2.90, 2.74},  {3.22, 1.70},  {2.40, 1.88},  {2.62, 1.98},  {0.32, 4.76},
          {-1.14, 5.20}, {-2.50, 5.62}, {-3.26, 4.50}, {-4.70, 4.74},
      };

      VerticesView vertices(polygon);
      NodePool node_pool;

      std::vector<ChainDecomposition> chain_decompositions = initial_chain_decompositions(vertices, node_pool);
      REQUIRE(chain_decompositions.size() == 6);

      ChainDecomposition branches =
          merge_chain_decompositions(vertices, node_pool, chain_decompositions[0], chain_decompositions[1]);
      REQUIRE(validate_chain_decomposition(vertices, branches));

      branches = merge_chain_decompositions(vertices, node_pool, branches, chain_decompositions[2]);
      REQUIRE(validate_chain_decomposition(vertices, branches));

      SECTION("Branch vertex visible from other chain")
      {
        ChainDecomposition merged = merge_chain_decompositions(vertices, node_pool, chain_decompositions[5], branches);
        CHECK(validate_chain_decomposition(vertices, merged));
      }

      SECTION("Branch vertex not visible from other chain")
      {
        ChainDecomposition merged = merge_chain_decompositions(vertices, node_pool, branches, chain_decompositions[3]);
        CHECK(validate_chain_decomposition(vertices, merged));
      }
    }

    SECTION("Towards left, from lower branch")
    {
      Polygon2 polygon{
          {-5.10, 3.24}, {-3.20, 3.24}, {-1.70, 3.14}, {0.16, 1.92},  {0.82, 1.38},  {1.58, 0.64},  {2.02, -0.40},
          {2.04, 1.32},  {1.38, 2.60},  {0.30, 3.28},  {-1.00, 3.84}, {-2.38, 4.54}, {-2.12, 3.90}, {-3.34, 4.50},
          {-2.00, 5.14}, {-2.36, 4.82}, {-0.16, 4.46}, {0.96, 3.98},  {2.20, 3.02},  {1.38, 4.40},  {0.12, 5.16},
          {-0.92, 5.28}, {0.30, 6.12},  {1.66, 5.78},  {0.18, 7.16},  {-1.56, 7.30}, {-2.66, 6.42}, {-3.00, 5.46},
          {-3.88, 4.94}, {-4.40, 4.94}, {-4.90, 4.26},
      };

      VerticesView vertices(polygon);
      NodePool node_pool;

      std::vector<ChainDecomposition> chain_decompositions = initial_chain_decompositions(vertices, node_pool);
      REQUIRE(chain_decompositions.size() == 6);

      ChainDecomposition branches =
          merge_chain_decompositions(vertices, node_pool, chain_decompositions[4], chain_decompositions[5]);
      REQUIRE(validate_chain_decomposition(vertices, branches));

      branches = merge_chain_decompositions(vertices, node_pool, branches, chain_decompositions[0]);
      REQUIRE(validate_chain_decomposition(vertices, branches));

      SECTION("Branch vertex visible from other chain")
      {
        ChainDecomposition merged = merge_chain_decompositions(vertices, node_pool, chain_decompositions[3], branches);
        CHECK(validate_chain_decomposition(vertices, merged));
      }

      SECTION("Branch vertex not visible from other chain")
      {
        ChainDecomposition merged = merge_chain_decompositions(vertices, node_pool, branches, chain_decompositions[1]);
        CHECK(validate_chain_decomposition(vertices, merged));
      }
    }

    SECTION("Towards left, from upper branch")
    {
      Polygon2 polygon{
          {-2.90, 4.36}, {-1.44, 3.32}, {-0.54, 1.08}, {-0.08, 3.32}, {-0.90, 4.76},
          {2.00, 5.54},  {3.88, 4.86},  {2.06, 6.42},  {-1.90, 4.76}, {-1.74, 4.36},
          {-2.24, 4.76}, {-1.84, 5.42}, {-1.92, 5.14}, {2.02, 6.98},  {-2.76, 7.34},
      };

      VerticesView vertices(polygon);
      NodePool node_pool;

      std::vector<ChainDecomposition> chain_decompositions = initial_chain_decompositions(vertices, node_pool);
      REQUIRE(chain_decompositions.size() == 6);

      ChainDecomposition branches =
          merge_chain_decompositions(vertices, node_pool, chain_decompositions[0], chain_decompositions[1]);
      REQUIRE(validate_chain_decomposition(vertices, branches));

      branches = merge_chain_decompositions(vertices, node_pool, chain_decompositions[5], branches);
      REQUIRE(validate_chain_decomposition(vertices, branches));

      SECTION("Branch vertex visible from other chain")
      {
        ChainDecomposition merged = merge_chain_decompositions(vertices, node_pool, branches, chain_decompositions[2]);
        CHECK(validate_chain_decomposition(vertices, merged));
      }

      SECTION("Branch vertex not visible from other chain")
      {
        ChainDecomposition merged = merge_chain_decompositions(vertices, node_pool, chain_decompositions[4], branches);
        CHECK(validate_chain_decomposition(vertices, merged));
      }
    }
  }

  SECTION("Difficult")
  {
    Polygon2 polygon{
        {0.86, 3.36},  {3.62, 3.90},  {3.54, 6.40},  {0.34, 7.42}, {-2.70, 6.06}, {-3.70, 4.10}, {-3.38, 1.88},
        {-0.48, 0.26}, {3.88, 0.48},  {6.20, 1.32},  {7.32, 2.98}, {7.64, 4.94},  {6.32, 7.58},  {3.46, 8.72},
        {0.46, 9.00},  {-1.74, 8.62}, {0.46, 8.52},  {3.32, 8.06}, {6.02, 6.20},  {6.38, 4.74},  {5.56, 2.50},
        {3.76, 1.94},  {1.06, 1.98},  {-0.48, 3.32}, {0.34, 4.88}, {1.60, 4.32},
    };

    VerticesView vertices(polygon);
    NodePool node_pool;

    std::vector<ChainDecomposition> chain_decompositions = initial_chain_decompositions(vertices, node_pool);
    REQUIRE(chain_decompositions.size() == 5);

    ChainDecomposition inner_chain =
        merge_chain_decompositions(vertices, node_pool, chain_decompositions[0], chain_decompositions[1]);
    REQUIRE(validate_chain_decomposition(vertices, inner_chain));

    ChainDecomposition outer_chain =
        merge_chain_decompositions(vertices, node_pool, chain_decompositions[2], chain_decompositions[3]);
    REQUIRE(validate_chain_decomposition(vertices, outer_chain));

    ChainDecomposition merged = merge_chain_decompositions(vertices, node_pool, inner_chain, outer_chain);
    // CHECK(validate_chain_decomposition(vertices, outer_chain));
  }

  SECTION("Difficult 2")
  {
    Polygon2 polygon{
        {-1.86, 3.26}, {-0.46, 2.74}, {0.98, 3.10},  {2.42, 4.20},   {2.34, 5.82},  {1.00, 6.68},  {1.16, 6.90},
        {0.76, 7.28},  {1.36, 7.42},  {2.48, 6.76},  {3.14, 5.70},   {3.60, 4.52},  {3.26, 3.18},  {2.62, 1.64},
        {0.26, 0.76},  {-1.36, 0.94}, {-1.66, 0.08}, {-0.20, -0.04}, {1.44, 0.18},  {2.90, 0.62},  {3.92, 2.00},
        {4.34, 3.82},  {3.90, 6.00},  {2.90, 7.34},  {1.60, 8.04},   {-0.38, 7.90}, {-1.08, 6.80}, {-0.02, 6.00},
        {1.32, 5.48},  {0.78, 4.30},  {-0.30, 3.60},
    };

    VerticesView vertices(polygon);
    NodePool node_pool;

    std::vector<ChainDecomposition> chain_decompositions = initial_chain_decompositions(vertices, node_pool);
    REQUIRE(chain_decompositions.size() == 6);

    ChainDecomposition channel =
        merge_chain_decompositions(vertices, node_pool, chain_decompositions[3], chain_decompositions[4]);
    REQUIRE(validate_chain_decomposition(vertices, channel));

    channel = merge_chain_decompositions(vertices, node_pool, channel, chain_decompositions[5]);
    REQUIRE(validate_chain_decomposition(vertices, channel));

    ChainDecomposition entering_chain =
        merge_chain_decompositions(vertices, node_pool, chain_decompositions[0], chain_decompositions[1]);
    REQUIRE(validate_chain_decomposition(vertices, entering_chain));

    ChainDecomposition merged = merge_chain_decompositions(vertices, node_pool, channel, entering_chain);
    //CHECK(validate_chain_decomposition(vertices, merged));
  }

  /*SECTION("Bug")
  {
    Polygon2 polygon{
        {-1.86, 3.26}, {-0.46, 2.74}, {0.98, 3.10},  {2.42, 4.20},   {2.34, 5.82},  {1.00, 6.68},  {1.16, 6.90},
        {0.76, 7.28},  {1.36, 7.42},  {2.48, 6.76},  {3.14, 5.70},   {3.60, 4.52},  {3.26, 3.18},  {2.62, 1.64},
        {0.26, 0.76},  {-1.36, 0.94}, {-1.66, 0.08}, {-0.20, -0.04}, {1.44, 0.18},  {2.90, 0.62},  {3.92, 2.00},
        {4.34, 3.82},  {3.90, 6.00},  {2.90, 7.34},  {1.60, 8.04},   {-0.38, 7.90}, {-1.08, 6.80}, {-0.02, 6.00},
        {1.32, 5.48},  {0.78, 4.30},  {-0.30, 3.60},
    };

    VerticesView vertices(polygon);
    NodePool node_pool;

    std::vector<ChainDecomposition> chain_decompositions = initial_chain_decompositions(vertices, node_pool);
    REQUIRE(chain_decompositions.size() == 6);

    ChainDecomposition channel = merge_chain_decompositions(vertices, chain_decompositions[0], chain_decompositions[1]);
    REQUIRE(validate_chain_decomposition(vertices, channel));

    channel = merge_chain_decompositions(vertices, channel, chain_decompositions[2]);
    REQUIRE(validate_chain_decomposition(vertices, channel));

    ChainDecomposition entering_chain = merge_chain_decompositions(vertices, chain_decompositions[3],
  chain_decompositions[4]); REQUIRE(validate_chain_decomposition(vertices, entering_chain));

    std::cout << "Start" << std::endl;
    ChainDecomposition merged = merge_chain_decompositions(vertices, channel, entering_chain);
    CHECK(validate_chain_decomposition(vertices, channel));
  }*/
}

} // namespace dida::detail::vertical_decomposition