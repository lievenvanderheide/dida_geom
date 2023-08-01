#define CATCH_CONFIG_MAIN
#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch2/catch.hpp>

#include "dida/polygon2.hpp"

#define BOOST_ALLOW_DEPRECATED_HEADERS
#include <boost/geometry/geometry.hpp>

#include <geos_c.h>
#include <stdarg.h>

#include "dida/detail/vertical_decomposition_dnc_builder.hpp"
#include "dida/detail/vertical_decomposition_zigzag_phase.hpp"
#include "dida/benchmarks/stock_objects.hpp"

/*#include <CGAL/Boolean_set_operations_2.h>
#include <CGAL/Cartesian.h>
#include <CGAL/Polygon_2.h>*/

using BoostPointD = boost::geometry::model::d2::point_xy<double>;
using BoostPolygonD = boost::geometry::model::polygon<BoostPointD>;

/*using CGALKernel = CGAL::Cartesian<double>;
using CGALPoint = CGALKernel::Point_2;
using CGALPolygon = CGAL::Polygon_2<CGALKernel>;*/

/// Converts a Dida polygon to a boost::geometry polygon.
///
/// @param dida_polygon The Dida polygon.
/// @return The boost::geometry polygon.
BoostPolygonD to_boost_polygon(dida::PolygonView2 dida_polygon)
{
  BoostPolygonD result;
  for (dida::Point2 vertex : dida_polygon)
  {
    boost::geometry::append(result.outer(),
                            BoostPointD(static_cast<double>(vertex.x()), static_cast<double>(vertex.y())));
  }
  boost::geometry::append(
      result.outer(), BoostPointD(static_cast<double>(dida_polygon[0].x()), static_cast<double>(dida_polygon[0].y())));

  return result;
}

/// Converts a Dida polygon to a GEOS polygon.
///
/// @param dida_polygon The Dida polygon.
/// @return The GEOS polygon.
GEOSGeometry* to_geos_polygon(dida::PolygonView2 dida_polygon)
{
  GEOSCoordSequence* geos_vertices = GEOSCoordSeq_create(dida_polygon.size() + 1, 2);
  for (size_t i = 0; i < dida_polygon.size(); i++)
  {
    GEOSCoordSeq_setX(geos_vertices, i, static_cast<double>(dida_polygon[i].x()));
    GEOSCoordSeq_setY(geos_vertices, i, static_cast<double>(dida_polygon[i].y()));
  }

  GEOSCoordSeq_setX(geos_vertices, dida_polygon.size(), static_cast<double>(dida_polygon[0].x()));
  GEOSCoordSeq_setY(geos_vertices, dida_polygon.size(), static_cast<double>(dida_polygon[0].y()));

  GEOSGeometry* ring = GEOSGeom_createLinearRing(geos_vertices);
  return GEOSGeom_createPolygon(ring, nullptr, 0);
}

/*CGALPolygon to_cgal_polygon(dida::PolygonView2 dida_polygon)
{
  CGALPolygon result;
  for (dida::Point2 vertex : dida_polygon)
  {
    result.push_back(CGALPoint(static_cast<double>(vertex.x()), static_cast<double>(vertex.y())));
  }

  return result;
}*/

static void geos_msg_handler(const char* fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  vprintf(fmt, ap);
  va_end(ap);
}

