#include "dida/detail/convex_polygons_intersection.hpp"

#include <catch2/catch.hpp>

#include "dida/segment2.hpp"

namespace dida::detail
{

namespace convex_polygons_intersection
{

TEST_CASE("other_arc")
{
  CHECK(other_arc(Arc::lower) == Arc::upper);
  CHECK(other_arc(Arc::upper) == Arc::lower);
}

TEST_CASE("PolygonInfo")
{
  ConvexPolygon2 polygon{{1.74, -5.4}, {2.88, -0.56}, {-1.9, 1.3}, {-9.48, -2.8}, {-8.36, -6.32}, {-5.44, -7.58}};
  PolygonInfo polygon_info(polygon);
  CHECK(std::equal(polygon_info.polygon.begin(), polygon_info.polygon.end(), polygon.begin(), polygon.end()));
  CHECK(*polygon_info.leftmost_it == Point2(-9.48, -2.8));
  CHECK(*polygon_info.rightmost_it == Point2(2.88, -0.56));
}

TEST_CASE("arc_vertex_less_than")
{
  SECTION("Arc::lower")
  {
    SECTION("PerturbationVector2::left_down")
    {
      CHECK(sweep_position_less_than<Arc::lower, PerturbationVector2::left_down>(ScalarDeg1(26), ScalarDeg1(90)));
      CHECK_FALSE(sweep_position_less_than<Arc::lower, PerturbationVector2::left_down>(ScalarDeg1(90), ScalarDeg1(26)));
      CHECK_FALSE(sweep_position_less_than<Arc::lower, PerturbationVector2::left_down>(ScalarDeg1(26), ScalarDeg1(26)));
    }

    SECTION("PerturbationVector2::right_up")
    {
      CHECK(sweep_position_less_than<Arc::lower, PerturbationVector2::right_up>(ScalarDeg1(26), ScalarDeg1(90)));
      CHECK_FALSE(sweep_position_less_than<Arc::lower, PerturbationVector2::right_up>(ScalarDeg1(90), ScalarDeg1(26)));
      CHECK(sweep_position_less_than<Arc::lower, PerturbationVector2::right_up>(ScalarDeg1(26), ScalarDeg1(26)));
    }
  }

  SECTION("Arc::upper")
  {
    SECTION("PerturbationVector2::left_down")
    {
      CHECK_FALSE(sweep_position_less_than<Arc::upper, PerturbationVector2::left_down>(ScalarDeg1(26), ScalarDeg1(90)));
      CHECK(sweep_position_less_than<Arc::upper, PerturbationVector2::left_down>(ScalarDeg1(90), ScalarDeg1(26)));
      CHECK(sweep_position_less_than<Arc::upper, PerturbationVector2::left_down>(ScalarDeg1(26), ScalarDeg1(26)));
    }

    SECTION("PerturbationVector2::right_up")
    {
      CHECK_FALSE(sweep_position_less_than<Arc::upper, PerturbationVector2::right_up>(ScalarDeg1(26), ScalarDeg1(90)));
      CHECK(sweep_position_less_than<Arc::upper, PerturbationVector2::right_up>(ScalarDeg1(90), ScalarDeg1(26)));
      CHECK_FALSE(sweep_position_less_than<Arc::upper, PerturbationVector2::right_up>(ScalarDeg1(26), ScalarDeg1(26)));
    }
  }
}

TEST_CASE("arc_first_vertex_it")
{
  ConvexPolygon2 polygon{{7, 2}, {5, 2}, {2, -1}, {2, -3}, {5, -3}, {7, -1}};
  PolygonInfo polygon_info(polygon);

  CHECK(*arc_first_vertex_it<Arc::lower>(polygon_info) == Point2(2, -3));
  CHECK(*arc_first_vertex_it<Arc::upper>(polygon_info) == Point2(7, 2));
}

TEST_CASE("arc_last_vertex_it")
{
  ConvexPolygon2 polygon{{2, 3}, {2, -2}, {9, 1}, {9, 2}};
  PolygonInfo polygon_info(polygon);

  CHECK(*arc_last_vertex_it<Arc::lower>(polygon_info) == Point2(9, 2));
  CHECK(*arc_last_vertex_it<Arc::upper>(polygon_info) == Point2(2, -2));
}

TEST_CASE("arc_first_forward_edge")
{
  ConvexPolygon2 polygon{{-4.02, 1.3}, {-5.9, 0.5}, {-3.66, -3.52}, {-2.22, -2.02}};
  PolygonInfo polygon_info(polygon);

  SECTION("Arc::lower")
  {
    ForwardEdge edge = arc_first_forward_edge<Arc::lower>(polygon_info);
    CHECK(*edge.end_it == polygon[2]);
    CHECK(edge.dir == polygon[2] - polygon[1]);
  }

  SECTION("Arc::upper")
  {
    ForwardEdge edge = arc_first_forward_edge<Arc::upper>(polygon_info);
    CHECK(*edge.end_it == polygon[0]);
    CHECK(edge.dir == polygon[0] - polygon[3]);
  }
}

TEST_CASE("arc_first_reverse_edge")
{
  ConvexPolygon2 polygon{{-1.3, 1.68}, {-3.72, 2.48}, {-6.26, 1.18}, {-7.56, -3.38}, {-4.44, -2.78}, {-1.82, -0.52}};
  PolygonInfo polygon_info(polygon);

  SECTION("Arc::lower")
  {
    ReverseEdge edge = arc_first_reverse_edge<Arc::lower>(polygon_info);
    CHECK(*edge.start_it == polygon[5]);
    CHECK(edge.dir == polygon[0] - polygon[5]);
  }

  SECTION("Arc::upper")
  {
    ReverseEdge edge = arc_first_reverse_edge<Arc::upper>(polygon_info);
    CHECK(*edge.start_it == polygon[2]);
    CHECK(edge.dir == polygon[3] - polygon[2]);
  }
}

TEST_CASE("forward_edge_for_sweep_position")
{
  ConvexPolygon2 polygon{{4, 4}, {2, 2}, {2, -1}, {4, -3}, {7, -3}, {9, -2}, {10, 0}, {9, 3}, {7, 5}};
  PolygonInfo polygon_info(polygon);

  SECTION("Arc::lower")
  {
    SECTION("General")
    {
      ForwardEdge edge =
          forward_edge_for_sweep_position<Arc::lower, PerturbationVector2::right_up>(polygon_info, ScalarDeg1(8));
      CHECK(*edge.end_it == polygon[5]);
      CHECK(edge.dir == polygon[5] - polygon[4]);
    }

    SECTION("Vertical line on vertex, perturb left_down")
    {
      ForwardEdge edge =
          forward_edge_for_sweep_position<Arc::lower, PerturbationVector2::left_down>(polygon_info, ScalarDeg1(4));
      CHECK(*edge.end_it == polygon[3]);
      CHECK(edge.dir == polygon[3] - polygon[2]);
    }

    SECTION("Vertical line on vertex, perturb right_up")
    {
      ForwardEdge edge =
          forward_edge_for_sweep_position<Arc::lower, PerturbationVector2::right_up>(polygon_info, ScalarDeg1(4));
      CHECK(*edge.end_it == polygon[4]);
      CHECK(edge.dir == polygon[4] - polygon[3]);
    }
  }

  SECTION("Arc::upper")
  {
    SECTION("General")
    {
      ForwardEdge edge =
          forward_edge_for_sweep_position<Arc::upper, PerturbationVector2::right_up>(polygon_info, ScalarDeg1(5));
      CHECK(*edge.end_it == polygon[0]);
      CHECK(edge.dir == polygon[0] - polygon[8]);
    }

    SECTION("Vertical line on vertex, perturb left_down")
    {
      ForwardEdge edge =
          forward_edge_for_sweep_position<Arc::upper, PerturbationVector2::left_down>(polygon_info, ScalarDeg1(4));
      CHECK(*edge.end_it == polygon[1]);
      CHECK(edge.dir == polygon[1] - polygon[0]);
    }

    SECTION("Vertical line on vertex, perturb right_up")
    {
      ForwardEdge edge =
          forward_edge_for_sweep_position<Arc::upper, PerturbationVector2::right_up>(polygon_info, ScalarDeg1(4));
      CHECK(*edge.end_it == polygon[0]);
      CHECK(edge.dir == polygon[0] - polygon[8]);
    }
  }
}

TEST_CASE("reverse_edge_for_sweep_position")
{
  ConvexPolygon2 polygon{{7, 4}, {5, 4}, {1, 3}, {-1, 1}, {-1, -2}, {1, -4}, {5, -5}, {8, -3}, {10, 0}, {9, 2}};
  PolygonInfo polygon_info(polygon);

  SECTION("Arc::lower")
  {
    SECTION("General")
    {
      ReverseEdge edge =
          reverse_edge_for_sweep_position<Arc::lower, PerturbationVector2::right_up>(polygon_info, ScalarDeg1(3));
      CHECK(*edge.start_it == polygon[5]);
      CHECK(edge.dir == polygon[6] - polygon[5]);
    }

    SECTION("Vertical line on vertex, perturb left_down")
    {
      ReverseEdge edge =
          reverse_edge_for_sweep_position<Arc::lower, PerturbationVector2::left_down>(polygon_info, ScalarDeg1(5));
      CHECK(*edge.start_it == polygon[5]);
      CHECK(edge.dir == polygon[6] - polygon[5]);
    }

    SECTION("Vertical line on vertex, perturb right_up")
    {
      ReverseEdge edge =
          reverse_edge_for_sweep_position<Arc::lower, PerturbationVector2::right_up>(polygon_info, ScalarDeg1(5));
      CHECK(*edge.start_it == polygon[6]);
      CHECK(edge.dir == polygon[7] - polygon[6]);
    }
  }

  SECTION("Arc::upper")
  {
    SECTION("General")
    {
      ReverseEdge edge =
          reverse_edge_for_sweep_position<Arc::upper, PerturbationVector2::right_up>(polygon_info, ScalarDeg1(6));
      CHECK(*edge.start_it == polygon[0]);
      CHECK(edge.dir == polygon[1] - polygon[0]);
    }

    SECTION("Vertical line on vertex, perturb left_down")
    {
      ReverseEdge edge =
          reverse_edge_for_sweep_position<Arc::upper, PerturbationVector2::left_down>(polygon_info, ScalarDeg1(1));
      CHECK(*edge.start_it == polygon[2]);
      CHECK(edge.dir == polygon[3] - polygon[2]);
    }

    SECTION("Vertical line on vertex, perturb right_up")
    {
      ReverseEdge edge =
          reverse_edge_for_sweep_position<Arc::upper, PerturbationVector2::right_up>(polygon_info, ScalarDeg1(1));
      CHECK(*edge.start_it == polygon[1]);
      CHECK(edge.dir == polygon[2] - polygon[1]);
    }
  }
}

TEST_CASE("advance_forward_edge")
{
  ConvexPolygon2 polygon{{5.26, -4.8}, {9.08, -4.24}, {11.54, -0.6}, {8.32, 3.4}, {1.16, -0.78}};
  PolygonInfo polygon_info(polygon);

  SECTION("Arc::lower")
  {
    ForwardEdge edge = arc_first_forward_edge<Arc::lower>(polygon_info);
    CHECK(*edge.end_it == polygon[0]);
    CHECK(edge.dir == polygon[0] - polygon[4]);

    CHECK(advance_forward_edge<Arc::lower>(polygon_info, edge));
    CHECK(*edge.end_it == polygon[1]);
    CHECK(edge.dir == polygon[1] - polygon[0]);

    CHECK(advance_forward_edge<Arc::lower>(polygon_info, edge));
    CHECK(*edge.end_it == polygon[2]);
    CHECK(edge.dir == polygon[2] - polygon[1]);

    CHECK_FALSE(advance_forward_edge<Arc::lower>(polygon_info, edge));
  }

  SECTION("Arc::upper")
  {
    ForwardEdge edge = arc_first_forward_edge<Arc::upper>(polygon_info);
    CHECK(*edge.end_it == polygon[3]);
    CHECK(edge.dir == polygon[3] - polygon[2]);

    CHECK(advance_forward_edge<Arc::upper>(polygon_info, edge));
    CHECK(*edge.end_it == polygon[4]);
    CHECK(edge.dir == polygon[4] - polygon[3]);

    CHECK_FALSE(advance_forward_edge<Arc::upper>(polygon_info, edge));
  }
}

TEST_CASE("advance_reverse_edge")
{
  ConvexPolygon2 polygon{{-3.98, 3.22}, {-3.5, 2.34}, {0.54, -0.3}, {4.48, 1.44},
                         {4.96, 5.14},  {0.66, 7.26}, {-1.62, 6.56}};
  PolygonInfo polygon_info(polygon);

  SECTION("Arc::lower")
  {
    ReverseEdge edge = arc_first_reverse_edge<Arc::lower>(polygon_info);
    CHECK(*edge.start_it == polygon[3]);
    CHECK(edge.dir == polygon[4] - polygon[3]);

    CHECK(advance_reverse_edge<Arc::lower>(polygon_info, edge));
    CHECK(*edge.start_it == polygon[2]);
    CHECK(edge.dir == polygon[3] - polygon[2]);

    CHECK(advance_reverse_edge<Arc::lower>(polygon_info, edge));
    CHECK(*edge.start_it == polygon[1]);
    CHECK(edge.dir == polygon[2] - polygon[1]);

    CHECK(advance_reverse_edge<Arc::lower>(polygon_info, edge));
    CHECK(*edge.start_it == polygon[0]);
    CHECK(edge.dir == polygon[1] - polygon[0]);

    CHECK_FALSE(advance_reverse_edge<Arc::lower>(polygon_info, edge));
  }

  SECTION("Arc::upper")
  {
    ReverseEdge edge = arc_first_reverse_edge<Arc::upper>(polygon_info);
    CHECK(*edge.start_it == polygon[6]);
    CHECK(edge.dir == polygon[7] - polygon[6]);

    CHECK(advance_reverse_edge<Arc::upper>(polygon_info, edge));
    CHECK(*edge.start_it == polygon[5]);
    CHECK(edge.dir == polygon[6] - polygon[5]);

    CHECK(advance_reverse_edge<Arc::upper>(polygon_info, edge));
    CHECK(*edge.start_it == polygon[4]);
    CHECK(edge.dir == polygon[5] - polygon[4]);

    CHECK_FALSE(advance_reverse_edge<Arc::upper>(polygon_info, edge));
  }
}

} // namespace convex_polygons_intersection

} // namespace dida::detail