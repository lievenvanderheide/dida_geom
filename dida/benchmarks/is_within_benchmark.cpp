#define CATCH_CONFIG_MAIN
#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch2/catch.hpp>

#include "dida/benchmarks/stock_objects.hpp"
#include "dida/predicates.hpp"

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