TEST_CASE("Polygons intersect benchmark")
{
  initGEOS(geos_msg_handler, geos_msg_handler);

  SECTION("20 x 20 vertices")
  {
    dida::Polygon2 a{
        {1.72, 8.00},  {1.70, 6.54},  {-0.82, 6.50}, {-2.46, 5.70},  {-3.14, 4.10}, {-3.18, 3.04}, {-2.18, 0.88},
        {-0.64, 0.30}, {1.98, 0.36},  {1.94, -1.26}, {-2.44, -1.12}, {-4.34, 0.96}, {-6.32, 2.50}, {-4.44, 4.22},
        {-5.60, 5.16}, {-3.98, 5.70}, {-6.00, 7.38}, {-3.70, 8.60},  {-1.56, 9.34}, {0.52, 9.00},
    };

    dida::Polygon2 b{
        {-3.12, 12.74}, {0.62, 13.36}, {-0.44, 12.22}, {4.06, 11.94}, {2.46, 10.74}, {7.98, 7.94},  {8.16, 5.02},
        {5.44, 3.86},   {7.12, 2.78},  {8.60, 1.22},   {8.40, -1.12}, {5.52, -3.56}, {1.52, -4.64}, {-1.12, -3.66},
        {1.68, -2.84},  {4.18, -0.62}, {2.06, 1.80},   {-0.90, 2.90}, {3.36, 6.74},  {0.66, 10.38}, {-3.12, 12.74},
    };

    {
      BoostPolygonD a_boost = to_boost_polygon(a);
      BoostPolygonD b_boost = to_boost_polygon(b);

      BENCHMARK("boost::geometry")
      {
        return boost::geometry::intersects(a_boost, b_boost);
      };
    }

    {
      GEOSGeometry* a_geos = to_geos_polygon(a);
      GEOSGeometry* b_geos = to_geos_polygon(b);

      BENCHMARK("GEOS")
      {
        return GEOSIntersects(a_geos, b_geos);
      };

      const GEOSPreparedGeometry* a_geos_prepared = GEOSPrepare(a_geos);

      // Run it once to initialize the lazy parts of GEOSPreparedGeometry.
      GEOSPreparedIntersects(a_geos_prepared, b_geos);

      // Actually, run it twice, because apparently there are parts which are only initialized after the second run.
      GEOSPreparedIntersects(a_geos_prepared, b_geos);

      BENCHMARK("GEOS, geometry A prepared")
      {
        return GEOSPreparedIntersects(a_geos_prepared, b_geos);
      };

      GEOSPreparedGeom_destroy(a_geos_prepared);
      GEOSGeom_destroy(a_geos);
      GEOSGeom_destroy(b_geos);
    }

    /*{
      CGALPolygon a_cgal = to_cgal_polygon(a);
      CGALPolygon b_cgal = to_cgal_polygon(b);

      BENCHMARK("CGAL")
      {
        return CGAL::do_intersect(a_cgal, b_cgal);
      };
    }*/
  }

  SECTION("Somewhat simple")
  {
    using namespace dida;
    using namespace dida::detail;
    using namespace dida::detail::vertical_decomposition;

    /*Polygon2 polygon{
        {-4.66, 3.32},  {-2.50, 3.48},  {-2.02, 1.72}, {-4.28, 0.68}, {-2.16, -0.62}, {0.16, 0.48},  {3.88, -0.78},
        {6.74, 2.84},   {4.60, 8.42},   {1.12, 9.48},  {-3.18, 8.68}, {-2.74, 8.06},  {-0.62, 7.82}, {1.32, 7.92},
        {3.68, 6.56},   {4.12, 4.06},   {3.20, 2.48},  {1.14, 1.74},  {-0.68, 3.22},  {-0.50, 5.10}, {1.56, 5.98},
        {2.34, 5.22},   {2.20, 4.24},   {0.98, 3.44},  {0.50, 4.10},  {1.30, 4.80},   {-0.04, 4.32}, {0.52, 2.98},
        {1.80, 2.82},   {2.88, 3.72},   {3.04, 5.00},  {2.48, 6.42},  {0.50, 7.24},   {-0.86, 6.40}, {-1.48, 4.82},
        {-3.42, 5.70},  {-5.72, 5.62},  {-7.62, 4.06}, {-7.36, 2.46}, {-5.94, 2.16},  {-4.30, 2.12}, {-5.78, 0.08},
        {-0.38, -2.68}, {-2.14, -1.30}, {-4.86, 0.08}, {-4.04, 0.14}, {-4.80, 0.82},  {-3.18, 2.30},
    };*/

    PolygonView2 netherlands = dida::benchmarks::stock_objects::germany();
    VerticesView vertices(netherlands.begin(), netherlands.size());

    BENCHMARK("Zigzag")
    {
      NodePool node_pool;
      return vertical_decomposition_zigzag_phase(vertices, node_pool);
    };

    BENCHMARK("Lala")
    {
      NodePool node_pool;
      Node* root_node =
          vertical_decomposition_with_dnc_builder(vertices, node_pool, VerticalDecompositionType::interior_decomposition);
      return root_node;
    };
  }

  finishGEOS();
}