#include "dida/detail/vertical_decomposition/triangulate.hpp"

#include <catch2/catch.hpp>
#include <unordered_set>

#include "dida/detail/vertical_decomposition/sweep_line_builder.hpp"
#include "dida/polygon2.hpp"
#include "dida/predicates.hpp"

namespace dida::detail::vertical_decomposition
{

/// Returns whether @c q is (non stricly) in front of the edge starting at @c p_edge_start with direction @c p_edge_dir. 
bool is_separating_axis(Point2 p_edge_start, Vector2 p_edge_dir, const Triangle2& q)
{
  for (Point2 q_vertex : q)
  {
    if (cross(p_edge_dir, q_vertex - p_edge_start) < 0)
    {
      return false;
    }
  }

  return true;
}

/// Returns whether triangles @c a and @c b intersect.
bool intersect(const Triangle2& a, const Triangle2& b)
{
  for (size_t i = 0; i < 3; i++)
  {
    if (is_separating_axis(a[i], a[succ_modulo(i, a.size())] - a[i], b) ||
        is_separating_axis(b[i], b[succ_modulo(i, b.size())] - b[i], a))
    {
      return false;
    }
  }

  return true;
}

/// Returns whether @c a and @c b cross on their interiors.
bool interiors_cross(Segment2 a, Segment2 b)
{
  ScalarDeg2 a_start_side = cross(b.direction(), a.start() - b.start());
  ScalarDeg2 a_end_side = cross(b.direction(), a.end() - b.start());
  if ((a_start_side <= 0 && a_end_side <= 0) || (a_end_side >= 0 && a_end_side >= 0))
  {
    return false;
  }

  ScalarDeg2 b_start_side = cross(a.direction(), b.start() - a.start());
  ScalarDeg2 b_end_side = cross(a.direction(), b.end() - a.start());
  if ((b_start_side <= 0 && b_end_side <= 0) || (b_end_side >= 0 && b_end_side >= 0))
  {
    return false;
  }

  return true;
}

/// Returns whether @c triangle is contained within @c polygon.
bool is_within(PolygonView2 polygon, const Triangle2& triangle)
{
  for (size_t i = 0; i < 3; i++)
  {
    if (!is_within(polygon, triangle[i]))
    {
      return false;
    }

    Segment2 triangle_edge(triangle[i], triangle[succ_modulo<size_t>(i, 3)]);
    for (size_t j = 0; j < polygon.size(); j++)
    {
      Segment2 polygon_edge(polygon[j], polygon[succ_modulo(j, polygon.size())]);
      if (interiors_cross(triangle_edge, polygon_edge))
      {
        return false;
      }
    }
  }

  return true;
}

/// Validates whether @c triangles form a valid triangulation of @c polygon.
///
/// The triangulation is valid if it's a tessellation of @c polygon.
bool validate_triangulation(PolygonView2 polygon, ArrayView<const Triangle2> triangles)
{
  // In order to validate whether 'triangles' are a tessellation of 'polygon', we check the following:
  //
  // 1. The number of triangles is polygon.size() - 2.
  // 2. Each triangle is valid.
  // 3. All triangle vertices are vertices of 'polygon'.
  // 4. All triangles are contained within 'polygon'
  // 5. The triangles don't overlap.
  //

  if (triangles.size() != polygon.size() - 2)
  {
    UNSCOPED_INFO("Incorrect number of triangles in triangulation. Expected: " << polygon.size() - 2
                                                                               << ", actual: " << triangles.size());
    return false;
  }

  std::unordered_set<Point2> vertices_set(polygon.begin(), polygon.end());

  for (size_t i = 0; i < triangles.size(); i++)
  {
    for (size_t j = 0; j < 3; j++)
    {
      if (vertices_set.find(triangles[i][j]) == vertices_set.end())
      {
        UNSCOPED_INFO("triangles[" << i << "], vertex " << j << " does not occur in 'polygon'");
        return false;
      }
    }

    if (!validate_convex_polygon_vertices(triangles[i]))
    {
      UNSCOPED_INFO("triangles[" << i << "] isn't valid.");
      return false;
    }

    if (!is_within(polygon, triangles[i]))
    {
      UNSCOPED_INFO("triangles[" << i << "] isn't contained within 'polygon'.");
      return false;
    }
  }

  for (size_t i = 0; i < triangles.size(); i++)
  {
    for (size_t j = i + 1; j < triangles.size(); j++)
    {
      if (intersect(triangles[i], triangles[j]))
      {
        UNSCOPED_INFO("triangles[" << i << "]: " << triangles[i] << " and triangles[" << j << "] " << triangles[j]
                                   << " intersect.");
        return false;
      }
    }
  }

  return true;
}

TEST_CASE("triangulate")
{
  auto test = [](PolygonView2 polygon)
  {
    VerticesView vertices(polygon);

    VerticalDecomposition vertical_decomposition =
        vertical_decomposition_with_sweep_line_builder(vertices, VerticalDecompositionType::interior_decomposition);

    std::vector<Triangle2> triangles = triangulate(vertices, vertical_decomposition.leftmost_node);
    CHECK(validate_triangulation(polygon, triangles));
  };

  SECTION("Single monotone region")
  {
    SECTION("General")
    {
      Polygon2 polygon{
          {-6.94, 0.12},  {-5.70, -0.02}, {-5.14, 1.96},  {-4.58, 2.96}, {-3.64, 4.30}, {-2.64, 5.26}, {-1.84, 5.68},
          {-0.50, 6.08},  {0.70, 5.94},   {1.64, 5.54},   {2.52, 4.94},  {3.00, 4.46},  {3.46, 3.90},  {4.26, 2.64},
          {4.56, 1.78},   {4.70, 1.06},   {4.82, 0.30},   {8.68, -3.18}, {13.80, 0.22}, {13.90, 2.56}, {12.88, 0.50},
          {12.08, -0.16}, {11.06, -0.70}, {10.08, -0.78}, {9.00, -0.58}, {7.74, -0.28}, {6.88, 0.52},  {6.54, 1.40},
          {6.34, 2.46},   {6.28, 3.90},   {-0.20, 9.38},  {-6.88, 2.38},
      };
      test(polygon);
    }

    SECTION("With vertical edges")
    {
      Polygon2 polygon{
          {-4, 5}, {-4, 4}, {-4, 3}, {-2, 3}, {-2, 4}, {-1, 4}, {-1, 5}, {-1, 6},
          {1, 6},  {1, 7},  {1, 8},  {-1, 8}, {-2, 8}, {-2, 6}, {-2, 5},
      };
      test(polygon);
    }

    SECTION("Triangle with mid vertex on upper edge")
    {
      Polygon2 polygon{{-1.50, 4.94}, {-3.62, 3.14}, {0.94, 2.46}};
      test(polygon);
    }

    SECTION("Triangle with mid vertex on lower edge")
    {
      Polygon2 polygon{{0.16, 1.28}, {3.02, 3.44}, {-3.04, 4.10}};
      test(polygon);
    }
  }

  SECTION("Right branches")
  {
    SECTION("Take lower branch first")
    {
      Polygon2 polygon{
          {-7.52, 2.90}, {6.78, 1.54},  {5.12, 2.64},  {3.38, 3.34},  {2.22, 4.26},  {2.06, 4.84},
          {2.28, 5.34},  {2.76, 5.92},  {3.56, 5.96},  {4.24, 5.54},  {4.90, 4.90},  {6.78, 8.58},
          {0.24, 5.16},  {-2.68, 4.68}, {-3.40, 5.00}, {-4.40, 6.14}, {-4.78, 6.90},
      };
      test(polygon);
    }

    SECTION("Take upper branch first")
    {
      Polygon2 polygon{
          {-6.34, 1.54}, {-4.38, -1.70}, {-4.02, -0.32}, {-3.56, 0.90}, {-2.82, 1.62}, {-2.10, 1.64}, {-1.48, 1.16},
          {-1.28, 1.54}, {-0.74, 1.88},  {-0.06, 1.72},  {0.58, 1.30},  {0.96, 0.72},  {1.68, -0.92}, {3.04, -1.48},
          {5.44, -1.58}, {6.16, -0.34},  {6.44, 1.24},   {5.66, 2.14},  {4.46, 2.90},  {3.02, 2.54},  {1.80, 2.68},
          {0.94, 3.40},  {0.46, 4.42},   {1.04, 5.24},   {2.22, 5.84},  {2.88, 5.74},  {3.32, 8.06},
      };
      test(polygon);
    }

    SECTION("No mid vertices")
    {
      Polygon2 polygon{{1.14, 5.48}, {3.68, 4.40}, {2.32, 2.34}, {6.16, 4.74}};
      test(polygon);
    }

    SECTION("Multiple right branches, no mid vertices")
    {
      Polygon2 polygon{
          {-4.60, 6.64}, {-4.18, -0.50}, {0.90, 0.26},  {-0.24, 1.00}, {4.58, 1.92},  {2.58, 2.56},
          {7.12, 4.12},  {-2.64, 5.32},  {-0.78, 5.94}, {-3.68, 6.02}, {-2.20, 6.46},
      };
      test(polygon);
    }

    SECTION("With vertical edges")
    {
      Polygon2 polygon{{-1, 5}, {3, 5}, {3, 6}, {2, 6}, {2, 7}, {3, 7}, {3, 8}, {-1, 8}};
      test(polygon);
    }

    SECTION("Vertex below branch vertex")
    {
      Polygon2 polygon{{-6, 4}, {-4, 4}, {-3, 4}, {-4, 5}, {-3, 7}};
      test(polygon);
    }

    SECTION("Vertex above branch vertex")
    {
      Polygon2 polygon{{-5, 4}, {-1, 1}, {-2, 3}, {-1, 4}, {-2, 4}};
      test(polygon);
    }
  }

  SECTION("Left branches")
  {
    SECTION("Lower branch continues")
    {
      Polygon2 polygon{
          {-6.98, -1.36}, {-5.92, -0.34}, {-3.96, 1.04}, {0.90, 2.12},  {3.56, 2.08},  {2.66, 3.18},  {1.74, 5.14},
          {0.46, 5.86},   {-0.88, 4.24},  {-2.26, 2.84}, {-3.14, 3.26}, {-4.22, 4.54}, {-4.54, 5.90}, {-5.88, 6.36},
          {-6.98, 5.82},  {-5.90, 5.64},  {-5.38, 4.28}, {-4.46, 3.10}, {-3.50, 2.42}, {-5.44, 1.26},
      };
      test(polygon);
    }

    SECTION("Upper branch continues")
    {
      Polygon2 polygon{
          {-3.56, 4.60}, {-1.70, 3.42}, {-0.10, 3.78}, {0.68, 2.80}, {-0.46, 1.84},
          {-1.96, 2.30}, {-1.32, 1.26}, {2.92, 2.14},  {6.78, 0.98}, {-1.62, 5.58},
      };
      test(polygon);
    }

    SECTION("No mid vertices")
    {
      Polygon2 polygon{{-3.18, 2.60}, {3.36, 4.12}, {-4.36, 7.24}, {-1.66, 4.52}};
      test(polygon);
    }

    SECTION("Multiple left branches, no mid vertices")
    {
      Polygon2 polygon{
          {1.32, 0.32},  {2.36, 8.00},  {-0.94, 7.86}, {0.36, 7.10},  {-1.96, 6.58}, {-1.08, 6.16},
          {-5.96, 4.96}, {-4.28, 4.58}, {-5.10, 3.98}, {-2.88, 3.48}, {-4.06, 2.72},
      };
      test(polygon);
    }

    SECTION("With vertical edges")
    {
      Polygon2 polygon{{2, 5}, {2, 4}, {4, 4}, {4, 2}, {1, 2}, {1, 1}, {5, 1}, {5, 5}};
      test(polygon);
    }

    SECTION("Vertex below branch vertex")
    {
      Polygon2 polygon{{-6, 3}, {-5, 3}, {-2, 3}, {-6, 6}, {-5, 4}};
      test(polygon);
    }

    SECTION("Vertex above branch vertex")
    {
      Polygon2 polygon{{-7, 0}, {-3, 3}, {-6, 3}, {-8, 3}, {-6, 2}};
      test(polygon);
    }
  }

  SECTION("Facing left and right branches")
  {
    SECTION("Connected branch vertices")
    {
      Polygon2 polygon{
          {-8.02, 0.62}, {-6.62, 0.84}, {4.84, 0.86},  {5.90, 0.46},  {5.60, 2.00},  {5.08, 3.02},
          {4.22, 3.74},  {3.12, 4.34},  {2.08, 4.58},  {0.58, 4.60},  {2.32, 5.04},  {3.52, 5.38},
          {4.16, 6.06},  {4.76, 6.64},  {5.08, 7.64},  {5.28, 8.52},  {-9.22, 7.68}, {-8.50, 6.40},
          {-7.70, 5.22}, {-6.86, 4.68}, {-6.06, 4.42}, {-5.28, 4.40}, {-4.30, 4.72}, {-3.50, 4.86},
          {-2.78, 4.72}, {-2.08, 4.40}, {-1.50, 4.26}, {-2.34, 4.00}, {-3.26, 4.14}, {-3.88, 4.26},
          {-4.82, 3.96}, {-5.88, 3.38}, {-6.38, 3.38}, {-7.04, 3.24}, {-7.60, 2.58}, {-7.88, 1.88},
      };
      test(polygon);
    }

    SECTION("Connected branch vertices, vertical edges")
    {
      Polygon2 polygon{{2, 3}, {4, 3}, {4, 2}, {2, 2}, {2, 1}, {9, 1}, {9, 4}, {7, 4}, {7, 5}, {9, 5}, {9, 6}, {2, 6}};
      test(polygon);
    }

    SECTION("Vertices on lower boundary between branches")
    {
      Polygon2 polygon{
          {-7.76, 4.84}, {-7.46, 3.80}, {-7.02, 2.98}, {-6.50, 2.34}, {-5.70, 1.56}, {-4.88, 1.02}, {-3.66, 0.50},
          {-2.44, 0.20}, {-1.30, 0.26}, {1.20, 0.76},  {2.46, 1.68},  {3.50, 2.46},  {4.20, 3.34},  {4.84, 4.16},
          {5.26, 5.12},  {5.42, 5.92},  {5.46, 7.06},  {2.18, 4.10},  {5.32, 7.92},  {-8.06, 6.84}, {-3.72, 3.48},
      };
      test(polygon);
    }

    SECTION("Vertices on upper boundary between branches")
    {
      Polygon2 polygon{
          {-6.04, 1.50}, {-3.96, 2.80}, {5.60, 0.88},  {7.00, 2.26},  {5.32, 1.54},  {2.80, 3.02},  {4.96, 4.96},
          {0.68, 2.84},  {-0.72, 8.26}, {-1.38, 2.82}, {-2.12, 4.98}, {-5.44, 5.78}, {-3.38, 4.26},
      };
      test(polygon);
    }

    SECTION("Vertex below left branch vertex")
    {
      Polygon2 polygon{{2, 2}, {4, 2}, {11, 3}, {9, 4}, {11, 5}, {2, 5}, {4, 3}};
      test(polygon);
    }

    SECTION("Vertex above left branch vertex")
    {
      Polygon2 polygon{{-4, 3}, {3, 3}, {-1, 4}, {3, 5}, {-2, 6}, {-4, 5}, {-2, 4}};
      test(polygon);
    }

    SECTION("Vertex below right branch vertex")
    {
      Polygon2 polygon{{-4, 3}, {-2, 3}, {1, -2}, {3, -2}, {1, 0}, {3, 1}, {-4, 9}, {-1, 4}};
      test(polygon);
    }

    SECTION("Vertex above right branch vertex")
    {
      Polygon2 polygon{{-2, 2}, {4, 4}, {3, 5}, {5, 7}, {3, 8}, {0, 5}, {-4, 7}, {1, 4}};
      test(polygon);
    }
  }
}

} // namespace dida::detail::vertical_decomposition