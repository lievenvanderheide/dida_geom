#include "dida/segment2.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_all.hpp>
#include <sstream>

namespace dida
{

TEST_CASE("Segment2 construction and access")
{
  Segment2 segment({-7.20, 4.26}, {-2.78, 7.38});
  CHECK(segment.start() == Point2(-7.20, 4.26));
  CHECK(segment.end() == Point2(-2.78, 7.38));
}

TEST_CASE("Segment2::unsafe_from_endpoints and access")
{
  Segment2 segment = Segment2::unsafe_from_endpoints({-7.20, 4.26}, {-2.78, 7.38});
  CHECK(segment.start() == Point2(-7.20, 4.26));
  CHECK(segment.end() == Point2(-2.78, 7.38));
}

TEST_CASE("Segment2::direction()")
{
  Segment2 segment({-3, -2}, {2, -1});
  CHECK(segment.direction() == Vector2(5, 1));
}

TEST_CASE("Segment2::operator==/operator!=")
{
  SECTION("Equal")
  {
    Segment2 a({-3.50, 0.66}, {3.22, -3.14});
    Segment2 b({-3.50, 0.66}, {3.22, -3.14});
    CHECK(a == b);
    CHECK_FALSE(a != b);
  }

  SECTION("Start points different")
  {
    Segment2 a({-3.50, 0.66}, {3.22, -3.14});
    Segment2 b({0.74, 1.12}, {3.22, -3.14});
    CHECK_FALSE(a == b);
    CHECK(a != b);
  }
  
  SECTION("End points different")
  {
    Segment2 a({-3.50, 0.66}, {3.22, -3.14});
    Segment2 b({-3.50, 0.66}, {-1.30, 1.28});
    CHECK_FALSE(a == b);
    CHECK(a != b);
  }
}

TEST_CASE("SegmentsCrossingPointParams construction and access")
{
  SegmentsCrossingPointParams params(ScalarDeg2(236.836), ScalarDeg2(6836.12), ScalarDeg2(13618.33));
  CHECK(params.s_num() == ScalarDeg2(236.836));
  CHECK(params.t_num() == ScalarDeg2(6836.12));
  CHECK(params.denom() == ScalarDeg2(13618.33));
}

TEST_CASE("SegmentsCrossingPointParams::unsafe_from_endpoints")
{
  SegmentsCrossingPointParams params(ScalarDeg2(98.47), ScalarDeg2(467.62), ScalarDeg2(806.01));
  CHECK(params.s_num() == ScalarDeg2(98.47));
  CHECK(params.t_num() == ScalarDeg2(467.62));
  CHECK(params.denom() == ScalarDeg2(806.01));
}

TEST_CASE("SegmentsCrossingPointParams comparison")
{
  SECTION("Equal")
  {
    SegmentsCrossingPointParams a(ScalarDeg2(3 * 35144), ScalarDeg2(3 * 76129), ScalarDeg2(3 * 97292));
    SegmentsCrossingPointParams b(ScalarDeg2(5 * 35144), ScalarDeg2(5 * 76129), ScalarDeg2(5 * 97292));
    CHECK(a == b);
    CHECK_FALSE(a != b);
  }

  SECTION("X different")
  {
    SegmentsCrossingPointParams a(ScalarDeg2(3 * 35144), ScalarDeg2(3 * 76129), ScalarDeg2(3 * 97292));
    SegmentsCrossingPointParams b(ScalarDeg2(5 * 28644), ScalarDeg2(5 * 76129), ScalarDeg2(5 * 97292));
    CHECK_FALSE(a == b);
    CHECK(a != b);
  }

  SECTION("Y different")
  {
    SegmentsCrossingPointParams a(ScalarDeg2(3 * 35144), ScalarDeg2(3 * 76129), ScalarDeg2(3 * 97292));
    SegmentsCrossingPointParams b(ScalarDeg2(5 * 35144), ScalarDeg2(5 * 24684), ScalarDeg2(5 * 97292));
    CHECK_FALSE(a == b);
    CHECK(a != b);
  }
}

TEST_CASE("crossing_point_with_perturbation")
{
  SECTION("Crossing")
  {
    Segment2 a({2, 1}, {12, 6});
    Segment2 b({3, 6}, {7, 2});
    SegmentsCrossingPointParams ab_expected_crossing_point(ScalarDeg2(40), ScalarDeg2(75), ScalarDeg2(100));
    SegmentsCrossingPointParams ba_expected_crossing_point(ScalarDeg2(75), ScalarDeg2(40), ScalarDeg2(100));

    SECTION("a, b + left_down")
    {
      std::optional<SegmentsCrossingPointParams> params =
          crossing_point_with_perturbation<PerturbationVector2::left_down>(a, b);
      REQUIRE(params);
      CHECK(params->denom() > 0);
      CHECK(params == ab_expected_crossing_point);
    }

    SECTION("a, b + right_up")
    {
      std::optional<SegmentsCrossingPointParams> params =
          crossing_point_with_perturbation<PerturbationVector2::right_up>(a, b);
      REQUIRE(params);
      CHECK(params->denom() > 0);
      CHECK(params == ab_expected_crossing_point);
    }

    SECTION("b, a + left_down")
    {
      std::optional<SegmentsCrossingPointParams> params =
          crossing_point_with_perturbation<PerturbationVector2::left_down>(b, a);
      REQUIRE(params);
      CHECK(params->denom() > 0);
      CHECK(params == ba_expected_crossing_point);
    }

    SECTION("b, a + right_up")
    {
      std::optional<SegmentsCrossingPointParams> params =
          crossing_point_with_perturbation<PerturbationVector2::right_up>(b, a);
      REQUIRE(params);
      CHECK(params->denom() > 0);
      CHECK(params == ba_expected_crossing_point);
    }
  }

  SECTION("Disjoint")
  {
    Segment2 a = GENERATE(Segment2({-4, 2}, {-8, -3}), Segment2({-8, -3}, {-4, 2}));
    Segment2 b = GENERATE(Segment2({-3, -1}, {-6, -2}), Segment2({-3, -1}, {-6, -2}));
    CHECK(!crossing_point_with_perturbation<PerturbationVector2::left_down>(a, b));
    CHECK(!crossing_point_with_perturbation<PerturbationVector2::right_up>(a, b));
    CHECK(!crossing_point_with_perturbation<PerturbationVector2::left_down>(b, a));
    CHECK(!crossing_point_with_perturbation<PerturbationVector2::right_up>(b, a));
  }

  SECTION("A vertex on b, intersection after perturbation")
  {
    SECTION("A start vertex")
    {
      Segment2 a({6, 3}, {11, 2});
      Segment2 b({2, 1}, {12, 6});

      SECTION("a, b")
      {
        std::optional<SegmentsCrossingPointParams> params =
            crossing_point_with_perturbation<PerturbationVector2::right_up>(a, b);
        REQUIRE(params);
        CHECK(params->denom() > 0);
        CHECK(params == SegmentsCrossingPointParams(ScalarDeg2(0), ScalarDeg2(4), ScalarDeg2(10)));
      }

      SECTION("b, a")
      {
        std::optional<SegmentsCrossingPointParams> params =
            crossing_point_with_perturbation<PerturbationVector2::left_down>(b, a);
        REQUIRE(params);
        CHECK(params->denom() > 0);
        CHECK(params == SegmentsCrossingPointParams(ScalarDeg2(4), ScalarDeg2(0), ScalarDeg2(10)));
      }
    }

    SECTION("A end vertex")
    {
      Segment2 a({11, 2}, {6, 3});
      Segment2 b({2, 1}, {12, 6});

      SECTION("a, b")
      {
        std::optional<SegmentsCrossingPointParams> params =
            crossing_point_with_perturbation<PerturbationVector2::right_up>(a, b);
        REQUIRE(params);
        CHECK(params->denom() > 0);
        CHECK(params == SegmentsCrossingPointParams(ScalarDeg2(10), ScalarDeg2(4), ScalarDeg2(10)));
      }

      SECTION("b, a")
      {
        std::optional<SegmentsCrossingPointParams> params =
            crossing_point_with_perturbation<PerturbationVector2::left_down>(b, a);
        REQUIRE(params);
        CHECK(params->denom() > 0);
        CHECK(params == SegmentsCrossingPointParams(ScalarDeg2(4), ScalarDeg2(10), ScalarDeg2(10)));
      }
    }
  }

  SECTION("A vertex on b, no intersection after perturbation")
  {
    Segment2 a = GENERATE(Segment2({3, 6}, {6, 3}), Segment2({6, 3}, {3, 6}));
    Segment2 b = GENERATE(Segment2({2, 1}, {12, 6}), Segment2({12, 6}, {2, 1}));
    CHECK(!crossing_point_with_perturbation<PerturbationVector2::right_up>(a, b));
    CHECK(!crossing_point_with_perturbation<PerturbationVector2::left_down>(b, a));
  }

  SECTION("B vertex on a, intersection after perturbation")
  {
    SECTION("B start vertex")
    {
      Segment2 a({-2, 3}, {3, -7});
      Segment2 b({2, -5}, {-2, -5});

      SECTION("a, b")
      {
        std::optional<SegmentsCrossingPointParams> params =
            crossing_point_with_perturbation<PerturbationVector2::right_up>(a, b);
        REQUIRE(params);
        CHECK(params->denom() > 0);
        CHECK(params == SegmentsCrossingPointParams(ScalarDeg2(8), ScalarDeg2(0), ScalarDeg2(10)));
      }

      SECTION("b, a")
      {
        std::optional<SegmentsCrossingPointParams> params =
            crossing_point_with_perturbation<PerturbationVector2::left_down>(b, a);
        REQUIRE(params);
        CHECK(params->denom() > 0);
        CHECK(params == SegmentsCrossingPointParams(ScalarDeg2(0), ScalarDeg2(8), ScalarDeg2(10)));
      }
    }

    SECTION("B end vertex")
    {
      Segment2 a({-2, 3}, {3, -7});
      Segment2 b({-2, -5}, {2, -5});

      SECTION("a, b")
      {
        std::optional<SegmentsCrossingPointParams> params =
            crossing_point_with_perturbation<PerturbationVector2::right_up>(a, b);
        REQUIRE(params);
        CHECK(params->denom() > 0);
        CHECK(params == SegmentsCrossingPointParams(ScalarDeg2(8), ScalarDeg2(10), ScalarDeg2(10)));
      }

      SECTION("b, a")
      {
        std::optional<SegmentsCrossingPointParams> params =
            crossing_point_with_perturbation<PerturbationVector2::left_down>(b, a);
        REQUIRE(params);
        CHECK(params->denom() > 0);
        CHECK(params == SegmentsCrossingPointParams(ScalarDeg2(10), ScalarDeg2(8), ScalarDeg2(10)));
      }
    }
  }

  SECTION("B vertex on a, disjoint after perturbation")
  {
    Segment2 a = GENERATE(Segment2({5, 4}, {2, -2}), Segment2({2, -2}, {5, 4}));
    Segment2 b = GENERATE(Segment2({4, 2}, {8, 3}), Segment2({8, 3}, {4, 2}));
    CHECK(!crossing_point_with_perturbation<PerturbationVector2::right_up>(a, b));
    CHECK(!crossing_point_with_perturbation<PerturbationVector2::left_down>(b, a));
  }

  SECTION("Colinear")
  {
    Segment2 a = GENERATE(Segment2({-6, 5}, {6, 1}), Segment2({6, 1}, {-6, 5}));
    Segment2 b = GENERATE(Segment2({-3, 4}, {9, 0}), Segment2({9, 0}, {-3, 4}));
    CHECK(!crossing_point_with_perturbation<PerturbationVector2::right_up>(a, b));
    CHECK(!crossing_point_with_perturbation<PerturbationVector2::left_down>(a, b));
    CHECK(!crossing_point_with_perturbation<PerturbationVector2::right_up>(b, a));
    CHECK(!crossing_point_with_perturbation<PerturbationVector2::left_down>(b, a));
  }
}

TEST_CASE("Segment2 printing")
{
  std::stringstream s;
  s << Segment2({936, -18}, {-716, 339});
  CHECK(s.str() == "{{936, -18}, {-716, 339}}");
}

} // namespace dida