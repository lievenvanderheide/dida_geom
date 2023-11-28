#pragma once

#include <cstddef>

namespace dida
{

/// Returns an iterator to the previous element from @c it, while wrapping around if the beginning of the container is
/// reached.
template <class Container, class Iterator>
Iterator prev_cyclic(Container& container, Iterator it);

/// Returns an iterator to the next element from @c it, while wrapping around if the end of the container is reached.
template <class Container, class Iterator>
Iterator next_cyclic(Container& container, Iterator it);

/// Returns the distance from @c a to @c b in a cyclically ordered set. The result is the number of times @c next_cyclic
/// has to be called to get from @c a to @c b.
template <class Container, class Iterator>
size_t distance_cyclic(const Container& container, Iterator a, Iterator b);

/// Returns the predecessor of @c a modulo @c m.
///
/// @pre @c a should be in the range <tt>0 <= a < m</tt>.
template <class IntType>
IntType pred_modulo(IntType a, IntType m);

/// Returns the successor of @c a modulo @c m.
///
/// @pre @c a should be in the range <tt>0 <= a < m</tt>.
template <class IntType>
IntType succ_modulo(IntType a, IntType m);

/// Computes <tt>a + b</tt> modulo m. Operand @c a must already be in the range <tt>0 <= a < m</tt>, and operand @c b
/// in the range <tt>0 <= b <= m</tt> (note that @c b is allowed to be equal to @c m).
template <class IntType>
IntType add_modulo(IntType a, IntType b, IntType m);

/// Computes <tt>a - b</tt> modulo m. Operand @c a must already be in the range <tt>0 <= a < m</tt>, and operand @c b
/// in the range <tt>0 <= b <= m</tt> (note that @c b is allowed to be equal to @c m).
template <class IntType>
IntType sub_modulo(IntType a, IntType b, IntType m);

/// Returns whether @c a comes before @c b when iterating over a cyclic set starting from @c base.
template <class T>
bool cyclic_less_than(const T& base, const T& a, const T& b);

/// An overload of @c cyclic_order which takes a user provided @c less_than functor.
template <class T, class LessThan>
bool cyclic_less_than(const T& base, const T& a, const T& b, LessThan less_than);

} // namespace dida

#include "dida/utils.inl"