#include "dida/detail/convex_polygons_intersection.hpp"

#include <catch2/catch.hpp>
#include <map>

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
    CHECK(edge.dir == polygon[0] - polygon[6]);

    CHECK(advance_reverse_edge<Arc::upper>(polygon_info, edge));
    CHECK(*edge.start_it == polygon[5]);
    CHECK(edge.dir == polygon[6] - polygon[5]);

    CHECK(advance_reverse_edge<Arc::upper>(polygon_info, edge));
    CHECK(*edge.start_it == polygon[4]);
    CHECK(edge.dir == polygon[5] - polygon[4]);

    CHECK_FALSE(advance_reverse_edge<Arc::upper>(polygon_info, edge));
  }
}

namespace
{

/// An implementation of the @c Callbacks interface, which checks that the expected crossing points are found.
class TestCallbacks
{
public:
  /// The flags for the @c find_expected_crossing_points function.
  struct FindExpectedCrossingPointsFlags
  {
    /// Whether crossing points between two edges which are both on the lower arc of their respective polygons should be
    /// included.
    bool include_on_lower_arc_points;

    /// Whether crossing points between two edges which are both on the upper arc of their respective polygons should be
    /// included.
    bool include_on_upper_arc_points;

    /// Whether a side crossing point which occurs at the beginning of the lower arc of the intersection polygon should
    /// be included.
    ///
    /// A side crossing point is one between an edge of the lower arc of one polygon and an edge of the upper arc of the
    /// other polygon.
    bool include_lower_arc_side_point;

    /// Whether a side crossing point which occurs at the end of the lower arc of the intersection polygon should
    /// be included.
    ///
    /// A side crossing point is one between an edge of the lower arc of one polygon and an edge of the upper arc of the
    /// other polygon.
    bool include_upper_arc_side_point;
  };

  /// Finds the expected crossing points and adds them to the @c expected_crossing_points_ map.
  ///
  /// @param a_info The @c PolygonInfo of the first polygon.
  /// @param b_info The @c PolygonInfo of the second polygon.
  /// @param flags The flags indicating which crossing points should be included.
  void find_expected_crossing_points(const PolygonInfo& a_info, const PolygonInfo& b_info,
                                     FindExpectedCrossingPointsFlags flags);

  /// The callback function which is called by the function under test when a crossing point is found. This
  /// implementation checks if the crossing point is in @c expected_crossing_points_, and if so, removes it from that
  /// map.
  ///
  /// @param a_edge The edge of the first input polygon.
  /// @param b_edge The edge of the second input polygon.
  /// @param s_num The numerator of the parameter of the crossing point on @c a_edge.
  /// @param t_num The numerator of the parameter of the crossing point on @c b_edge.
  /// @param denom The denominator of the 2 interpolation parameters.
  void crossing_point(const ForwardEdge& a_edge, const ForwardEdge& b_edge, ScalarDeg2 s_num, ScalarDeg2 t_num,
                      ScalarDeg2 denom);

  /// Returns whether all expected crossing points have been found, that is.
  ///
  /// @return True iff all expected crossing points have been found.
  bool all_expected_points_found();

private:
  /// Returns whether a crossing point between edges @c a_edge and @c b_edge should be included in the set of expected
  /// crossing points, given @c flags.
  ///
  /// @param a_edge The first edge.
  /// @param b_edge The second edge.
  /// @param flags The flags which indicate which crossing points should be included.
  static bool should_include(Segment2 a_edge, Segment2 b_edge, FindExpectedCrossingPointsFlags flags);

  struct CrossingPointKey
  {
    ConvexPolygonView2::const_iterator a_end_it;
    ConvexPolygonView2::const_iterator b_end_it;

    bool operator<(CrossingPointKey rhs) const;
  };

