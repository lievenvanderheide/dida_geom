#include "dida/detail/vertical_decomposition/divide_and_conquer_builder.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_all.hpp>

#include "dida/detail/vertical_decomposition/tests/test_utils.hpp"
#include "dida/polygon2.hpp"
#include "dida/polygon2_utils.hpp"

namespace dida::detail::vertical_decomposition
{

Polygon2 spiral_polygon(size_t num_revolutions, size_t num_vertices_per_revolution, double outer_radius,
                        double inner_radius)
{
  size_t boundary_num_vertices = num_revolutions * num_vertices_per_revolution;

  std::vector<Point2> inwards_vertices;
  std::vector<Point2> outward_vertices;

  for (size_t i = 0; i <= boundary_num_vertices; i++)
  {
    double angle = 2 * M_PI * static_cast<double>(i) / static_cast<double>(num_vertices_per_revolution);
    double inward_radius = outer_radius + (inner_radius - outer_radius) * static_cast<double>(i) /
                                              static_cast<double>(boundary_num_vertices);
    double outward_radius = inward_radius + (.5 / static_cast<double>(num_revolutions)) * (inner_radius - outer_radius);
    double s = std::sin(angle);
    double c = std::cos(angle);

    inwards_vertices.emplace_back(inward_radius * s, inward_radius * -c);
    outward_vertices.emplace_back(outward_radius * s, outward_radius * -c);
  }

  std::vector<Point2> vertices = std::move(inwards_vertices);
  for (size_t i = 0; i < outward_vertices.size(); i++)
  {
    vertices.push_back(outward_vertices[outward_vertices.size() - i - 1]);
  }

  return Polygon2(vertices);
}

TEST_CASE("vertical_decomposition_with_divide_and_conquer_builder")
{
  SECTION("Monotone polygon")
  {
    Polygon2 polygon{{-4.52, 3.44}, {-1.72, 0.24}, {1.52, 0.28}, {4.84, 4.12}, {1.06, 8.34}, {-0.74, 8.36}};

    VerticesView vertices(polygon);
    NodePool node_pool;

    Node* root_node = vertical_decomposition_with_divide_and_conquer_builder(
        vertices, node_pool, VerticalDecompositionType::interior_decomposition);
    CHECK(validate_polygon_decomposition(vertices, root_node));
  }

  SECTION("General case")
  {
    Polygon2 polygon{
        {-4.66, 3.32},  {-2.50, 3.48},  {-2.02, 1.72}, {-4.28, 0.68}, {-2.16, -0.62}, {0.16, 0.48},  {3.88, -0.78},
        {6.74, 2.84},   {4.60, 8.42},   {1.12, 9.48},  {-3.18, 8.68}, {-2.74, 8.06},  {-0.62, 7.82}, {1.32, 7.92},
        {3.68, 6.56},   {4.12, 4.06},   {3.20, 2.48},  {1.14, 1.74},  {-0.68, 3.22},  {-0.50, 5.10}, {1.56, 5.98},
        {2.34, 5.22},   {2.20, 4.24},   {0.98, 3.44},  {0.50, 4.10},  {1.30, 4.80},   {-0.04, 4.32}, {0.52, 2.98},
        {1.80, 2.82},   {2.88, 3.72},   {3.04, 5.00},  {2.48, 6.42},  {0.50, 7.24},   {-0.86, 6.40}, {-1.48, 4.82},
        {-3.42, 5.70},  {-5.72, 5.62},  {-7.62, 4.06}, {-7.36, 2.46}, {-5.94, 2.16},  {-4.30, 2.12}, {-5.78, 0.08},
        {-0.38, -2.68}, {-2.14, -1.30}, {-4.86, 0.08}, {-4.04, 0.14}, {-4.80, 0.82},  {-3.18, 2.30},
    };

    VerticesView vertices(polygon);
    NodePool node_pool;
    Node* root_node = vertical_decomposition_with_divide_and_conquer_builder(
        vertices, node_pool, VerticalDecompositionType::interior_decomposition);

    CHECK(validate_polygon_decomposition(vertices, root_node));
  }

  SECTION("Spirals")
  {
    size_t num_revolutions = GENERATE(2, 3, 10, 20);

    Polygon2 polygon = spiral_polygon(num_revolutions, 12, 10, 2);

    SECTION("Counter clockwise")
    {
      VerticesView vertices(polygon);
      NodePool node_pool;

      Node* root_node = vertical_decomposition_with_divide_and_conquer_builder(
          vertices, node_pool, VerticalDecompositionType::interior_decomposition);
      CHECK(validate_polygon_decomposition(vertices, root_node));
    }

    SECTION("Clockwise")
    {
      flip_vertically(polygon);

      VerticesView vertices(polygon);
      NodePool node_pool;

      Node* root_node = vertical_decomposition_with_divide_and_conquer_builder(
          vertices, node_pool, VerticalDecompositionType::interior_decomposition);
      CHECK(validate_polygon_decomposition(vertices, root_node));
    }
  }
}

} // namespace dida::detail::vertical_decomposition