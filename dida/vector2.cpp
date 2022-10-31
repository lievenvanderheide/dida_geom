#include "dida/vector2.hpp"

#include <iostream>

namespace dida
{

std::ostream& operator<<(std::ostream& s, Vector2 v)
{
  return s << '{' << v.x() << ", " << v.y() << '}';
}

} // namespace dida