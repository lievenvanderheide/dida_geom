#include "dida/segment2.hpp"

#include <iostream>

namespace dida
{

std::ostream& operator<<(std::ostream& s, Segment2 segment)
{
  return s << '{' << segment.start() << ", " << segment.end() << '}';
}

} // namespace dida