#include "dida/utils.hpp"

#include <catch2/catch.hpp>

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

TEST_CASE("cyclic_order")
{
  SECTION("All distinct")
  {
    CHECK(cyclic_order(1, 2, 3));
    CHECK(cyclic_order(2, 3, 1));
    CHECK(cyclic_order(3, 1, 2));

    CHECK_FALSE(cyclic_order(2, 1, 3));
    CHECK_FALSE(cyclic_order(3, 2, 1));
    CHECK_FALSE(cyclic_order(1, 3, 2));
  }

  SECTION("Equal operands")
  {
    CHECK_FALSE(cyclic_order(1, 1, 1));
    CHECK_FALSE(cyclic_order(1, 1, 2));
    CHECK_FALSE(cyclic_order(1, 2, 1));
    CHECK_FALSE(cyclic_order(2, 1, 1));
  }

  SECTION("With user provided less_than functor")
  {
    std::string operand_1 = "31";
    std::string operand_2 = "22";
    std::string operand_3 = "13";

    auto less_than = [](const std::string& a, const std::string& b) { return a[1] < b[1]; };

    SECTION("All distinct")
    {
      CHECK(cyclic_order(operand_1, operand_2, operand_3, less_than));
      CHECK(cyclic_order(operand_2, operand_3, operand_1, less_than));
      CHECK(cyclic_order(operand_3, operand_1, operand_2, less_than));

      CHECK_FALSE(cyclic_order(operand_2, operand_1, operand_3, less_than));
      CHECK_FALSE(cyclic_order(operand_3, operand_2, operand_1, less_than));
      CHECK_FALSE(cyclic_order(operand_1, operand_3, operand_2, less_than));
    }

    SECTION("Equal operands")
    {
      CHECK_FALSE(cyclic_order(operand_1, operand_1, operand_1, less_than));
      CHECK_FALSE(cyclic_order(operand_1, operand_1, operand_2, less_than));
      CHECK_FALSE(cyclic_order(operand_1, operand_2, operand_1, less_than));
      CHECK_FALSE(cyclic_order(operand_2, operand_1, operand_1, less_than));
    }
  }
}

} // namespace dida