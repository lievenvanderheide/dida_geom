#include "dida/assert.hpp"

#include <functional>

namespace dida
{

template <class Container, class Iterator>
Iterator prev_cyclic(Container& container, Iterator it)
{
  return it != container.begin() ? it - 1 : container.end() - 1;
}

template <class Container, class Iterator>
Iterator next_cyclic(Container& container, Iterator it)
{
  ++it;
  return it != container.end() ? it : container.begin();
}

template <class IntType>
IntType pred_modulo(IntType a, IntType m)
{
  DIDA_DEBUG_ASSERT(a >= 0 && a < m);
  DIDA_DEBUG_ASSERT(m > 0);

  return a != 0 ? a - 1 : m - 1;
}

template <class IntType>
IntType succ_modulo(IntType a, IntType m)
{
  DIDA_DEBUG_ASSERT(a >= 0 && a < m);
  DIDA_DEBUG_ASSERT(m > 0);

  a++;
  return a != m ? a : 0;
}

template <class T>
bool cyclic_order(const T& a, const T& b, const T& c)
{
  return cyclic_order(a, b, c, std::less<T>());
}

template <class T, class LessThan>
bool cyclic_order(const T& a, const T& b, const T& c, LessThan less_than)
{
  if (less_than(a, b))
  {
    return less_than(c, a) || less_than(b, c);
  }
  else
  {
    return less_than(b, c) && less_than(c, a);
  }
}

} // namespace dida