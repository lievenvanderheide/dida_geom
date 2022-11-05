#include "dida/utils.hpp"

#include <catch2/catch.hpp>
#include <iostream>

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

} // namespace dida