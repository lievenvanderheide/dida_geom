#include "dida/utils.hpp"

#include <catch2/catch_test_macros.hpp>

namespace dida
{

TEST_CASE("prev_cyclic")
{
  std::vector<int> elements{5, 35, 73, 89};
  std::vector<int>::const_iterator it = elements.end() - 1;
  for (size_t j = 0; j < 2; j++)
  {
    for (size_t i = 0; i < elements.size(); i++)
    {
      CHECK(*it == elements[elements.size() - i - 1]);
      it = prev_cyclic(elements, it);
    }
  }
}

TEST_CASE("next_cyclic")
{
  std::vector<int> elements{85, 30, 5, 1};
  std::vector<int>::const_iterator it = elements.begin();
  for (size_t j = 0; j < 2; j++)
  {
    for (size_t i = 0; i < elements.size(); i++)
    {
      CHECK(*it == elements[i]);
      it = next_cyclic(elements, it);
    }
  }
}

TEST_CASE("distance_cyclic")
{
  std::vector<int> elements{7, 90, 94, 95};
  for (std::vector<int>::iterator a = elements.begin(); a != elements.end(); ++a)
  {
    std::vector<int>::iterator b = a;
    for (size_t expected_distance = 0; expected_distance < elements.size(); expected_distance++)
    {
      size_t distance = distance_cyclic(elements, a, b);
      b = next_cyclic(elements, b);
    }
  }
}

TEST_CASE("pred_modulo")
{
  int32_t a = 4;
  for (int32_t j = 0; j < 2; j++)
  {
    for (int32_t i = 4; i >= 0; i--)
    {
      CHECK(a == i);
      a = pred_modulo(a, 5);
    }
  }
}

TEST_CASE("succ_modulo")
{
  int32_t a = 0;
  for (int32_t j = 0; j < 2; j++)
  {
    for (int32_t i = 0; i < 5; i++)
    {
      CHECK(a == i);
      a = succ_modulo(a, 5);
    }
  }
}

TEST_CASE("add_modulo")
{
  size_t m = 7;
  for(size_t i = 0; i < m; i++)
  {
    for(size_t j = 0; j <= m; j++)
    {
      CHECK(add_modulo(i, j, m) == (i + j) % m);
    }
  }
}

TEST_CASE("sub_modulo")
{
  size_t m = 7;
  for(size_t i = 0; i < m; i++)
  {
    for(size_t j = 0; j <= m; j++)
    {
      CHECK(sub_modulo(i, j, m) == (m + i - j) % m);
    }
  }
}

TEST_CASE("cyclic_order")
{
  SECTION("All distinct")
  {
    CHECK(cyclic_less_than(1, 2, 3));
    CHECK(cyclic_less_than(2, 3, 1));
    CHECK(cyclic_less_than(3, 1, 2));

    CHECK_FALSE(cyclic_less_than(2, 1, 3));
    CHECK_FALSE(cyclic_less_than(3, 2, 1));
    CHECK_FALSE(cyclic_less_than(1, 3, 2));
  }

  SECTION("Equal operands")
  {
    CHECK_FALSE(cyclic_less_than(1, 1, 1));
    CHECK(cyclic_less_than(1, 1, 2));
    CHECK_FALSE(cyclic_less_than(1, 2, 1));
    CHECK_FALSE(cyclic_less_than(2, 1, 1));
  }

  SECTION("With user provided less_than functor")
  {
    std::string operand_1 = "31";
    std::string operand_2 = "22";
    std::string operand_3 = "13";

    auto less_than = [](const std::string& a, const std::string& b) { return a[1] < b[1]; };

    SECTION("All distinct")
    {
      CHECK(cyclic_less_than(operand_1, operand_2, operand_3, less_than));
      CHECK(cyclic_less_than(operand_2, operand_3, operand_1, less_than));
      CHECK(cyclic_less_than(operand_3, operand_1, operand_2, less_than));

      CHECK_FALSE(cyclic_less_than(operand_2, operand_1, operand_3, less_than));
      CHECK_FALSE(cyclic_less_than(operand_3, operand_2, operand_1, less_than));
      CHECK_FALSE(cyclic_less_than(operand_1, operand_3, operand_2, less_than));
    }

    SECTION("Equal operands")
    {
      CHECK_FALSE(cyclic_less_than(operand_1, operand_1, operand_1, less_than));
      CHECK(cyclic_less_than(operand_1, operand_1, operand_2, less_than));
      CHECK_FALSE(cyclic_less_than(operand_1, operand_2, operand_1, less_than));
      CHECK_FALSE(cyclic_less_than(operand_2, operand_1, operand_1, less_than));
    }
  }
}

} // namespace dida