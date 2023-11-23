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

template <class Container, class Iterator>
size_t distance_cyclic(const Container& container, Iterator a, Iterator b)
{
  if (a <= b)
  {
    return static_cast<size_t>(b - a);
  }
  else
  {
    // b - a + container.size() => container.size() - (a - b)
    return container.size() - static_cast<size_t>(a - b);
  }
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

template <class IntType>
IntType add_modulo(IntType a, IntType b, IntType m)
{
  DIDA_DEBUG_ASSERT(a >= 0 && a < m);
  DIDA_DEBUG_ASSERT(b >= 0 && b <= m);

  a += b;
  return a < m ? a : a - m;
}

template <class IntType>
IntType sub_modulo(IntType a, IntType b, IntType m)
{
  DIDA_DEBUG_ASSERT(a >= 0 && a < m);
  DIDA_DEBUG_ASSERT(b >= 0 && b <= m);
  
  return a >= b ? a - b : m + a - b;
}

template <class T>
bool cyclic_less_than(const T& base, const T& a, const T& b)
{
  return cyclic_less_than(base, a, b, std::less<T>());
}

template <class T, class LessThan>
bool cyclic_less_than(const T& base, const T& a, const T& b, LessThan less_than)
{
  // The result is true iff one of the following holds
  //
  //  - base <= a < b 
  //  - a < b < base
  //  - b < base <= a

  if (less_than(a, base))
  {
    return less_than(a, b) && less_than(b, base);
  }
  else
  {
    return less_than(a, b) || less_than(b, base);
  }
}

} // namespace dida