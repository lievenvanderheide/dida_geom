#include "dida/detail/vertical_decomposition/merge.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_all.hpp>

#include "dida/detail/vertical_decomposition/tests/test_utils.hpp"
#include "dida/polygon2.hpp"

namespace dida::detail::vertical_decomposition
{

TEST_CASE("vertical_decomposition_merge")
{
  SECTION("Clams")
  {
    std::vector<Point2> vertices_storage{
        {0.66, 2.30}, {0.74, 1.12}, {2.54, 0.18}, {6.84, -0.48}, {9.94, 0.92}, {8.22, 2.08}, {6.76, 2.10},
        {7.16, 1.58}, {5.84, 2.44}, {7.22, 3.34}, {6.84, 2.80},  {7.78, 3.02}, {8.66, 3.78}, {7.20, 4.62},
        {4.82, 3.80}, {3.58, 4.30}, {1.56, 4.78}, {0.00, 3.98},  {1.82, 2.62}, {3.44, 2.44}, {3.06, 3.16},
        {4.36, 2.46}, {2.80, 1.18}, {3.28, 1.98}, {2.20, 1.30},
    };

    Winding winding = GENERATE(Winding::ccw, Winding::cw);
    if (winding == Winding::cw)
    {
      flip_horizontally(vertices_storage);
    }

    VerticesView vertices(vertices_storage);
    NodePool node_pool;

    std::vector<ChainDecomposition> chain_decompositions = initial_chain_decompositions(vertices, winding, node_pool);

    SECTION("Right facing, lower chain longer")
    {
      ChainDecomposition merged =
          merge_chain_decompositions(vertices, winding, node_pool, chain_decompositions[7], chain_decompositions[0]);
      CHECK(validate_chain_decomposition(vertices, winding, merged));
    }

    SECTION("Right facing, upper chain longer")
    {
      ChainDecomposition merged =
          merge_chain_decompositions(vertices, winding, node_pool, chain_decompositions[4], chain_decompositions[5]);
      CHECK(validate_chain_decomposition(vertices, winding, merged));
    }

    SECTION("Left facing, lower chain longer")
    {
      ChainDecomposition merged =
          merge_chain_decompositions(vertices, winding, node_pool, chain_decompositions[0], chain_decompositions[1]);
      CHECK(validate_chain_decomposition(vertices, winding, merged));
    }

    SECTION("Left facing, upper chain longer")
    {
      ChainDecomposition merged =
          merge_chain_decompositions(vertices, winding, node_pool, chain_decompositions[3], chain_decompositions[4]);
      CHECK(validate_chain_decomposition(vertices, winding, merged));
    }
  }

  SECTION("Enter forward branch")
  {
    SECTION("Towards right, enter lower branch")
    {
      std::vector<Point2> vertices_storage{
          {-5.16, 2.56}, {-3.14, 2.32}, {1.12, 3.28},  {3.70, 3.06},  {2.04, 5.02},  {-0.28, 5.14}, {1.40, 6.20},
          {3.92, 6.40},  {0.80, 7.74},  {-2.66, 5.58}, {-2.14, 4.52}, {0.88, 4.34},  {1.38, 4.30},  {1.20, 4.54},
          {2.04, 4.18},  {1.26, 3.98},  {1.40, 4.16},  {-1.58, 3.32}, {-3.26, 3.62},
      };

      Winding winding = GENERATE(Winding::ccw, Winding::cw);
      if (winding == Winding::cw)
      {
        flip_horizontally(vertices_storage);
      }

      VerticesView vertices(vertices_storage);
      NodePool node_pool;

      std::vector<ChainDecomposition> chain_decompositions = initial_chain_decompositions(vertices, winding, node_pool);
      REQUIRE(chain_decompositions.size() == 6);

      ChainDecomposition branches =
          merge_chain_decompositions(vertices, winding, node_pool, chain_decompositions[0], chain_decompositions[1]);
      REQUIRE(validate_chain_decomposition(vertices, winding, branches));

      branches = merge_chain_decompositions(vertices, winding, node_pool, branches, chain_decompositions[2]);
      REQUIRE(validate_chain_decomposition(vertices, winding, branches));

      SECTION("Chain A enters branch")
      {
        ChainDecomposition merged =
            merge_chain_decompositions(vertices, winding, node_pool, chain_decompositions[5], branches);
        CHECK(validate_chain_decomposition(vertices, winding, merged));
      }

      SECTION("Chain B enters branch")
      {
        ChainDecomposition merged =
            merge_chain_decompositions(vertices, winding, node_pool, branches, chain_decompositions[3]);
        CHECK(validate_chain_decomposition(vertices, winding, merged));
      }
    }

    SECTION("Towards right, enter upper branch")
    {
      std::vector<Point2> vertices_storage{
          {-3.58, 2.24}, {-2.22, 1.34}, {0.84, 0.98},  {3.46, 2.26},  {1.16, 3.64},  {-0.32, 2.94},
          {-1.46, 3.66}, {-0.50, 4.64}, {1.12, 4.20},  {3.14, 5.74},  {-0.20, 7.54}, {-2.94, 8.16},
          {-4.82, 8.12}, {-6.16, 7.16}, {-5.14, 5.98}, {-2.98, 5.48}, {-1.14, 6.14}, {-0.14, 5.98},
          {-0.42, 6.48}, {0.46, 5.56},  {-0.56, 5.30}, {-0.24, 5.62}, {-1.64, 5.44}, {-3.28, 4.20},
      };

      Winding winding = GENERATE(Winding::ccw, Winding::cw);
      if (winding == Winding::cw)
      {
        flip_horizontally(vertices_storage);
      }

      VerticesView vertices(vertices_storage);
      NodePool node_pool;

      std::vector<ChainDecomposition> chain_decompositions = initial_chain_decompositions(vertices, winding, node_pool);
      REQUIRE(chain_decompositions.size() == 6);

      ChainDecomposition branches =
          merge_chain_decompositions(vertices, winding, node_pool, chain_decompositions[0], chain_decompositions[1]);
      REQUIRE(validate_chain_decomposition(vertices, winding, branches));

      branches = merge_chain_decompositions(vertices, winding, node_pool, branches, chain_decompositions[2]);
      REQUIRE(validate_chain_decomposition(vertices, winding, branches));

      SECTION("Chain A enters branch")
      {
        ChainDecomposition merged =
            merge_chain_decompositions(vertices, winding, node_pool, chain_decompositions[5], branches);
        CHECK(validate_chain_decomposition(vertices, winding, merged));
      }

      SECTION("Chain B enters branch")
      {
        ChainDecomposition merged =
            merge_chain_decompositions(vertices, winding, node_pool, branches, chain_decompositions[3]);
        CHECK(validate_chain_decomposition(vertices, winding, merged));
      }
    }

    SECTION("Towards left, enter lower branch")
    {
      std::vector<Point2> vertices_storage{
          {-5.48, 3.04}, {-3.72, 0.38}, {-0.92, 2.02}, {2.60, 0.24},  {0.82, 3.48},  {-1.78, 2.60},
          {-3.14, 2.74}, {-2.96, 2.34}, {-3.90, 3.02}, {-2.86, 3.54}, {-3.20, 3.14}, {-1.76, 3.06},
          {0.70, 4.38},  {-1.30, 5.96}, {-4.36, 5.22}, {-2.00, 4.48},
      };

      Winding winding = GENERATE(Winding::ccw, Winding::cw);
      if (winding == Winding::cw)
      {
        flip_horizontally(vertices_storage);
      }

      VerticesView vertices(vertices_storage);
      NodePool node_pool;

      std::vector<ChainDecomposition> chain_decompositions = initial_chain_decompositions(vertices, winding, node_pool);
      REQUIRE(chain_decompositions.size() == 6);

      ChainDecomposition branches =
          merge_chain_decompositions(vertices, winding, node_pool, chain_decompositions[4], chain_decompositions[5]);
      REQUIRE(validate_chain_decomposition(vertices, winding, branches));

      branches = merge_chain_decompositions(vertices, winding, node_pool, branches, chain_decompositions[0]);
      REQUIRE(validate_chain_decomposition(vertices, winding, branches));

      SECTION("Chain A enters branch")
      {
        ChainDecomposition merged =
            merge_chain_decompositions(vertices, winding, node_pool, chain_decompositions[3], branches);
        CHECK(validate_chain_decomposition(vertices, winding, merged));
      }

      SECTION("Chain B enters branch")
      {
        ChainDecomposition merged =
            merge_chain_decompositions(vertices, winding, node_pool, branches, chain_decompositions[1]);
        CHECK(validate_chain_decomposition(vertices, winding, merged));
      }
    }

    SECTION("Towards left, enter upper branch")
    {
      std::vector<Point2> vertices_storage{
          {-4.94, 2.26}, {-3.36, 0.94}, {-0.78, 1.00}, {4.54, 2.32},  {4.24, 3.86},  {2.44, 5.32}, {0.08, 6.10},
          {-1.02, 6.04}, {-0.36, 5.36}, {-2.16, 6.22}, {-0.52, 7.58}, {-0.94, 6.98}, {0.86, 7.06}, {3.08, 7.34},
          {4.00, 8.20},  {-0.44, 9.14}, {-3.68, 8.34}, {-5.36, 6.34}, {-3.36, 4.78}, {0.40, 3.58}, {-1.64, 1.96},
      };

      Winding winding = GENERATE(Winding::ccw, Winding::cw);
      if (winding == Winding::cw)
      {
        flip_horizontally(vertices_storage);
      }

      VerticesView vertices(vertices_storage);
      NodePool node_pool;

      std::vector<ChainDecomposition> chain_decompositions = initial_chain_decompositions(vertices, winding, node_pool);
      REQUIRE(chain_decompositions.size() == 6);

      ChainDecomposition branches =
          merge_chain_decompositions(vertices, winding, node_pool, chain_decompositions[4], chain_decompositions[5]);
      REQUIRE(validate_chain_decomposition(vertices, winding, branches));

      branches = merge_chain_decompositions(vertices, winding, node_pool, branches, chain_decompositions[0]);
      REQUIRE(validate_chain_decomposition(vertices, winding, branches));

      SECTION("Chain A enters branch")
      {
        ChainDecomposition merged =
            merge_chain_decompositions(vertices, winding, node_pool, chain_decompositions[3], branches);
        CHECK(validate_chain_decomposition(vertices, winding, merged));
      }

      SECTION("Chain B enters branch")
      {
        ChainDecomposition merged =
            merge_chain_decompositions(vertices, winding, node_pool, branches, chain_decompositions[1]);
        CHECK(validate_chain_decomposition(vertices, winding, merged));
      }
    }
  }

  SECTION("Move past reverse branch")
  {
    SECTION("Towards right, from lower branch")
    {
      std::vector<Point2> vertices_storage{
          {-4.60, 3.22}, {-2.78, 2.12}, {0.82, 2.30},  {3.72, 4.04},  {3.50, 4.30},  {4.38, 4.30},  {3.68, 3.48},
          {3.78, 3.88},  {2.00, 2.24},  {0.46, 1.58},  {-1.38, 1.70}, {-4.16, 0.82}, {-0.96, 0.04}, {3.64, 2.28},
          {5.38, 4.62},  {1.88, 6.94},  {-0.80, 5.50}, {1.40, 5.94},  {2.70, 4.12},  {0.20, 3.38},  {-1.52, 4.30},
      };

      Winding winding = GENERATE(Winding::ccw, Winding::cw);
      if (winding == Winding::cw)
      {
        flip_horizontally(vertices_storage);
      }

      VerticesView vertices(vertices_storage);
      NodePool node_pool;

      std::vector<ChainDecomposition> chain_decompositions = initial_chain_decompositions(vertices, winding, node_pool);
      REQUIRE(chain_decompositions.size() == 6);

      ChainDecomposition branches =
          merge_chain_decompositions(vertices, winding, node_pool, chain_decompositions[4], chain_decompositions[5]);
      REQUIRE(validate_chain_decomposition(vertices, winding, branches));

      branches = merge_chain_decompositions(vertices, winding, node_pool, chain_decompositions[3], branches);
      REQUIRE(validate_chain_decomposition(vertices, winding, branches));

      SECTION("Branch vertex visible from other chain")
      {
        ChainDecomposition merged =
            merge_chain_decompositions(vertices, winding, node_pool, branches, chain_decompositions[0]);
        CHECK(validate_chain_decomposition(vertices, winding, merged));
      }

      SECTION("Branch vertex not visible from other chain")
      {
        ChainDecomposition merged =
            merge_chain_decompositions(vertices, winding, node_pool, chain_decompositions[2], branches);
        CHECK(validate_chain_decomposition(vertices, winding, merged));
      }
    }

    SECTION("Towards right, from upper branch")
    {
      std::vector<Point2> vertices_storage{
          {-5.40, 3.58}, {-3.64, 2.44}, {-1.92, 3.60}, {-0.08, 3.92}, {1.40, 2.32},  {-0.68, 0.54}, {-2.24, 0.70},
          {1.12, -0.18}, {4.10, 1.60},  {2.64, 4.84},  {0.98, 7.60},  {-1.62, 8.38}, {-4.38, 6.86}, {-2.06, 7.10},
          {0.64, 5.20},  {3.00, 2.10},  {2.90, 2.74},  {3.22, 1.70},  {2.40, 1.88},  {2.62, 1.98},  {0.32, 4.76},
          {-1.14, 5.20}, {-2.50, 5.62}, {-3.26, 4.50}, {-4.70, 4.74},
      };

      Winding winding = GENERATE(Winding::ccw, Winding::cw);
      if (winding == Winding::cw)
      {
        flip_horizontally(vertices_storage);
      }

      VerticesView vertices(vertices_storage);
      NodePool node_pool;

      std::vector<ChainDecomposition> chain_decompositions = initial_chain_decompositions(vertices, winding, node_pool);
      REQUIRE(chain_decompositions.size() == 6);

      ChainDecomposition branches =
          merge_chain_decompositions(vertices, winding, node_pool, chain_decompositions[0], chain_decompositions[1]);
      REQUIRE(validate_chain_decomposition(vertices, winding, branches));

      branches = merge_chain_decompositions(vertices, winding, node_pool, branches, chain_decompositions[2]);
      REQUIRE(validate_chain_decomposition(vertices, winding, branches));

      SECTION("Branch vertex visible from other chain")
      {
        ChainDecomposition merged =
            merge_chain_decompositions(vertices, winding, node_pool, chain_decompositions[5], branches);
        CHECK(validate_chain_decomposition(vertices, winding, merged));
      }

      SECTION("Branch vertex not visible from other chain")
      {
        ChainDecomposition merged =
            merge_chain_decompositions(vertices, winding, node_pool, branches, chain_decompositions[3]);
        CHECK(validate_chain_decomposition(vertices, winding, merged));
      }
    }

    SECTION("Towards left, from lower branch")
    {
      std::vector<Point2> vertices_storage{
          {-5.10, 3.24}, {-3.20, 3.24}, {-1.70, 3.14}, {0.16, 1.92},  {0.82, 1.38},  {1.58, 0.64},  {2.02, -0.40},
          {2.04, 1.32},  {1.38, 2.60},  {0.30, 3.28},  {-1.00, 3.84}, {-2.38, 4.54}, {-2.12, 3.90}, {-3.34, 4.50},
          {-2.00, 5.14}, {-2.36, 4.82}, {-0.16, 4.46}, {0.96, 3.98},  {2.20, 3.02},  {1.38, 4.40},  {0.12, 5.16},
          {-0.92, 5.28}, {0.30, 6.12},  {1.66, 5.78},  {0.18, 7.16},  {-1.56, 7.30}, {-2.66, 6.42}, {-3.00, 5.46},
          {-3.88, 4.94}, {-4.40, 4.94}, {-4.90, 4.26},
      };

      Winding winding = GENERATE(Winding::ccw, Winding::cw);
      if (winding == Winding::cw)
      {
        flip_horizontally(vertices_storage);
      }

      VerticesView vertices(vertices_storage);
      NodePool node_pool;

      std::vector<ChainDecomposition> chain_decompositions = initial_chain_decompositions(vertices, winding, node_pool);
      REQUIRE(chain_decompositions.size() == 6);

      ChainDecomposition branches =
          merge_chain_decompositions(vertices, winding, node_pool, chain_decompositions[4], chain_decompositions[5]);
      REQUIRE(validate_chain_decomposition(vertices, winding, branches));

      branches = merge_chain_decompositions(vertices, winding, node_pool, branches, chain_decompositions[0]);
      REQUIRE(validate_chain_decomposition(vertices, winding, branches));

      SECTION("Branch vertex visible from other chain")
      {
        ChainDecomposition merged =
            merge_chain_decompositions(vertices, winding, node_pool, chain_decompositions[3], branches);
        CHECK(validate_chain_decomposition(vertices, winding, merged));
      }

      SECTION("Branch vertex not visible from other chain")
      {
        ChainDecomposition merged =
            merge_chain_decompositions(vertices, winding, node_pool, branches, chain_decompositions[1]);
        CHECK(validate_chain_decomposition(vertices, winding, merged));
      }
    }

    SECTION("Towards left, from upper branch")
    {
      std::vector<Point2> vertices_storage{
          {-2.90, 4.36}, {-1.44, 3.32}, {-0.54, 1.08}, {-0.08, 3.32}, {-0.90, 4.76},
          {2.00, 5.54},  {3.88, 4.86},  {2.06, 6.42},  {-1.90, 4.76}, {-1.74, 4.36},
          {-2.24, 4.76}, {-1.84, 5.42}, {-1.92, 5.14}, {2.02, 6.98},  {-2.76, 7.34},
      };

      Winding winding = GENERATE(Winding::ccw, Winding::cw);
      if (winding == Winding::cw)
      {
        flip_horizontally(vertices_storage);
      }

      VerticesView vertices(vertices_storage);
      NodePool node_pool;

      std::vector<ChainDecomposition> chain_decompositions = initial_chain_decompositions(vertices, winding, node_pool);
      REQUIRE(chain_decompositions.size() == 6);

      ChainDecomposition branches =
          merge_chain_decompositions(vertices, winding, node_pool, chain_decompositions[0], chain_decompositions[1]);
      REQUIRE(validate_chain_decomposition(vertices, winding, branches));

      branches = merge_chain_decompositions(vertices, winding, node_pool, chain_decompositions[5], branches);
      REQUIRE(validate_chain_decomposition(vertices, winding, branches));

      SECTION("Branch vertex visible from other chain")
      {
        ChainDecomposition merged =
            merge_chain_decompositions(vertices, winding, node_pool, branches, chain_decompositions[2]);
        CHECK(validate_chain_decomposition(vertices, winding, merged));
      }

      SECTION("Branch vertex not visible from other chain")
      {
        ChainDecomposition merged =
            merge_chain_decompositions(vertices, winding, node_pool, chain_decompositions[4], branches);
        CHECK(validate_chain_decomposition(vertices, winding, merged));
      }
    }
  }

  SECTION("Outer branches")
  {
    SECTION("New outer branch in init_merge")
    {
      SECTION("Towards right, on chain A")
      {
        std::vector<Point2> vertices_storage{
            {-3.52, 4.34}, {-1.04, 3.78}, {-1.48, 4.48}, {0.40, 4.98},  {1.74, 1.32}, {-6.72, 1.60}, {-6.70, 7.60},
            {1.74, 7.48},  {1.78, 8.32},  {-7.38, 8.20}, {-7.12, 1.00}, {2.44, 1.04}, {1.06, 5.92},
        };

        Winding winding = GENERATE(Winding::ccw, Winding::cw);
        if (winding == Winding::cw)
        {
          flip_horizontally(vertices_storage);
        }

        VerticesView vertices(vertices_storage);
        NodePool node_pool;

        std::vector<ChainDecomposition> chain_decompositions =
            initial_chain_decompositions(vertices, winding, node_pool);
        REQUIRE(chain_decompositions.size() == 5);

        ChainDecomposition a = merge_chain_decompositions(
            vertices, winding, node_pool,
            merge_chain_decompositions(vertices, winding, node_pool, chain_decompositions[2], chain_decompositions[3]),
            chain_decompositions[4]);
        REQUIRE(validate_chain_decomposition(vertices, winding, a));

        ChainDecomposition b = chain_decompositions[0];

        ChainDecomposition merged = merge_chain_decompositions(vertices, winding, node_pool, a, b);
        CHECK(validate_chain_decomposition(vertices, winding, merged));
      }

      SECTION("Towards right, on chain B")
      {
        std::vector<Point2> vertices_storage{
            {-0.30, 6.54}, {-2.88, 5.62}, {0.36, 4.92}, {2.18, 8.74},  {-6.60, 5.92},
            {2.46, 1.86},  {-4.16, 5.88}, {0.20, 6.94}, {-0.62, 6.04},
        };

        Winding winding = GENERATE(Winding::ccw, Winding::cw);
        if (winding == Winding::cw)
        {
          flip_horizontally(vertices_storage);
        }

        VerticesView vertices(vertices_storage);
        NodePool node_pool;

        std::vector<ChainDecomposition> chain_decompositions =
            initial_chain_decompositions(vertices, winding, node_pool);
        REQUIRE(chain_decompositions.size() == 5);

        ChainDecomposition a = chain_decompositions[0];
        ChainDecomposition b = merge_chain_decompositions(
            vertices, winding, node_pool,
            merge_chain_decompositions(vertices, winding, node_pool, chain_decompositions[1], chain_decompositions[2]),
            chain_decompositions[3]);
        REQUIRE(validate_chain_decomposition(vertices, winding, b));

        ChainDecomposition merged = merge_chain_decompositions(vertices, winding, node_pool, a, b);
        CHECK(validate_chain_decomposition(vertices, winding, merged));
      }

      SECTION("Towards left, on chain A")
      {
        std::vector<Point2> vertices_storage{
            {-6.06, -1.76}, {4.78, 1.12}, {4.64, 6.04},  {-3.26, 5.96}, {-3.30, 2.34}, {2.34, 2.68},
            {0.34, 4.08},   {0.62, 3.42}, {-2.62, 3.10}, {-2.86, 5.38}, {4.14, 5.38},  {4.12, 1.44},
        };

        Winding winding = GENERATE(Winding::ccw, Winding::cw);
        if (winding == Winding::cw)
        {
          flip_horizontally(vertices_storage);
        }

        VerticesView vertices(vertices_storage);
        NodePool node_pool;

        std::vector<ChainDecomposition> chain_decompositions =
            initial_chain_decompositions(vertices, winding, node_pool);
        REQUIRE(chain_decompositions.size() == 5);

        ChainDecomposition a = merge_chain_decompositions(
            vertices, winding, node_pool,
            merge_chain_decompositions(vertices, winding, node_pool, chain_decompositions[0], chain_decompositions[1]),
            chain_decompositions[2]);
        REQUIRE(validate_chain_decomposition(vertices, winding, a));

        ChainDecomposition b = chain_decompositions[3];

        ChainDecomposition merged = merge_chain_decompositions(vertices, winding, node_pool, a, b);
        CHECK(validate_chain_decomposition(vertices, winding, merged));
      }

      SECTION("Towards left, on chain B")
      {
        std::vector<Point2> vertices_storage{
            {-1.70, 4.08}, {0.48, 5.42}, {-4.96, 5.54}, {-5.16, 0.72}, {4.12, 0.92},  {4.58, 9.18},
            {-5.30, 9.04}, {3.74, 8.22}, {3.46, 1.86},  {-4.10, 1.38}, {-3.92, 4.68}, {-1.48, 5.02},
        };

        Winding winding = GENERATE(Winding::ccw, Winding::cw);
        if (winding == Winding::cw)
        {
          flip_horizontally(vertices_storage);
        }

        VerticesView vertices(vertices_storage);
        NodePool node_pool;

        std::vector<ChainDecomposition> chain_decompositions =
            initial_chain_decompositions(vertices, winding, node_pool);
        REQUIRE(chain_decompositions.size() == 5);

        ChainDecomposition a = chain_decompositions[0];
        ChainDecomposition b = merge_chain_decompositions(
            vertices, winding, node_pool,
            merge_chain_decompositions(vertices, winding, node_pool, chain_decompositions[1], chain_decompositions[2]),
            chain_decompositions[3]);

        ChainDecomposition merged = merge_chain_decompositions(vertices, winding, node_pool, a, b);
        CHECK(validate_chain_decomposition(vertices, winding, merged));
      }
    }

    SECTION("New outer branch in merge_iteration_reverse_branch")
    {
      SECTION("Around leftward branch of chain A")
      {
        std::vector<Point2> vertices_storage{
            {-1.86, 3.26}, {-0.46, 2.74}, {0.98, 3.10},  {2.42, 4.20},   {2.34, 5.82},  {1.00, 6.68},  {1.16, 6.90},
            {0.76, 7.28},  {1.36, 7.42},  {2.48, 6.76},  {3.14, 5.70},   {3.60, 4.52},  {3.26, 3.18},  {2.62, 1.64},
            {0.26, 0.76},  {-1.36, 0.94}, {-1.66, 0.08}, {-0.20, -0.04}, {1.44, 0.18},  {2.90, 0.62},  {3.92, 2.00},
            {4.34, 3.82},  {3.90, 6.00},  {2.90, 7.34},  {1.60, 8.04},   {-0.38, 7.90}, {-1.08, 6.80}, {-0.02, 6.00},
            {1.32, 5.48},  {0.78, 4.30},  {-0.30, 3.60},
        };

        Winding winding = GENERATE(Winding::ccw, Winding::cw);
        if (winding == Winding::cw)
        {
          flip_horizontally(vertices_storage);
        }

        VerticesView vertices(vertices_storage);
        NodePool node_pool;

        std::vector<ChainDecomposition> chain_decompositions =
            initial_chain_decompositions(vertices, winding, node_pool);
        REQUIRE(chain_decompositions.size() == 6);

        ChainDecomposition a = merge_chain_decompositions(
            vertices, winding, node_pool,
            merge_chain_decompositions(vertices, winding, node_pool, chain_decompositions[3], chain_decompositions[4]),
            chain_decompositions[5]);
        REQUIRE(validate_chain_decomposition(vertices, winding, a));

        ChainDecomposition b =
            merge_chain_decompositions(vertices, winding, node_pool, chain_decompositions[0], chain_decompositions[1]);
        REQUIRE(validate_chain_decomposition(vertices, winding, b));

        ChainDecomposition merged = merge_chain_decompositions(vertices, winding, node_pool, a, b);
        CHECK(validate_chain_decomposition(vertices, winding, merged));
      }

      SECTION("Around leftward branch of chain B")
      {
        std::vector<Point2> vertices_storage{
            {-2.06, 4.80},  {0.22, 4.56},  {1.92, 3.52}, {0.42, 2.18},   {-1.56, 2.18}, {-2.64, 0.16},
            {-0.84, -1.20}, {2.66, -0.54}, {5.02, 1.06}, {5.38, 5.20},   {4.54, 7.90},  {0.18, 8.72},
            {-3.16, 8.88},  {-2.14, 7.32}, {1.30, 6.98}, {3.62, 5.36},   {3.78, 3.10},  {3.12, 1.30},
            {1.42, 0.80},   {-0.28, 0.52}, {0.06, 0.16}, {-0.30, -0.28}, {-1.16, 0.50}, {-0.34, 1.54},
            {1.46, 1.62},   {2.66, 2.86},  {2.80, 4.34}, {1.90, 5.56},   {-0.24, 5.88},
        };

        Winding winding = GENERATE(Winding::ccw, Winding::cw);
        if (winding == Winding::cw)
        {
          flip_horizontally(vertices_storage);
        }

        VerticesView vertices(vertices_storage);
        NodePool node_pool;

        std::vector<ChainDecomposition> chain_decompositions =
            initial_chain_decompositions(vertices, winding, node_pool);
        REQUIRE(chain_decompositions.size() == 6);

        ChainDecomposition a =
            merge_chain_decompositions(vertices, winding, node_pool, chain_decompositions[4], chain_decompositions[5]);

        ChainDecomposition b = merge_chain_decompositions(
            vertices, winding, node_pool,
            merge_chain_decompositions(vertices, winding, node_pool, chain_decompositions[0], chain_decompositions[1]),
            chain_decompositions[2]);
        REQUIRE(validate_chain_decomposition(vertices, winding, b));

        ChainDecomposition merged = merge_chain_decompositions(vertices, winding, node_pool, a, b);
        CHECK(validate_chain_decomposition(vertices, winding, merged));
      }

      SECTION("Around rightward branch of chain A")
      {
        std::vector<Point2> vertices_storage{
            {4.12, 6.12},  {0.84, 6.08},  {-0.44, 5.44}, {-0.86, 4.32}, {-0.60, 3.32}, {0.48, 2.26},
            {3.16, 1.72},  {2.42, 1.44},  {2.82, 0.96},  {1.34, 0.90},  {-0.54, 1.20}, {-1.80, 2.42},
            {-2.02, 3.86}, {-1.12, 5.86}, {1.60, 6.76},  {3.00, 7.24},  {2.86, 8.38},  {0.54, 8.42},
            {-1.44, 7.60}, {-3.22, 6.00}, {-3.62, 3.28}, {-2.50, 0.76}, {0.76, -0.42}, {4.42, 0.48},
            {4.76, 1.98},  {3.24, 2.80},  {1.02, 3.26},  {-0.22, 4.06}, {0.86, 5.36},
        };

        Winding winding = GENERATE(Winding::ccw, Winding::cw);
        if (winding == Winding::cw)
        {
          flip_horizontally(vertices_storage);
        }

        VerticesView vertices(vertices_storage);
        NodePool node_pool;

        std::vector<ChainDecomposition> chain_decompositions =
            initial_chain_decompositions(vertices, winding, node_pool);
        REQUIRE(chain_decompositions.size() == 6);

        ChainDecomposition a = merge_chain_decompositions(
            vertices, winding, node_pool,
            merge_chain_decompositions(vertices, winding, node_pool, chain_decompositions[3], chain_decompositions[4]),
            chain_decompositions[5]);
        REQUIRE(validate_chain_decomposition(vertices, winding, a));

        ChainDecomposition b =
            merge_chain_decompositions(vertices, winding, node_pool, chain_decompositions[0], chain_decompositions[1]);
        REQUIRE(validate_chain_decomposition(vertices, winding, b));

        ChainDecomposition merged = merge_chain_decompositions(vertices, winding, node_pool, a, b);
        CHECK(validate_chain_decomposition(vertices, winding, merged));
      }

      SECTION("Around rightward branch of chain B")
      {
        std::vector<Point2> vertices_storage{
            {1.26, 3.90},  {-1.86, 3.54}, {-3.64, 5.18}, {-1.04, 6.80},  {0.06, 6.86},  {3.36, 6.20},  {0.66, 8.70},
            {-3.52, 8.38}, {-7.06, 6.06}, {-6.92, 2.52}, {-4.18, -1.16}, {2.62, -2.22}, {2.74, -0.16}, {0.40, 0.96},
            {-2.36, 1.10}, {-4.94, 1.74}, {-6.06, 3.90}, {-5.32, 5.78},  {-3.62, 7.20}, {-0.88, 8.02}, {-1.30, 7.78},
            {-0.88, 7.68}, {-3.28, 6.90}, {-4.78, 5.56}, {-5.22, 3.84},  {-4.74, 2.60}, {-2.20, 1.98}, {0.40, 2.42},
        };

        Winding winding = GENERATE(Winding::ccw, Winding::cw);
        if (winding == Winding::cw)
        {
          flip_horizontally(vertices_storage);
        }

        VerticesView vertices(vertices_storage);
        NodePool node_pool;

        std::vector<ChainDecomposition> chain_decompositions =
            initial_chain_decompositions(vertices, winding, node_pool);
        REQUIRE(chain_decompositions.size() == 6);

        ChainDecomposition a =
            merge_chain_decompositions(vertices, winding, node_pool, chain_decompositions[4], chain_decompositions[5]);
        REQUIRE(validate_chain_decomposition(vertices, winding, a));

        ChainDecomposition b = merge_chain_decompositions(
            vertices, winding, node_pool,
            merge_chain_decompositions(vertices, winding, node_pool, chain_decompositions[0], chain_decompositions[1]),
            chain_decompositions[2]);
        REQUIRE(validate_chain_decomposition(vertices, winding, b));

        ChainDecomposition merged = merge_chain_decompositions(vertices, winding, node_pool, a, b);
        CHECK(validate_chain_decomposition(vertices, winding, merged));
      }
    }

    SECTION("New outer branch in merge_tail")
    {
      SECTION("Towards right")
      {
        std::vector<Point2> vertices_storage{
            {-3.42, 3.80}, {-1.56, 3.08},  {0.86, 3.90},  {-0.38, 5.42}, {-3.58, 6.42}, {-6.18, 4.34},
            {-5.58, 1.00}, {-2.48, -0.36}, {1.70, -0.12}, {4.42, 1.86},  {4.68, 4.96},  {3.18, 7.50},
            {0.20, 8.86},  {-3.96, 8.36},  {0.06, 7.42},  {2.02, 6.68},  {2.60, 5.06},  {2.52, 2.96},
            {1.04, 1.84},  {-1.96, 1.80},  {-4.88, 3.92}, {-3.02, 4.28},
        };

        Winding winding = GENERATE(Winding::ccw, Winding::cw);
        if (winding == Winding::cw)
        {
          flip_horizontally(vertices_storage);
        }

        VerticesView vertices(vertices_storage);
        NodePool node_pool;

        std::vector<ChainDecomposition> chain_decompositions =
            initial_chain_decompositions(vertices, winding, node_pool);
        REQUIRE(chain_decompositions.size() == 5);

        ChainDecomposition a =
            merge_chain_decompositions(vertices, winding, node_pool, chain_decompositions[0], chain_decompositions[1]);
        REQUIRE(validate_chain_decomposition(vertices, winding, a));

        ChainDecomposition b =
            merge_chain_decompositions(vertices, winding, node_pool, chain_decompositions[2], chain_decompositions[3]);
        REQUIRE(validate_chain_decomposition(vertices, winding, b));

        ChainDecomposition merged = merge_chain_decompositions(vertices, winding, node_pool, a, b);
        CHECK(validate_chain_decomposition(vertices, winding, merged));
      }

      SECTION("Towards left")
      {
        Polygon2 polygon{
            {7.68, 5.80},  {6.50, 6.16},  {4.38, 5.38}, {5.82, 4.06}, {9.26, 4.14}, {11.68, 6.28},
            {9.50, 9.18},  {6.74, 10.06}, {2.66, 9.18}, {0.44, 7.20}, {2.24, 3.70}, {5.08, 1.56},
            {10.24, 1.88}, {11.10, 2.96}, {9.50, 2.58}, {5.46, 2.42}, {3.00, 4.24}, {2.34, 7.12},
            {3.44, 8.32},  {6.66, 9.22},  {8.96, 8.46}, {9.68, 6.42}, {8.76, 5.24}, {7.12, 5.34},
        };

        VerticesView vertices(polygon);
        NodePool node_pool;

        std::vector<ChainDecomposition> chain_decompositions =
            initial_chain_decompositions(vertices, Winding::ccw, node_pool);
        REQUIRE(chain_decompositions.size() == 5);

        ChainDecomposition a = merge_chain_decompositions(vertices, Winding::ccw, node_pool, chain_decompositions[0],
                                                          chain_decompositions[1]);
        REQUIRE(validate_chain_decomposition(vertices, Winding::ccw, a));

        ChainDecomposition b = merge_chain_decompositions(vertices, Winding::ccw, node_pool, chain_decompositions[2],
                                                          chain_decompositions[3]);
        REQUIRE(validate_chain_decomposition(vertices, Winding::ccw, b));

        ChainDecomposition merged = merge_chain_decompositions(vertices, Winding::ccw, node_pool, a, b);
        CHECK(validate_chain_decomposition(vertices, Winding::ccw, merged));
      }
    }

    SECTION("New outer branch in merge_iteration_reverse_branch, case 2")
    {
      SECTION("Towards left, tail is lower chain")
      {
        std::vector<Point2> vertices_storage{
            {-5.78, 2.38}, {0.52, -0.70}, {5.76, 2.60}, {2.76, 6.20}, {-1.66, 5.78}, {-3.06, 4.34},
            {-1.86, 1.88}, {1.22, 1.84},  {2.86, 3.46}, {1.34, 4.68}, {-0.68, 3.38}, {-0.32, 3.04},
            {-1.30, 3.28}, {1.36, 5.56},  {3.96, 3.20}, {1.82, 0.88}, {0.40, 0.32},
        };

        Winding winding = GENERATE(Winding::ccw, Winding::cw);
        if (winding == Winding::cw)
        {
          flip_horizontally(vertices_storage);
        }

        VerticesView vertices(vertices_storage);
        NodePool node_pool;

        std::vector<ChainDecomposition> chain_decompositions =
            initial_chain_decompositions(vertices, winding, node_pool);
        REQUIRE(chain_decompositions.size() == 5);

        ChainDecomposition a = chain_decompositions[0];
        ChainDecomposition b = merge_chain_decompositions(
            vertices, winding, node_pool, chain_decompositions[1],
            merge_chain_decompositions(vertices, winding, node_pool, chain_decompositions[2], chain_decompositions[3]));
        REQUIRE(validate_chain_decomposition(vertices, winding, b));

        ChainDecomposition merged = merge_chain_decompositions(vertices, winding, node_pool, a, b);
        CHECK(validate_chain_decomposition(vertices, winding, merged));
      }

      SECTION("Towards right, tail is upper chain")
      {
        std::vector<Point2> vertices_storage{
            {-5.36, 5.98}, {0.02, 7.20},  {4.04, 4.10},  {2.72, 1.96}, {-0.60, 2.80}, {1.08, 3.40}, {0.74, 2.70},
            {2.96, 4.12},  {-0.08, 5.08}, {-2.64, 2.22}, {3.00, 1.46}, {5.62, 4.64},  {0.04, 8.08},
        };

        Winding winding = GENERATE(Winding::ccw, Winding::cw);
        if (winding == Winding::cw)
        {
          flip_horizontally(vertices_storage);
        }

        VerticesView vertices(vertices_storage);
        NodePool node_pool;

        std::vector<ChainDecomposition> chain_decompositions =
            initial_chain_decompositions(vertices, winding, node_pool);
        REQUIRE(chain_decompositions.size() == 5);

        ChainDecomposition a = merge_chain_decompositions(
            vertices, winding, node_pool, chain_decompositions[1],
            merge_chain_decompositions(vertices, winding, node_pool, chain_decompositions[2], chain_decompositions[3]));
        REQUIRE(validate_chain_decomposition(vertices, winding, a));

        ChainDecomposition b = chain_decompositions[4];

        ChainDecomposition merged = merge_chain_decompositions(vertices, winding, node_pool, a, b);
        CHECK(validate_chain_decomposition(vertices, winding, merged));
      }

      SECTION("Towards right, tail is lower chain")
      {
        std::vector<Point2> vertices_storage{
            {7.18, -0.06}, {4.16, 2.80},  {-3.88, 1.52}, {-6.24, 3.94}, {-2.64, 6.82}, {1.08, 5.72},
            {0.10, 4.06},  {-1.60, 3.32}, {-3.34, 4.28}, {-2.70, 5.00}, {-0.84, 4.92}, {-1.28, 4.34},
            {-0.20, 4.52}, {-0.56, 5.24}, {-2.78, 5.86}, {-4.70, 4.10}, {-1.64, 2.68}, {0.40, 3.52},
            {2.18, 5.74},  {-2.80, 7.62}, {-8.16, 3.74}, {-4.16, 0.80}, {3.98, 1.60},
        };

        Winding winding = GENERATE(Winding::ccw, Winding::cw);
        if (winding == Winding::cw)
        {
          flip_horizontally(vertices_storage);
        }

        VerticesView vertices(vertices_storage);
        NodePool node_pool;

        std::vector<ChainDecomposition> chain_decompositions =
            initial_chain_decompositions(vertices, winding, node_pool);
        REQUIRE(chain_decompositions.size() == 6);

        ChainDecomposition a = merge_chain_decompositions(
            vertices, winding, node_pool,
            merge_chain_decompositions(vertices, winding, node_pool, chain_decompositions[2], chain_decompositions[3]),
            chain_decompositions[4]);
        REQUIRE(validate_chain_decomposition(vertices, winding, a));

        ChainDecomposition b = chain_decompositions[5];

        ChainDecomposition merged = merge_chain_decompositions(vertices, winding, node_pool, a, b);
        CHECK(validate_chain_decomposition(vertices, winding, merged));
      }

      SECTION("Towards right, tail is upper chain")
      {
        std::vector<Point2> vertices_storage{
            {5.60, 4.36},  {-0.14, 8.24}, {-6.36, 5.00}, {-2.10, 0.14}, {3.38, 1.32}, {1.36, 5.34},
            {-4.08, 4.96}, {-1.52, 2.22}, {0.32, 2.86},  {-0.02, 3.14}, {0.74, 3.16}, {0.58, 2.18},
            {-1.86, 1.40}, {-5.20, 4.88}, {-0.14, 7.44}, {5.16, 3.92},
        };

        Winding winding = GENERATE(Winding::ccw, Winding::cw);
        if (winding == Winding::cw)
        {
          flip_horizontally(vertices_storage);
        }

        VerticesView vertices(vertices_storage);
        NodePool node_pool;

        std::vector<ChainDecomposition> chain_decompositions =
            initial_chain_decompositions(vertices, winding, node_pool);
        REQUIRE(chain_decompositions.size() == 5);

        ChainDecomposition a = chain_decompositions[0];
        ChainDecomposition b = merge_chain_decompositions(
            vertices, winding, node_pool, chain_decompositions[1],
            merge_chain_decompositions(vertices, winding, node_pool, chain_decompositions[2], chain_decompositions[3]));
        REQUIRE(validate_chain_decomposition(vertices, winding, b));

        ChainDecomposition merged = merge_chain_decompositions(vertices, winding, node_pool, a, b);
        CHECK(validate_chain_decomposition(vertices, winding, merged));
      }
    }

    SECTION("Merge chain with outer branch")
    {
      SECTION("Outer branch towards left")
      {
        std::vector<Point2> vertices_storage{
            {-0.86, 3.52}, {2.50, 4.88},  {1.04, 6.98}, {-2.98, 7.04}, {-5.64, 3.62}, {-1.42, -0.34},
            {5.36, 1.52},  {6.72, 6.02},  {4.10, 8.98}, {-0.76, 9.56}, {-8.06, 9.08}, {-7.06, 8.84},
            {-7.80, 8.06}, {-0.88, 8.84}, {3.64, 8.16}, {5.72, 6.08},  {4.78, 2.34},  {-1.42, 0.42},
            {-4.52, 3.78}, {-2.86, 6.24}, {0.42, 6.18}, {-0.80, 4.76}, {-0.26, 4.40},
        };

        Winding winding = GENERATE(Winding::ccw, Winding::cw);
        if (winding == Winding::cw)
        {
          flip_horizontally(vertices_storage);
        }

        VerticesView vertices(vertices_storage);
        NodePool node_pool;

        std::vector<ChainDecomposition> chain_decompositions =
            initial_chain_decompositions(vertices, winding, node_pool);
        REQUIRE(chain_decompositions.size() == 7);

        ChainDecomposition chain_with_outer_branch = merge_chain_decompositions(
            vertices, winding, node_pool,
            merge_chain_decompositions(vertices, winding, node_pool, chain_decompositions[0], chain_decompositions[1]),
            merge_chain_decompositions(vertices, winding, node_pool, chain_decompositions[2], chain_decompositions[3]));
        REQUIRE(validate_chain_decomposition(vertices, winding, chain_with_outer_branch));

        SECTION("From upper branch")
        {
          ChainDecomposition other_chain = merge_chain_decompositions(vertices, winding, node_pool,
                                                                      chain_decompositions[5], chain_decompositions[6]);
          REQUIRE(validate_chain_decomposition(vertices, winding, other_chain));

          ChainDecomposition merged =
              merge_chain_decompositions(vertices, winding, node_pool, other_chain, chain_with_outer_branch);
          CHECK(validate_chain_decomposition(vertices, winding, merged));
        }

        SECTION("From lower branch")
        {
          ChainDecomposition other_chain = merge_chain_decompositions(vertices, winding, node_pool,
                                                                      chain_decompositions[4], chain_decompositions[5]);
          REQUIRE(validate_chain_decomposition(vertices, winding, other_chain));

          ChainDecomposition merged =
              merge_chain_decompositions(vertices, winding, node_pool, chain_with_outer_branch, other_chain);
          CHECK(validate_chain_decomposition(vertices, winding, merged));
        }
      }

      SECTION("Outer branch towards right")
      {
        std::vector<Point2> vertices_storage{
            {4.48, 6.48},  {0.02, 4.26},  {2.18, 2.18},   {5.84, 1.84},  {9.00, 5.00},   {4.42, 9.60},
            {-1.18, 8.54}, {-3.84, 4.06}, {-1.26, -0.66}, {5.86, -2.18}, {11.06, -1.10}, {10.20, -0.32},
            {10.56, 0.40}, {5.68, -1.12}, {-0.38, 0.18},  {-3.08, 4.02}, {-0.38, 7.86},  {4.12, 8.54},
            {7.92, 4.92},  {5.46, 3.04},  {2.86, 3.30},   {3.40, 4.50},  {5.34, 5.34},   {3.80, 5.56},
        };

        Winding winding = GENERATE(Winding::ccw, Winding::cw);
        if (winding == Winding::cw)
        {
          flip_horizontally(vertices_storage);
        }

        VerticesView vertices(vertices_storage);
        NodePool node_pool;

        std::vector<ChainDecomposition> chain_decompositions =
            initial_chain_decompositions(vertices, winding, node_pool);
        REQUIRE(chain_decompositions.size() == 7);

        ChainDecomposition chain_with_outer_branch = merge_chain_decompositions(
            vertices, winding, node_pool,
            merge_chain_decompositions(vertices, winding, node_pool, chain_decompositions[0], chain_decompositions[1]),
            merge_chain_decompositions(vertices, winding, node_pool, chain_decompositions[2], chain_decompositions[3]));
        REQUIRE(validate_chain_decomposition(vertices, winding, chain_with_outer_branch));

        SECTION("From upper branch")
        {
          ChainDecomposition other_chain = merge_chain_decompositions(vertices, winding, node_pool,
                                                                      chain_decompositions[5], chain_decompositions[6]);
          REQUIRE(validate_chain_decomposition(vertices, winding, other_chain));

          ChainDecomposition merged =
              merge_chain_decompositions(vertices, winding, node_pool, other_chain, chain_with_outer_branch);
          CHECK(validate_chain_decomposition(vertices, winding, merged));
        }

        SECTION("From lower branch")
        {
          ChainDecomposition other_chain = merge_chain_decompositions(vertices, winding, node_pool,
                                                                      chain_decompositions[4], chain_decompositions[5]);
          REQUIRE(validate_chain_decomposition(vertices, winding, other_chain));

          ChainDecomposition merged =
              merge_chain_decompositions(vertices, winding, node_pool, chain_with_outer_branch, other_chain);
          CHECK(validate_chain_decomposition(vertices, winding, merged));
        }
      }
    }
  }
}

} // namespace dida::detail::vertical_decomposition