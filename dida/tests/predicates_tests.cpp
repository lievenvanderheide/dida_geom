#include <catch2/catch.hpp>

#include "dida/predicates.hpp"

namespace dida::tests
{

TEST_CASE("is_within_with_perturbation(PolygonView2, Point2)")
{
  Polygon2 polygon{
      {-5, 2}, {-4, 5}, {-2, 3}, {2, 3},  {3, 5},  {5, 1},  {7, 5},  {6, 8},
      {4, 7},  {2, 9},  {1, 7},  {-2, 7}, {-3, 5}, {-5, 9}, {-6, 5},
  };

  SECTION("Point inside, general cases")
  {
    CHECK(is_within(polygon, Point2(-4.34, 5.98)));
    CHECK(is_within_with_perturbation<PerturbationVector2::left_down>(polygon, Point2(-4.34, 5.98)));
    CHECK(is_within_with_perturbation<PerturbationVector2::right_up>(polygon, Point2(-4.34, 5.98)));

    CHECK(is_within(polygon, Point2(-1.16, 5.90)));
    CHECK(is_within_with_perturbation<PerturbationVector2::left_down>(polygon, Point2(-1.16, 5.90)));
    CHECK(is_within_with_perturbation<PerturbationVector2::right_up>(polygon, Point2(-1.16, 5.90)));

    CHECK(is_within(polygon, Point2(4.74, 2.72)));
    CHECK(is_within_with_perturbation<PerturbationVector2::left_down>(polygon, Point2(4.74, 2.72)));
    CHECK(is_within_with_perturbation<PerturbationVector2::right_up>(polygon, Point2(4.74, 2.72)));

    CHECK(is_within(polygon, Point2(2.12, 8.08)));
    CHECK(is_within_with_perturbation<PerturbationVector2::left_down>(polygon, Point2(2.12, 8.08)));
    CHECK(is_within_with_perturbation<PerturbationVector2::right_up>(polygon, Point2(2.12, 8.08)));
  }

  SECTION("Point outside, general cases")
  {
    CHECK_FALSE(is_within(polygon, Point2(-6.72, 5.14)));
    CHECK_FALSE(is_within_with_perturbation<PerturbationVector2::left_down>(polygon, Point2(-6.72, 5.14)));
    CHECK_FALSE(is_within_with_perturbation<PerturbationVector2::right_up>(polygon, Point2(-6.72, 5.14)));

    CHECK_FALSE(is_within(polygon, Point2(-2.30, 7.90)));
    CHECK_FALSE(is_within_with_perturbation<PerturbationVector2::left_down>(polygon, Point2(-2.30, 7.90)));
    CHECK_FALSE(is_within_with_perturbation<PerturbationVector2::right_up>(polygon, Point2(-2.30, 7.90)));

    CHECK_FALSE(is_within(polygon, Point2(7.08, 4.36)));
    CHECK_FALSE(is_within_with_perturbation<PerturbationVector2::left_down>(polygon, Point2(7.08, 4.36)));
    CHECK_FALSE(is_within_with_perturbation<PerturbationVector2::right_up>(polygon, Point2(7.08, 4.36)));

    CHECK_FALSE(is_within(polygon, Point2(3.06, 3.94)));
    CHECK_FALSE(is_within_with_perturbation<PerturbationVector2::left_down>(polygon, Point2(3.06, 3.94)));
    CHECK_FALSE(is_within_with_perturbation<PerturbationVector2::right_up>(polygon, Point2(3.06, 3.94)));
  }

  SECTION("Ray intersects vertices")
  {
    CHECK_FALSE(is_within(polygon, Point2(-4, 9)));
    CHECK_FALSE(is_within_with_perturbation<PerturbationVector2::left_down>(polygon, Point2(-4, 9)));
    CHECK_FALSE(is_within_with_perturbation<PerturbationVector2::right_up>(polygon, Point2(-4, 9)));

    CHECK_FALSE(is_within(polygon, Point2(-4, 2)));
    CHECK_FALSE(is_within_with_perturbation<PerturbationVector2::left_down>(polygon, Point2(-4, 2)));
    CHECK_FALSE(is_within_with_perturbation<PerturbationVector2::right_up>(polygon, Point2(-4, 2)));

    CHECK(is_within(polygon, Point2(-2, 5)));
    CHECK(is_within_with_perturbation<PerturbationVector2::left_down>(polygon, Point2(-2, 5)));
    CHECK(is_within_with_perturbation<PerturbationVector2::right_up>(polygon, Point2(-2, 5)));

    CHECK(is_within(polygon, Point2(4, 5)));
    CHECK(is_within_with_perturbation<PerturbationVector2::left_down>(polygon, Point2(4, 5)));
    CHECK(is_within_with_perturbation<PerturbationVector2::right_up>(polygon, Point2(4, 5)));
  }

  SECTION("Ray intersects horizontal edges")
  {
    CHECK_FALSE(is_within(polygon, Point2(-4, 3)));
    CHECK_FALSE(is_within_with_perturbation<PerturbationVector2::left_down>(polygon, Point2(-4, 3)));
    CHECK_FALSE(is_within_with_perturbation<PerturbationVector2::right_up>(polygon, Point2(-4, 3)));

    CHECK_FALSE(is_within(polygon, Point2(3, 3)));
    CHECK_FALSE(is_within_with_perturbation<PerturbationVector2::left_down>(polygon, Point2(3, 3)));
    CHECK_FALSE(is_within_with_perturbation<PerturbationVector2::right_up>(polygon, Point2(3, 3)));

    CHECK(is_within(polygon, Point2(2, 7)));
    CHECK(is_within_with_perturbation<PerturbationVector2::left_down>(polygon, Point2(2, 7)));
    CHECK(is_within_with_perturbation<PerturbationVector2::right_up>(polygon, Point2(2, 7)));

    CHECK_FALSE(is_within(polygon, Point2(-3, 7)));
    CHECK_FALSE(is_within_with_perturbation<PerturbationVector2::left_down>(polygon, Point2(-3, 7)));
    CHECK_FALSE(is_within_with_perturbation<PerturbationVector2::right_up>(polygon, Point2(-3, 7)));
  }

  SECTION("Point on edge")
  {
    CHECK(is_within(polygon, Point2(-2.5, 6)));
    CHECK_FALSE(is_within_with_perturbation<PerturbationVector2::left_down>(polygon, Point2(-2.5, 6)));
    CHECK(is_within_with_perturbation<PerturbationVector2::right_up>(polygon, Point2(-2.5, 6)));

    CHECK(is_within(polygon, Point2(3, 8)));
    CHECK(is_within_with_perturbation<PerturbationVector2::left_down>(polygon, Point2(3, 8)));
    CHECK_FALSE(is_within_with_perturbation<PerturbationVector2::right_up>(polygon, Point2(3, 8)));

    CHECK(is_within(polygon, Point2(-1, 3)));
    CHECK_FALSE(is_within_with_perturbation<PerturbationVector2::left_down>(polygon, Point2(-1, 3)));
    CHECK(is_within_with_perturbation<PerturbationVector2::right_up>(polygon, Point2(-1, 3)));

    CHECK(is_within(polygon, Point2(-1, 7)));
    CHECK(is_within_with_perturbation<PerturbationVector2::left_down>(polygon, Point2(-1, 7)));
    CHECK_FALSE(is_within_with_perturbation<PerturbationVector2::right_up>(polygon, Point2(-1, 7)));
  }

  SECTION("Unperturbed point on vertex")
  {
    CHECK(is_within(polygon, Point2(-6, 5)));
    CHECK_FALSE(is_within_with_perturbation<PerturbationVector2::left_down>(polygon, Point2(-6, 5)));
    CHECK(is_within_with_perturbation<PerturbationVector2::right_up>(polygon, Point2(-5, 5)));

    CHECK(is_within(polygon, Point2(-4, 5)));
    CHECK(is_within_with_perturbation<PerturbationVector2::left_down>(polygon, Point2(-4, 5)));
    CHECK(is_within_with_perturbation<PerturbationVector2::right_up>(polygon, Point2(-4, 5)));

    CHECK(is_within(polygon, Point2(3, 5)));
    CHECK(is_within_with_perturbation<PerturbationVector2::left_down>(polygon, Point2(3, 5)));
    CHECK(is_within_with_perturbation<PerturbationVector2::right_up>(polygon, Point2(3, 5)));

    CHECK(is_within(polygon, Point2(7, 5)));
    CHECK(is_within_with_perturbation<PerturbationVector2::left_down>(polygon, Point2(7, 5)));
    CHECK_FALSE(is_within_with_perturbation<PerturbationVector2::right_up>(polygon, Point2(7, 5)));

    CHECK(is_within(polygon, Point2(-5, 2)));
    CHECK_FALSE(is_within_with_perturbation<PerturbationVector2::left_down>(polygon, Point2(-5, 2)));
    CHECK_FALSE(is_within_with_perturbation<PerturbationVector2::right_up>(polygon, Point2(-5, 2)));

    CHECK(is_within(polygon, Point2(6, 8)));
    CHECK_FALSE(is_within_with_perturbation<PerturbationVector2::left_down>(polygon, Point2(6, 8)));
    CHECK_FALSE(is_within_with_perturbation<PerturbationVector2::right_up>(polygon, Point2(6, 8)));
  }
}

} // namespace dida::tests