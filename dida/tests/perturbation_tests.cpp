#include "dida/perturbation.hpp"

#include <catch2/catch.hpp>

namespace dida
{

TEST_CASE("dot_is_positive/dot_is_negative(Vector2, PerturbationVector2)")
{
  SECTION("Left")
  {
    Vector2 a(-1251, 636);
    CHECK(dot_is_positive(a, PerturbationVector2::left_down));
    CHECK_FALSE(dot_is_positive(a, PerturbationVector2::right_up));
    CHECK_FALSE(dot_is_negative(a, PerturbationVector2::left_down));
    CHECK(dot_is_negative(a, PerturbationVector2::right_up));
  }

  SECTION("Right")
  {
    Vector2 a(1251, -636);
    CHECK_FALSE(dot_is_positive(a, PerturbationVector2::left_down));
    CHECK(dot_is_positive(a, PerturbationVector2::right_up));
    CHECK(dot_is_negative(a, PerturbationVector2::left_down));
    CHECK_FALSE(dot_is_negative(a, PerturbationVector2::right_up));
  }

  SECTION("Down")
  {
    Vector2 a(0, -191);
    CHECK(dot_is_positive(a, PerturbationVector2::left_down));
    CHECK_FALSE(dot_is_positive(a, PerturbationVector2::right_up));
    CHECK_FALSE(dot_is_negative(a, PerturbationVector2::left_down));
    CHECK(dot_is_negative(a, PerturbationVector2::right_up));
  }

  SECTION("Up")
  {
    Vector2 a(0, 191);
    CHECK_FALSE(dot_is_positive(a, PerturbationVector2::left_down));
    CHECK(dot_is_positive(a, PerturbationVector2::right_up));
    CHECK(dot_is_negative(a, PerturbationVector2::left_down));
    CHECK_FALSE(dot_is_negative(a, PerturbationVector2::right_up));
  }

  SECTION("Null vector")
  {
    Vector2 a(0, 0);
    CHECK_FALSE(dot_is_positive(a, PerturbationVector2::left_down));
    CHECK_FALSE(dot_is_positive(a, PerturbationVector2::right_up));
    CHECK_FALSE(dot_is_negative(a, PerturbationVector2::left_down));
    CHECK_FALSE(dot_is_negative(a, PerturbationVector2::right_up));
  }
}

TEST_CASE("cross_is_positive/cross_is_negative(Vector2, PerturbationVector2")
{
  SECTION("Left")
  {
    Vector2 a(-1251, 0);
    CHECK(cross_is_positive(a, PerturbationVector2::left_down));
    CHECK_FALSE(cross_is_positive(a, PerturbationVector2::right_up));
    CHECK_FALSE(cross_is_negative(a, PerturbationVector2::left_down));
    CHECK(cross_is_negative(a, PerturbationVector2::right_up));
  }

  SECTION("Right")
  {
    Vector2 a(1251, 0);
    CHECK_FALSE(cross_is_positive(a, PerturbationVector2::left_down));
    CHECK(cross_is_positive(a, PerturbationVector2::right_up));
    CHECK(cross_is_negative(a, PerturbationVector2::left_down));
    CHECK_FALSE(cross_is_negative(a, PerturbationVector2::right_up));
  }

  SECTION("Down")
  {
    Vector2 a(735, -191);
    CHECK_FALSE(cross_is_positive(a, PerturbationVector2::left_down));
    CHECK(cross_is_positive(a, PerturbationVector2::right_up));
    CHECK(cross_is_negative(a, PerturbationVector2::left_down));
    CHECK_FALSE(cross_is_negative(a, PerturbationVector2::right_up));
  }

  SECTION("Up")
  {
    Vector2 a(-735, 191);
    CHECK(cross_is_positive(a, PerturbationVector2::left_down));
    CHECK_FALSE(cross_is_positive(a, PerturbationVector2::right_up));
    CHECK_FALSE(cross_is_negative(a, PerturbationVector2::left_down));
    CHECK(cross_is_negative(a, PerturbationVector2::right_up));
  }

  SECTION("Null vector")
  {
    Vector2 a(0, 0);
    CHECK_FALSE(cross_is_positive(a, PerturbationVector2::left_down));
    CHECK_FALSE(cross_is_positive(a, PerturbationVector2::right_up));
    CHECK_FALSE(cross_is_negative(a, PerturbationVector2::left_down));
    CHECK_FALSE(cross_is_negative(a, PerturbationVector2::right_up));
  }
}

} // namespace dida