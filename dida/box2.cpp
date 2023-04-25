#include "dida/box2.hpp"

#include <iostream>

namespace dida
{

std::ostream& operator<<(std::ostream& s, Box2 v)
{
  s << "{" << v.min() << ", " << v.max() << "}";
  return s;
}

} // namespace dida