#define CATCH_CONFIG_MAIN
#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch2/catch.hpp>

#define BOOST_ALLOW_DEPRECATED_HEADERS
#include <boost/geometry/geometry.hpp>

#include <geos_c.h>
#include <stdarg.h>

#include <CGAL/Boolean_set_operations_2.h>
#include <CGAL/Cartesian.h>
#include <CGAL/Polygon_2.h>

struct Point
{
  double x;
  double y;
};

using BoostPointD = boost::geometry::model::d2::point_xy<double>;
using BoostPolygonD = boost::geometry::model::polygon<BoostPointD>;

using CGALKernel = CGAL::Cartesian<double>;
using CGALPoint = CGALKernel::Point_2;
using CGALPolygon = CGAL::Polygon_2<CGALKernel>;

BoostPolygonD to_boost_polygon(const std::vector<Point>& vertices)
{
  BoostPolygonD result;
  for (Point vertex : vertices)
  {
    boost::geometry::append(result.outer(), BoostPointD(vertex.x, vertex.y));
  }
  boost::geometry::append(result.outer(), BoostPointD(vertices[0].x, vertices[0].y));

  return result;
}

GEOSGeometry* to_geos_polygon(const std::vector<Point>& vertices)
{
  GEOSCoordSequence* geos_vertices = GEOSCoordSeq_create(vertices.size() + 1, 2);
  for (size_t i = 0; i < vertices.size(); i++)
  {
    GEOSCoordSeq_setX(geos_vertices, i, vertices[i].x);
    GEOSCoordSeq_setY(geos_vertices, i, vertices[i].x);
  }

  GEOSCoordSeq_setX(geos_vertices, vertices.size(), vertices[0].x);
  GEOSCoordSeq_setY(geos_vertices, vertices.size(), vertices[0].x);

  GEOSGeometry* ring = GEOSGeom_createLinearRing(geos_vertices);
  return GEOSGeom_createPolygon(ring, nullptr, 0);
}

CGALPolygon to_cgal_polygon(const std::vector<Point>& vertices)
{
  CGALPolygon result;
  for (size_t i = 0; i < vertices.size(); i++)
  {
    result.push_back(CGALPoint(vertices[i].x, vertices[i].y));
  }

  return result;
}

static void geos_msg_handler(const char* fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  vprintf(fmt, ap);
  va_end(ap);
}

TEST_CASE("Polygon intersection test benchmark")
{
  initGEOS(geos_msg_handler, geos_msg_handler);

  SECTION("20 x 20 vertices")
  {
    std::vector<Point> a_vertices{{1.72, 8.00},   {1.70, 6.54},  {-0.82, 6.50}, {-2.46, 5.70}, {-3.14, 4.10},
                                  {-3.18, 3.04},  {-2.18, 0.88}, {-0.64, 0.30}, {1.98, 0.36},  {1.94, -1.26},
                                  {-2.44, -1.12}, {-4.34, 0.96}, {-6.32, 2.50}, {-4.44, 4.22}, {-5.60, 5.16},
                                  {-3.98, 5.70},  {-6.00, 7.38}, {-3.70, 8.60}, {-1.56, 9.34}, {0.52, 9.00}};
    std::vector<Point> b_vertices{
        {-3.12, 12.74}, {0.62, 13.36}, {-0.44, 12.22}, {4.06, 11.94}, {2.46, 10.74}, {7.98, 7.94},  {8.16, 5.02},
        {5.44, 3.86},   {7.12, 2.78},  {8.60, 1.22},   {8.40, -1.12}, {5.52, -3.56}, {1.52, -4.64}, {-1.12, -3.66},
        {1.68, -2.84},  {4.18, -0.62}, {2.06, 1.80},   {-0.90, 2.90}, {3.36, 6.74},  {0.66, 10.38}, {-3.12, 12.74}};

    {
      BoostPolygonD a_boost = to_boost_polygon(a_vertices);
      BoostPolygonD b_boost = to_boost_polygon(b_vertices);

      BENCHMARK("boost::geometry")
      {
        return boost::geometry::intersects(a_boost, b_boost);
      };
    }

    {
      GEOSGeometry* a_geos = to_geos_polygon(a_vertices);
      GEOSGeometry* b_geos = to_geos_polygon(b_vertices);

      BENCHMARK("GEOS")
      {
        return GEOSIntersects(a_geos, b_geos);
      };

      const GEOSPreparedGeometry* a_geos_prepared = GEOSPrepare(a_geos);
      
      // Run it once to initialize the lazy parts of GEOSPreparedGeometry.
      GEOSPreparedIntersects(a_geos_prepared, b_geos);

      BENCHMARK("GEOS, geometry A prepared")
      {
        return GEOSPreparedIntersects(a_geos_prepared, b_geos);
      };

      GEOSPreparedGeom_destroy(a_geos_prepared);
      GEOSGeom_destroy(a_geos);
      GEOSGeom_destroy(b_geos);
    }

    {
      CGALPolygon a_cgal = to_cgal_polygon(a_vertices);
      CGALPolygon b_cgal = to_cgal_polygon(a_vertices);

      BENCHMARK("CGAL")
      {
        return CGAL::do_intersect(a_cgal, b_cgal);
      };
    }

    finishGEOS();
  }
}