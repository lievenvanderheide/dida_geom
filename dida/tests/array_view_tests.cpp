#include "dida/array_view.hpp"

#include <catch2/catch.hpp>

namespace dida
{

TEST_CASE("Construct empty ArrayView and access")
{
  ArrayView<int> view;
  const ArrayView<int>& view_const_ref = view;

  CHECK(view.size() == 0);
  CHECK(view.begin() == view.end());
  CHECK(view_const_ref.begin() == view_const_ref.end());
}

TEST_CASE("ArrayView<int> from std::vector<int>& and access")
{
  std::vector<int> vector{-831, -718, 591, 517, 647, 578, 74, -389, 303, 51, -721, -939};
  ArrayView<int> view(vector);
  const ArrayView<int> view_const_ref = view;

  CHECK(view.size() == vector.size());
  CHECK(std::equal(view.begin(), view.end(), vector.begin(), vector.end()));
  CHECK(std::equal(view_const_ref.begin(), view_const_ref.end(), vector.begin(), vector.end()));

  for (size_t i = 0; i < view.size(); i++)
  {
    CHECK(view[i] == vector[i]);
    CHECK(view_const_ref[i] == vector[i]);
  }
}

TEST_CASE("ArrayView<const int> from const std::vector<int>& and access")
{
  std::vector<int> vector{650, -199, 651, 869, -391, -31, 750, 550, 63, 126, -4, 817};
  ArrayView<const int> view(vector);
  const ArrayView<const int> view_const_ref = view;

  CHECK(view.size() == vector.size());
  CHECK(std::equal(view.begin(), view.end(), vector.begin(), vector.end()));
  CHECK(std::equal(view_const_ref.begin(), view_const_ref.end(), vector.begin(), vector.end()));

  for (size_t i = 0; i < view.size(); i++)
  {
    CHECK(view[i] == vector[i]);
    CHECK(view_const_ref[i] == vector[i]);
  }
}

} // namespace dida