  using ExpectedCrossingPointsMap = std::map<CrossingPointKey, SegmentsCrossingPointParams>;
  ExpectedCrossingPointsMap expected_crossing_points_;
};

void TestCallbacks::find_expected_crossing_points(const PolygonInfo& a_info, const PolygonInfo& b_info,
                                                  FindExpectedCrossingPointsFlags flags)
{
  ConvexPolygonView2::const_iterator a_start_it = a_info.polygon.end() - 1;
  for (ConvexPolygonView2::const_iterator a_end_it = a_info.polygon.begin(); a_end_it != a_info.polygon.end();
       ++a_end_it)
  {
    ConvexPolygonView2::const_iterator b_start_it = b_info.polygon.end() - 1;
    for (ConvexPolygonView2::const_iterator b_end_it = b_info.polygon.begin(); b_end_it != b_info.polygon.end();
         ++b_end_it)
    {
      Segment2 a_edge(*a_start_it, *a_end_it);
      Segment2 b_edge(*b_start_it, *b_end_it);
      std::optional<SegmentsCrossingPointParams> crossing_point =
          crossing_point_with_perturbation<PerturbationVector2::right_up>(a_edge, b_edge);
      if (crossing_point && should_include(a_edge, b_edge, flags))
      {
        expected_crossing_points_.emplace(CrossingPointKey{a_end_it, b_end_it}, *crossing_point);
      }

      b_start_it = b_end_it;
    }

    a_start_it = a_end_it;
  }
}

bool TestCallbacks::should_include(Segment2 a_edge, Segment2 b_edge, FindExpectedCrossingPointsFlags flags)
{
  if (lex_less_than(a_edge.start(), a_edge.end()))
  {
    if (lex_less_than(b_edge.start(), b_edge.end()))
    {
      // Both edges are on the lower arc.
      return flags.include_on_lower_arc_points;
    }
    else
    {
      // Edge 'a' is on the lower arc, edge 'b' on the upper arc.
      Vector2 lower_dir = a_edge.end() - a_edge.start();
      Vector2 upper_dir = b_edge.end() - b_edge.start();
      if (cross(lower_dir, upper_dir) > 0)
      {
        return flags.include_lower_arc_side_point;
      }
      else
      {
        return flags.include_upper_arc_side_point;
      }
    }
  }
  else
  {
    if (lex_less_than(b_edge.start(), b_edge.end()))
    {
      // Edge 'a' is on the upper arc, edge 'b' on the lower arc.
      Vector2 upper_dir = a_edge.end() - a_edge.start();
      Vector2 lower_dir = b_edge.end() - b_edge.start();
      if (cross(lower_dir, upper_dir) > 0)
      {
        return flags.include_lower_arc_side_point;
      }
      else
      {
        return flags.include_upper_arc_side_point;
      }
    }
    else
    {
      // Both edges are on the upper arc.
      return flags.include_on_upper_arc_points;
    }
  }
}

void TestCallbacks::crossing_point(const ForwardEdge& a_edge, const ForwardEdge& b_edge, ScalarDeg2 s_num,
                                   ScalarDeg2 t_num, ScalarDeg2 denom)
{
  if (denom < 0)
  {
    s_num = -s_num;
    t_num = -t_num;
    denom = -denom;
  }

  s_num = denom - s_num;
  t_num = denom - t_num;

  SegmentsCrossingPointParams crossing_point(s_num, t_num, denom);
  ExpectedCrossingPointsMap::const_iterator expected_it =
      expected_crossing_points_.find(CrossingPointKey{a_edge.end_it, b_edge.end_it});
  REQUIRE(expected_it != expected_crossing_points_.end());

  CHECK(expected_it->second == crossing_point);
  expected_crossing_points_.erase(expected_it);
}

bool TestCallbacks::all_expected_points_found()
{
  return expected_crossing_points_.empty();
}

bool TestCallbacks::CrossingPointKey::operator<(CrossingPointKey rhs) const
{
  return a_end_it < rhs.a_end_it || (a_end_it == rhs.a_end_it && b_end_it < rhs.b_end_it);
}

} // namespace

