#include "dida/predicates.hpp"

#include <catch2/benchmark/catch_benchmark_all.hpp>
#include <catch2/catch_test_macros.hpp>

#include "stock_objects.hpp"

namespace dida::benchmarks
{

TEST_CASE("is_within benchmark")
{
  PolygonView2 netherlands = stock_objects::netherlands();
  Point2 point(8.82425, 54.9059);

  BENCHMARK("is_within netherlands polygon")
  {
    return is_within(netherlands, point);
  };

  BENCHMARK("is_within_with_perturbation netherlands polygon")
  {
    return is_within_with_perturbation<PerturbationVector2::right_up>(netherlands, point);
  };
}

} // namespace dida::benchmarks