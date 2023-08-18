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

  REQUIRE(view.size() == vector.size());
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

  REQUIRE(view.size() == vector.size());
  CHECK(std::equal(view.begin(), view.end(), vector.begin(), vector.end()));
  CHECK(std::equal(view_const_ref.begin(), view_const_ref.end(), vector.begin(), vector.end()));

  for (size_t i = 0; i < view.size(); i++)
  {
    CHECK(view[i] == vector[i]);
    CHECK(view_const_ref[i] == vector[i]);
  }
}

TEST_CASE("ArrayView<std::string> from std::array<std::string>& and access")
{
  std::array<std::string, 3> array{"foo", "bar", "baz"};
  ArrayView<std::string> view(array);
  const ArrayView<std::string> view_const_ref = view;
  
  REQUIRE(view.size() == array.size());
  CHECK(std::equal(view.begin(), view.end(), array.begin(), array.end()));
  CHECK(std::equal(view_const_ref.begin(), view_const_ref.end(), array.begin(), array.end()));

  for (size_t i = 0; i < view.size(); i++)
  {
    CHECK(view[i] == array[i]);
    CHECK(view_const_ref[i] == array[i]);
  }
}

TEST_CASE("ArrayView<const std::string> from const std::array<std::string>& and access")
{
  std::array<std::string, 3> array{"foo", "bar", "baz"};
  ArrayView<const std::string> view(array);
  const ArrayView<const std::string> view_const_ref = view;
  
  REQUIRE(view.size() == array.size());
  CHECK(std::equal(view.begin(), view.end(), array.begin(), array.end()));
  CHECK(std::equal(view_const_ref.begin(), view_const_ref.end(), array.begin(), array.end()));

  for (size_t i = 0; i < view.size(); i++)
  {
    CHECK(view[i] == array[i]);
    CHECK(view_const_ref[i] == array[i]);
  }
}

TEST_CASE("ArrayView<int> with pointer and size")
{
  int data[]{650, -199, 651, 869, -391, -31, 750, 550, 63, 126, -4, 817};
  size_t size = sizeof(data) / sizeof(int);
  ArrayView<int> view(data, size);

  REQUIRE(view.size() == size);
  CHECK(std::equal(view.begin(), view.end(), data, data + size));

  for (size_t i = 0; i < view.size(); i++)
  {
    CHECK(view[i] == data[i]);
  }
}

} // namespace dida