namespace
{

/// Tests the @c find_on_arc_crossing_points. It's assumed that the first edge of the relevant arc of both @c a and @c b
/// can be passed as @c a_edge and @c b_edge to @c find_on_arc_crossing_points, that is, there must be a sweep position
/// for which both @c a_edge and @c b_edge intersect the sweep line.
///
/// @tparam arc The arc for which to find the "on arc" crossing points.
/// @param a The first input polygon.
/// @param b The second input polygon.
/// @param a_is_inner Whether the point of the first edge of @c a initially intersecting the sweep line is on the
/// interior side of edge @c b.
template <Arc arc>
void test_find_on_arc_crossing_points(ConvexPolygonView2 a, ConvexPolygonView2 b, bool a_is_inner)
{
  PolygonInfo a_info(a);
  PolygonInfo b_info(b);

  ForwardEdge a_edge = arc_first_forward_edge<arc>(a_info);
  ForwardEdge b_edge = arc_first_forward_edge<arc>(b_info);

  TestCallbacks::FindExpectedCrossingPointsFlags flags;
  flags.include_on_lower_arc_points = arc == Arc::lower;
  flags.include_on_upper_arc_points = arc == Arc::upper;
  flags.include_lower_arc_side_point = false;
  flags.include_upper_arc_side_point = false;

  TestCallbacks callbacks;
  callbacks.find_expected_crossing_points(a_info, b_info, flags);

  find_on_arc_crossing_points<arc>(a_info, a_edge, b_info, b_edge, a_is_inner, callbacks);
  CHECK(callbacks.all_expected_points_found());
}

void rotate_180_deg(ConvexPolygon2& polygon)
{
  for (Point2& vertex : polygon.unsafe_mutable_vertices())
  {
    vertex = Point2(-static_cast<Vector2>(vertex));
  }
}

} // namespace

TEST_CASE("find_on_arc_crossing_points")
{
  SECTION("General, crosses when advancing A")
  {
    ConvexPolygon2 a{{-2.96, 4.76}, {-1.7, 2.06}, {0.36, 0.62}, {2.18, -0.3},
                     {3.78, -0.16}, {5.84, 0.28}, {9.24, 3.12}};
    ConvexPolygon2 b{{-1.9, 3.7}, {2.54, -1.26}, {6.32, 0.9}, {9.38, 5.34}};

    SECTION("Lower arc")
    {
      test_find_on_arc_crossing_points<Arc::lower>(a, b, false);
    }

    SECTION("Upper arc")
    {
      rotate_180_deg(a);
      rotate_180_deg(b);
      test_find_on_arc_crossing_points<Arc::upper>(a, b, false);
    }
  }

  SECTION("General, crosses when advancing B")
  {
    ConvexPolygon2 a{{10.04, 1.66}, {-0.26, 3.52}, {3.18, -3.82}};
    ConvexPolygon2 b{{-2.76, 5.06}, {0.72, 0.18}, {2.78, -2.28}, {5.2, -1.66}, {9.58, 0.86}};

    SECTION("Lower arc")
    {
      test_find_on_arc_crossing_points<Arc::lower>(a, b, true);
    }

    SECTION("Upper arc")
    {
      rotate_180_deg(a);
      rotate_180_deg(b);
      test_find_on_arc_crossing_points<Arc::upper>(a, b, true);
    }
  }

  SECTION("A vertices on B")
  {
    ConvexPolygon2 a{{-1.35, 2.65}, {-1, 1}, {5, -4}, {10, -6}, {14, -5}, {16, 0}, {16.35, 4.4}};
    ConvexPolygon2 b{{-2, 3}, {1, -3}, {9, -5}, {15, -5}, {17, 5}};

    SECTION("Lower arc")
    {
      test_find_on_arc_crossing_points<Arc::lower>(a, b, true);
    }

    SECTION("Upper arc")
    {
      rotate_180_deg(a);
      rotate_180_deg(b);
      test_find_on_arc_crossing_points<Arc::upper>(a, b, true);
    }
  }

  SECTION("B vertices on A")
  {
    ConvexPolygon2 a{{4, 8}, {6, 4}, {12, 2}, {22, 4}, {26, 10}};
    ConvexPolygon2 b{{4.8, 7.68}, {5, 6}, {9, 3}, {13, 1}, {17, 3}, {24, 7}, {24.78, 9.38}};

    SECTION("Lower arc")
    {
      test_find_on_arc_crossing_points<Arc::lower>(a, b, false);
    }

    SECTION("Upper arc")
    {
      rotate_180_deg(a);
      rotate_180_deg(b);
      test_find_on_arc_crossing_points<Arc::upper>(a, b, false);
    }
  }

  SECTION("Two equal squares")
  {
    ConvexPolygon2 a{{5, 7}, {2, 7}, {2, 4}, {5, 4}};
    ConvexPolygon2 b = a;

    SECTION("Lower arc")
    {
      test_find_on_arc_crossing_points<Arc::lower>(a, b, false);
    }

    SECTION("Upper arc")
    {
      rotate_180_deg(a);
      rotate_180_deg(b);
      test_find_on_arc_crossing_points<Arc::upper>(a, b, true);
    }
  }
}

} // namespace convex_polygons_intersection

} // namespace dida::detail